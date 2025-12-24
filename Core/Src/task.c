#include "task.h"
#include "handlers.h"


void task1(void){
    if(btn9_pressed){
        pwm_mode = (pwm_mode + 1) % 6
        ;
        btn9_pressed=0;
          Switch_PWM();
    }
    if(btn8_pressed){
        counter1 = (counter1 + 1) % 5;
        btn8_pressed = 0;
        Set_All_LEDs_Brightness();
    }
}


void Switch_PWM(void)
{
    // ВЫКЛЮЧАЕМ ВСЕ КАНАЛЫ
    CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);
    CLEAR_BIT(TIM2->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);
     CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);
    
    
    // ВКЛЮЧАЕМ ТОЛЬКО ОДИН КАНАЛ
    switch(pwm_mode) {
        case 0: 
            SET_BIT(TIM2->CCER, TIM_CCER_CC1E);  
            break;
            
        case 1: 
            SET_BIT(TIM2->CCER, TIM_CCER_CC2E);  
            break;
            
        case 2: 
            SET_BIT(TIM1->CCER, TIM_CCER_CC1E); 
            break;
            
        case 3: 
            SET_BIT(TIM1->CCER, TIM_CCER_CC2E);
            break;
            
        case 4: 
            SET_BIT(TIM3->CCER, TIM_CCER_CC1E);  
            break;
            
        case 5: 
            SET_BIT(TIM3->CCER, TIM_CCER_CC2E);  
            break;
    }
    
    Set_All_LEDs_Brightness();
}


void Set_All_LEDs_Brightness(void)
{
    uint32_t duty_cycle;
    
    switch(counter1) {
        case 0: 
            duty_cycle = 0;
            break;
        case 1: // 25%
            duty_cycle = 200;   // 799 * 0.25 ≈ 200
            break;
        case 2: // 50%
            duty_cycle = 400;   // 799 * 0.5 ≈ 400
            break;
        case 3: // 75%
            duty_cycle = 600;   // 799 * 0.75 ≈ 600
            break;
        case 4: // 100%
            duty_cycle = 799;   // Полная яркость
            break;
        default:
            duty_cycle = 0;
            break;
    }
    MODIFY_REG(TIM3->CCR1, TIM_CCR1_CCR1_Msk, duty_cycle);
    MODIFY_REG(TIM3->CCR2, TIM_CCR2_CCR2_Msk, duty_cycle);

    MODIFY_REG(TIM2->CCR1, TIM_CCR1_CCR1_Msk, duty_cycle);  
    MODIFY_REG(TIM2->CCR2, TIM_CCR2_CCR2_Msk, duty_cycle); 
    
    MODIFY_REG(TIM1->CCR1, TIM_CCR1_CCR1_Msk, duty_cycle);
    MODIFY_REG(TIM1->CCR2, TIM_CCR2_CCR2_Msk, duty_cycle);
}