#include "init.h"
#include "Driver.h"

// Обработчики прерываний для обоих таймеров 
 void TIM2_IRQHandler(void) {
     Rotation_UGL_IRQHandler(TIM2, &TIM2->CCR4);
     }

 void TIM1_UP_TIM10_IRQHandler(void) {
     Rotation_UGL_IRQHandler(TIM1, &TIM1->CCR4);
 }

// NVIC_EnableIRQ(TIM2_IRQn);      
    
// NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);   
//для разгона
//  INIT_TIM(&RCC->APB1ENR, RCC_APB1ENR_TIM2EN, TIM2, 2, 4);
//  INIT_TIM(&RCC->APB2ENR, RCC_APB2ENR_TIM1EN, TIM1, 2, 4);

//Rotation_OB(TIM1, &TIM1->CCR4, 300, 100, 5); Вращение по оборотам

//Rotation_GC(TIM1, &TIM1->CCR4, 10000, 100, 5); Вращение по частота

// Rotation_UGL_GC(TIM2,&TIM2->CCR4,10000,180.0f,50);
//         delay_ms(3000);
//на высоких оборотах ставить 2
// 
// Rotation_UGL_OB(TIM2,&TIM2->CCR4,300,180.0f,100);
//         delay_ms(3000);


// Rotation_OB(TIM1, &TIM1->CCR4, 300, 100, 5);
//         delay_ms(2000);
//         Motor_Smooth_Stop_OB(TIM1,&TIM1->CCR4,1,1000,10);

int main(void) {
    if (RCC_Init() != 0) {
        while (1) { } 
    }
    
    DWT_Init();
    GPIO_Init();

    INIT_UPRAVLENIE(GPIOB, 11, 1);   // TIM2_CH4
    INIT_UPRAVLENIE(GPIOE, 14, 1);   // TIM1_CH4

     INIT_TIM(&RCC->APB1ENR, RCC_APB1ENR_TIM2EN, TIM2, 2, 4);
     INIT_TIM(&RCC->APB2ENR, RCC_APB2ENR_TIM1EN, TIM1, 2, 4);
    
    
     Sync_Timer_Params(TIM2, 90000000U);
    Sync_Timer_Params(TIM1, 180000000U);

    Motor_Init(10000, 0.036, 3000);

     NVIC_EnableIRQ(TIM2_IRQn);      
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);      

    
    while (1) { 
        Rotation_OB(TIM1, &TIM1->CCR4, 2500, 500, 5);
        delay_ms(2000);
        Motor_Smooth_Stop_OB(TIM1,&TIM1->CCR4,1,500,10);
        delay_ms(1000);
        
        
        
}
}