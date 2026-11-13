#include "init.h"

void GPIO_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIODEN;
    (void)RCC->AHB1ENR;
}

int RCC_Init(void) {
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while(READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    
    // !!! КРИТИЧЕСКИ ВАЖНО ДЛЯ NUCLEO-144 !!!
    // Так как кварц не распаян, используем сигнал 8 МГц от ST-LINK (MCO)
    SET_BIT(RCC->CR, RCC_CR_HSEBYP); 
    
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET); // Теперь здесь не зависнет
    
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);
    
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); // Делитель M = 4
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7); // N = 180
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); // P = 2
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_Msk, RCC_PLLCFGR_PLLQ_3);
    
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);
    
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    
    SystemCoreClock = 180000000U;
    return 0; // Успех
}

void INIT_UPRAVLENIE(GPIO_TypeDef *port, uint8_t n, uint8_t nf) {
    port->MODER &= ~(0x3U << (n * 2));
    port->MODER |= (0x2U << (n * 2));
    if (n < 8) { 
        port->AFR[0] &= ~(0xFU << (n * 4)); 
        port->AFR[0] |= ((uint32_t)nf << (n * 4)); 
    } else { 
        port->AFR[1] &= ~(0xFU << ((n - 8) * 4)); 
        port->AFR[1] |= ((uint32_t)nf << ((n - 8) * 4)); 
    }
    port->OTYPER &= ~(1U << n);
    port->OSPEEDR &= ~(0x3U << (n * 2));
    port->OSPEEDR |= (2U << (n * 2));
    port->PUPDR &= ~(0x3U << (n * 2));
}

// ИСПРАВЛЕНО: Убрали жесткий CCR = 2000, теперь там 0, чтобы управлял Driver.c
void INIT_TIM(volatile uint32_t *rcc_reg, uint32_t bit, TIM_TypeDef *TIMX, uint32_t fp, uint8_t chan) {
    if (chan < 1U || chan > 4U) return;
    *rcc_reg |= bit;
    (void)*rcc_reg;
    
    TIMX->PSC = fp - 1; 
    TIMX->CR1 |= (1U << 7); // ARPE
    TIMX->ARR = 0xFFFFU;

    if(chan == 1) {
        TIMX->CCMR1 &= ~0xFFU; TIMX->CCMR1 |= 0x68;
        TIMX->CCR1 = 0; TIMX->CCER |= (1U << 0); 
    } else if (chan == 2) {
        TIMX->CCMR1 &= ~0xFF00U; TIMX->CCMR1 |= 0x6800;
        TIMX->CCR2 = 0; TIMX->CCER |= (1U << 4); 
    } else if(chan == 3) {
        TIMX->CCMR2 &= ~0xFFU; TIMX->CCMR2 |= 0x68;
        TIMX->CCR3 = 0; TIMX->CCER |= (1U << 8); 
    } else if (chan == 4) {
        TIMX->CCMR2 &= ~0xFF00U; TIMX->CCMR2 |= 0x6800;
        TIMX->CCR4 = 0; TIMX->CCER |= (1U << 12); 
    }
    
    if (TIMX == TIM1 || TIMX == TIM8) { TIMX->BDTR |= (1U << 15); }
    
    TIMX->EGR = (1U << 0);
    TIMX->SR = ~TIM_SR_UIF;
}