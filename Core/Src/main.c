#include "../Inc/init.h"

#include <stdint.h>
#include "stm32f4xx.h"
#include "init.h"
#include "it_handlers.h"

volatile uint32_t holdTime = 0;
volatile uint8_t shortState = 0;
volatile uint8_t mode = 0;
volatile uint8_t flag = 0;
volatile uint16_t adc = 0;
volatile uint8_t current_led = 0;
volatile uint16_t pwm_value = 500;
volatile uint32_t switch_speed = 500;
volatile uint32_t last_switch_time = 0;
uint16_t prev_encoder_count = 0;
volatile uint32_t systemTickCount = 0;
volatile uint32_t base_time = 1000; 
volatile uint32_t phase_start_time = 0;
volatile uint8_t current_phase = 0;
volatile uint32_t pe13_time = 1000;  
volatile uint32_t pe14_time = 500;    
volatile uint32_t pe11_time = 1000;    
volatile uint32_t pe14_11_time = 500; 



void Buzz(uint16_t frequency)
{
    if (frequency == 0) 
    {
        TIM3->CCR2 = 0; 
        return;
    }

    uint16_t period = 1000000 / frequency;
    TIM3->ARR = period;
    TIM3->CCR2 = period / 2; 
}


uint16_t ADC_Read(void) 
{
    SET_BIT(ADC1->CR2, ADC_CR2_SWSTART);
    while(!(READ_BIT(ADC1->SR, ADC_SR_EOC)));
    return ADC1->DR;
}

int main(void) 
{
    GPIO_Init();
    ADC_Init();
    TIM_Init();
    SysTick_Init();

    TIM2->CNT = 32768;
    prev_encoder_count = 32768;
    phase_start_time = systemTickCount;

    while(1) 
    {
        
        int32_t encoder_count = (int32_t)TIM2->CNT;
        int16_t encoder_delta = (int16_t)(encoder_count - prev_encoder_count);
        prev_encoder_count = encoder_count;

        
        uint16_t adc_value = ADC_Read();
        uint16_t pwm_value = adc_value * 1000 / 4096;

        
        if (encoder_delta != 0)
        {
            pe13_time += encoder_delta * 100;
            if (pe13_time < 100) pe13_time = 100;
            if (pe13_time > 6000) pe13_time = 6000;
            pe14_time += encoder_delta * 100; 
            if (pe14_time < 70) pe14_time = 70;
            if (pe14_time > 3500) pe14_time = 3500;
            pe11_time += encoder_delta * 100; 
            if (pe11_time < 70) pe11_time = 70;
            if (pe11_time > 3500) pe11_time = 3500;
            pe14_11_time += encoder_delta * 100; 
            if (pe14_11_time < 30) pe14_11_time = 30;
            if (pe14_11_time > 1500) pe14_11_time = 1500; 
        }
        

        uint32_t elapsed_time = systemTickCount - phase_start_time;
        uint32_t current_phase_time = 0;
        
        switch(current_phase)
        {
            case 0: current_phase_time = pe13_time; break;
            case 1: current_phase_time = pe11_time; break;
            case 2: current_phase_time = pe14_11_time; break;
        }
        
        if (elapsed_time >= current_phase_time) 
        {
            current_phase = (current_phase + 1) % 3;
            phase_start_time = systemTickCount;
            current_led = current_phase;
        }

        
        switch(current_phase) 
        {
            case 0: 
                TIM1->CCR2 = 0;         
                TIM1->CCR3 = pwm_value; 
                TIM1->CCR4 = 0;         
                Buzz(1500);
                break;
                
            case 1: 
                TIM1->CCR2 = pwm_value; 
                TIM1->CCR3 = 0;        
                TIM1->CCR4 = 0;         
                Buzz(3000);
                break;
                
            case 2: 
                TIM1->CCR2 = pwm_value; 
                TIM1->CCR3 = 0;         
                TIM1->CCR4 = pwm_value; 
                Buzz(4500);
                break;
        }
        
    }
}