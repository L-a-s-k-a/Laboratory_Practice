#include "../Inc/init.h"
#include <stdint.h>
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429ZI/stm32f429xx.h"
#include "stm32f4xx.h"
#include "../Inc/it_handlers.h"
#define DELAY_BUTTON_FILTER 200  // уменьшили время фильтра дребезга


// void EXTI3_IRQHandler(void)
// {
//     if (READ_BIT(EXTI->PR, EXTI_PR_PR3)) 
//     {
//         SET_BIT(EXTI->PR, EXTI_PR_PR3); 
        
//         if (systemTickCount >= DELAY_BUTTON_FILTER)
//         {
//             if (buttonState == 0)
//             {
//                 buttonPressTime = systemTickCount;
//                 buttonState = 1;
//                 longPressTriggered = 0; 

//             }
//             else
//             {
//                 holdTime = systemTickCount - buttonPressTime;
                
//                 if (holdTime > 200 && holdTime <= 2000 && !longPressTriggered)
//                 {
//                     shortState = 1;
//                 }
                
//                 buttonState = 0;
//                 systemTickCount = 0;
//             }
//         }
//     }
// }

void SysTick_Handler(void)
{
    systemTickCount++;
     
    // if (buttonState == 1 && !longPressTriggered)
    // {
    //     holdTime = systemTickCount - buttonPressTime;
    //     if (holdTime > 2000) 
    //     {
    //         mode=!mode;
             
    //         longPressTriggered = 1; 
    //     }
        
    // }
}


void ADC_IRQHandler(void)
{
    flag = 1;
    SET_BIT(ADC1->CR2, ADC_CR2_SWSTART); 
    adc =  ADC1->DR; 
    CLEAR_BIT(ADC1->SR, ADC_SR_EOC);
}