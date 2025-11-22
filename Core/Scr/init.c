#include "../Inc/init.h"

static uint16_t i = 6;
extern uint16_t GlobalTickCount;
static uint16_t Buf1 = 0;
static uint16_t Buf2 = 0;
static uint16_t Buf3 = 0;
static uint8_t Led1_Merz = 0;

void RCC_Ini(void)
{
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80UL);
    CLEAR_REG(RCC->CFGR);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);

    SET_BIT(RCC->CR, RCC_CR_HSION);
    while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET);

    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSI);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_3);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_5);
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2);

    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_HPRE_DIV1);
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE1_DIV2);
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE2_DIV1);
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_1);

    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_3WS);

    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);
}
void GPIO_Ini(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE0_0);
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_0);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0_0);

    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE1_0);
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_1);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR1_0);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR1_0);

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0);

    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE5_0);
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_5);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR5_0);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR5_0);

    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE6_0);
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_6);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR6_0);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR6_0);

    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE8_0);
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_8);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR8_0);

    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER9_1);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk);
    MODIFY_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL9_Msk, 0x00UL);
}
void EXTI_ITR_Ini(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, SYSCFG_EXTICR4_EXTI13_PC);
    SET_BIT(EXTI->IMR, EXTI_IMR_MR13);
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13);
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR13);
    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}
void SysTick_Init(void)
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 95999 << SysTick_LOAD_RELOAD_Pos);
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 95999 << SysTick_VAL_CURRENT_Pos);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}

void LedPush(uint16_t led[6][2])
{
    i++;
    if (i == 7)
    {
        i = 0;
    }
}

void LedMerz(uint16_t led[6][2])
{
    if (i != 6)
    {
        if (led[i][0] == 1)
        {
            if (led[i][1] == 0)
            {
                led[i][1] = 1;
            }
            else if (led[i][1] == 1)
            {
                led[i][1] = 2;
            }
            else if (led[i][1] == 2)
            {
                led[i][1] = 0;
            }
        }
    }
}

void LedOn(uint16_t led[6][2])
{
    if (i != 6)
    {
        if (led[i][0] == 0)
        {
            led[i][0] = 1;
        }
        else if (led[i][0] == 1)
        {
            led[i][0] = 0;
        }
    }
}

void Work(uint16_t led[6][2])
{
    uint16_t j = 0;
    if (i == 6)
    {
        while (j < 6)
        {
            Led1_Merz = 0;
            WorkHelpOff(j);
            j++;
        }
        j = 0;
    }
    while ((j <= i) && (i != 6))
    {
        if (led[j][0] == 0)
        {
            Led1_Merz = 1;
            WorkHelpOn(j);
        }
        else if (led[j][1] == 0)
        {
            if ((GlobalTickCount - Buf1) < 2500)
            {
                Led1_Merz = 1;
                WorkHelpOn(j);
            }
            else
            {
                Led1_Merz = 0;
                WorkHelpOff(j);
                if ((GlobalTickCount - Buf1) > (2500 * 2))
                {
                    Buf1 = GlobalTickCount;
                }
            }
        }
        else if (led[j][1] == 1)
        {
            if ((GlobalTickCount - Buf2) < 909)
            {
                Led1_Merz = 1;
                WorkHelpOn(j);
            }
            else
            {
                Led1_Merz = 0;
                WorkHelpOff(j);
                if ((GlobalTickCount - Buf2) > (909 * 2))
                {
                    Buf2 = GlobalTickCount;
                }
            }
        }
        else if (led[j][1] == 2)
        {
            if ((GlobalTickCount - Buf3) < 526)
            {
                Led1_Merz = 1;
                WorkHelpOn(j);
            }
            else
            {
                Led1_Merz = 0;
                WorkHelpOff(j);
                if ((GlobalTickCount - Buf3) > (526 * 2))
                {
                    Buf3 = GlobalTickCount;
                }
            }
        }
        j++;
    }
}

void WorkHelpOn(uint16_t ValLed)
{
    switch(ValLed)
    {
        case 0: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS0); break;
        case 1: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS1); break;
        case 2: SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0); break;
        case 3: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS5); break;
        case 4: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS6); break;
        case 5: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS8); break;
    }
}

void WorkHelpOff(uint16_t ValLed)
{
    switch(ValLed)
    {
        case 0: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR0); break;
        case 1: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR1); break;
        case 2: SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0); break;
        case 3: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR5); break;
        case 4: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR6); break;
        case 5: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR8); break;
    }
}

void Clear(void)
{
    if (Buf1 > GlobalTickCount || Buf2 > GlobalTickCount || Buf3 > GlobalTickCount)
    {
        Buf1 = GlobalTickCount;
        Buf2 = GlobalTickCount;
        Buf3 = GlobalTickCount;
    }
}

