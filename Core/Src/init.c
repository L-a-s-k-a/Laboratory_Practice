    #include "../Inc/init.h"
    #include <stdint.h>
    #include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429ZI/stm32f429xx.h"
    #include "stm32f4xx.h"
    void RCC_Ini(void)
    {
   /* Предварительная очистка регистров RCC */
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
    /* Настройка главного регистра RCC */
    SET_BIT(RCC->CR, RCC_CR_HSEON);                                                    //Запускаем внешний кварцевыйрезонатор
    while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);                                  //Ждём пока он запустится
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);                                                 //Сбросим бит байпаса в 0
    SET_BIT(RCC->CR, RCC_CR_CSSON);                                                    //Запустим Clock detector
    /* Натсройка регистров PLL
    * Предварительная очистка регистра
    * Включаем работу PLL*/
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 |RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7);
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk);                                     //Настраивае мпредделитель получившейся частоты после умножения. Иными словами,получаем итоговую частоту PLL
    SET_BIT(RCC->CR, RCC_CR_PLLON);                                                    //Запустим PLL
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY));                                           //Ждём запуска PLL84
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL); 
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);
    }

    void GPIO_Init(void)
    {
       
        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOEEN);

        // PA0 (TIM2_CH1) - AF1
        MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER0, GPIO_MODER_MODER0_1);
        MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPDR0, GPIO_PUPDR_PUPDR0_0);
        MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL0, 0x01 << GPIO_AFRL_AFSEL0_Pos);

        // PB3 (TIM2_CH2) - AF1
        MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER3, GPIO_MODER_MODER3_1);
        MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPDR3, GPIO_PUPDR_PUPDR3_0);
        MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL3, 0x01 << GPIO_AFRL_AFSEL3_Pos);

        // PB1 Analog (ADC1_IN9)
        MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER1, GPIO_MODER_MODER1);
        MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPDR1, 0);


        
        
        CLEAR_BIT(GPIOE->MODER, GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER9 | GPIO_MODER_MODER11 | GPIO_MODER_MODER13 | GPIO_MODER_MODER14);
        SET_BIT(GPIOE->MODER,  GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER9_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1);

        MODIFY_REG(GPIOE->AFR[0], GPIO_AFRL_AFSEL5, 0x03 << GPIO_AFRL_AFSEL5_Pos);      // TIM9 CH1
        MODIFY_REG(GPIOE->AFR[0], GPIO_AFRL_AFSEL6, 0x03 << GPIO_AFRL_AFSEL6_Pos);      // TIM9 CH2

        MODIFY_REG(GPIOE->AFR[1], GPIO_AFRH_AFSEL9,  0x01 << GPIO_AFRH_AFSEL9_Pos);     // TIM1 CH1
        MODIFY_REG(GPIOE->AFR[1], GPIO_AFRH_AFSEL11, 0x01 << GPIO_AFRH_AFSEL11_Pos);    // TIM1 CH2
        MODIFY_REG(GPIOE->AFR[1], GPIO_AFRH_AFSEL13, 0x01 << GPIO_AFRH_AFSEL13_Pos);    // TIM1 CH3
        MODIFY_REG(GPIOE->AFR[1], GPIO_AFRH_AFSEL14, 0x01 << GPIO_AFRH_AFSEL14_Pos);    // TIM1 CH4
        MODIFY_REG(GPIOE->MODER, GPIO_MODER_MODER5, GPIO_MODER_MODER5_1);               
        MODIFY_REG(GPIOE->AFR[0], GPIO_AFRL_AFSEL5, 0x03 << GPIO_AFRL_AFSEL5_Pos);      // AF3  TIM9
    }

void TIM_Init(void)
{
 
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN);

    TIM2->PSC = 0;
    TIM2->ARR = 0xFFFFFFFF;
    MODIFY_REG(TIM2->CCMR1, TIM_CCMR1_CC1S | TIM_CCMR1_CC2S, TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0);
    MODIFY_REG(TIM2->CCMR1, TIM_CCMR1_IC1F | TIM_CCMR1_IC2F, (0xF << TIM_CCMR1_IC1F_Pos) | (0xF << TIM_CCMR1_IC2F_Pos));
    MODIFY_REG(TIM2->SMCR, TIM_SMCR_SMS, TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1);
    
    
    MODIFY_REG(TIM2->CCMR1, TIM_CCMR1_OC1M, 0x6 << TIM_CCMR1_OC1M_Pos); 
    SET_BIT(TIM2->CCMR1, TIM_CCMR1_OC1PE);
    CLEAR_BIT(TIM2->CCER, TIM_CCER_CC1E); 
    
    SET_BIT(TIM2->CR1, TIM_CR1_CEN);

    
    TIM1->PSC = 179;
    TIM1->ARR = 1000 - 1;
    
    
    MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC2M, 0x6 << TIM_CCMR1_OC2M_Pos);
    SET_BIT(TIM1->CCMR1, TIM_CCMR1_OC2PE);
    SET_BIT(TIM1->CCER, TIM_CCER_CC2E);
    
    
    MODIFY_REG(TIM1->CCMR2, TIM_CCMR2_OC3M, 0x6 << TIM_CCMR2_OC3M_Pos);
    SET_BIT(TIM1->CCMR2, TIM_CCMR2_OC3PE);
    SET_BIT(TIM1->CCER, TIM_CCER_CC3E);
    
    MODIFY_REG(TIM1->CCMR2, TIM_CCMR2_OC4M, 0x6 << TIM_CCMR2_OC4M_Pos);
    SET_BIT(TIM1->CCMR2, TIM_CCMR2_OC4PE);
    SET_BIT(TIM1->CCER, TIM_CCER_CC4E);
    
    SET_BIT(TIM1->BDTR, TIM_BDTR_MOE);
    SET_BIT(TIM1->CR1, TIM_CR1_ARPE);
    SET_BIT(TIM1->CR1, TIM_CR1_CEN);
   
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN); 

    GPIOB->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5);
    
    GPIOB->MODER |= (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1);

    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL4);          
    GPIOB->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL4_Pos); 
    
    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL5);          
    GPIOB->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL5_Pos); 

    TIM3->PSC = 179; 
    TIM3->ARR = 1000-1; 
    
    TIM3->CCMR1 &= ~TIM_CCMR1_OC2M;
    TIM3->CCMR1 |= (0x6 << TIM_CCMR1_OC2M_Pos); 
    TIM3->CCMR1 |= TIM_CCMR1_OC2PE;             

    SET_BIT(TIM3->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);

    SET_BIT(TIM3->CR1, TIM_CR1_ARPE | TIM_CR1_CEN);
}


void ADC_Init(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);

    CLEAR_BIT(ADC1->CR2, ADC_CR2_ADON); 
    SET_BIT(ADC->CCR, ADC_CCR_ADCPRE_0); 
    MODIFY_REG(ADC1->SQR1, ADC_SQR1_L, 0); 
    MODIFY_REG(ADC1->SQR3, ADC_SQR3_SQ1, 9 << ADC_SQR3_SQ1_Pos); 
    MODIFY_REG(ADC1->SMPR2, ADC_SMPR2_SMP9, 4 << ADC_SMPR2_SMP9_Pos); 
    SET_BIT(ADC1->CR2, ADC_CR2_ADON); 
    

}   
    void ITR_init(void)
{ 
    
    SET_BIT(ADC1->CR1, ADC_CR1_EOCIE);
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_SetPriority(ADC_IRQn, 0);
}

    void EXTI_ITR_Init(void)
    {
       SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);                                      // Включение тактирования SYSCFG

    

        MODIFY_REG(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI3_Msk, SYSCFG_EXTICR1_EXTI3_PC); //Настройка мультиплексора на вывод линии прерывания EXTI3 на PC3
        SET_BIT(EXTI->IMR, EXTI_IMR_MR3);                                                 //Настройка маскирования 3 линии
        SET_BIT(EXTI->RTSR, EXTI_RTSR_TR3);                                               //Настройка детектирования нарастающего фронта 3 линии
        SET_BIT(EXTI->FTSR, EXTI_FTSR_TR3);                                               //Настройка детектирования спадающего фронта 3 линии
  
        NVIC_SetPriority(EXTI3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));//Установка 0 приоритета прерывания для вектора EXTI3
        NVIC_EnableIRQ(EXTI3_IRQn);                                                         //Включение прерывания по вектору EXTI3
    }
    // Функция инициализации системного таймера
    void SysTick_Init(void)
    {
        CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                    //На всякий случай предварительно выключим счётчик
        SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);                                     //Разрешаем прерывание по системному таймеру
        SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);                                   //Источник тактирования будет идти из AHB без деления
        MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk,17999 << SysTick_LOAD_RELOAD_Pos);  //Будет тикать с частотой 1 кГц и вызывать прерывания
        MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk,0 << SysTick_VAL_CURRENT_Pos);       //Начнём считать со значения 179999
        SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                      //Включим счётчик
    }