#include "main.h"
#include "gpio.h"

volatile uint8_t button1, button2;
volatile int period, flag_button1, flag_button2, period_bt2, bt1_state,
	bt2_state, bt2_change;
volatile int counter_tim2;
volatile int mas[6];
volatile int once, bt1_current;
volatile int zhopa = 0;

void RCC_init()
{
	SET_BIT(RCC->CR, RCC_CR_HSION);
	while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0)
		;
	MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, RCC_CR_HSITRIM_3);
	SET_BIT(PWR->CR, PWR_CR_VOS);
	CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
	while (READ_BIT(RCC->CR, RCC_CR_PLLRDY))
		;
	CLEAR_REG(RCC->PLLCFGR);
	// Настройка источника PLL: HSI
	SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSI);
	// Установка M = 8
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, 8 << RCC_PLLCFGR_PLLM_Pos);
	// Установка N = 100
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, 100 << RCC_PLLCFGR_PLLN_Pos);
	CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP);
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_Msk, 4 << RCC_PLLCFGR_PLLQ_Pos);
	SET_BIT(RCC->CR, RCC_CR_PLLON);
	while (READ_BIT(RCC->CR, RCC_CR_PLLRDY))
		;
	MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
	MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);	// AHB = SYSCLK / 1
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2); // APB1 = AHB / 2
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1); // APB2 = AHB / 1
	MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
}

void GPIO_init()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

	GPIOA->MODER |= (1 << (1 * 2)) | (1 << (2 * 2)) | (1 << (3 * 2)) | (1 << (4 * 2)) | (1 << (5 * 2)) | (1 << (6 * 2));
	GPIOA->OTYPER &= ~((1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6));
	GPIOA->OSPEEDR |= (3 << (1 * 2)) | (3 << (2 * 2)) | (3 << (3 * 2)) | (3 << (4 * 2)) | (3 << (5 * 2)) | (3 << (6 * 2));
	GPIOA->PUPDR &= ~((3 << (1 * 2)) | (3 << (2 * 2)) | (3 << (3 * 2)) | (3 << (4 * 2)) | (3 << (5 * 2)) | (3 << (6 * 2)));

	GPIOA->MODER &= ~(3 << (0 * 2));
	GPIOA->PUPDR |= (1 << (0 * 2));
	GPIOB->MODER &= ~(3 << (1 * 2));
	GPIOB->PUPDR |= (1 << (1 * 2));
}

void TIM2_init()
{
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
	CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN);
	TIM2->PSC = 1000 - 1;
	TIM2->ARR = 20000 - 1;
	CLEAR_BIT(TIM2->CR1, TIM_CR1_DIR);
	CLEAR_BIT(TIM2->CR1, TIM_CR1_CKD);
	CLEAR_BIT(TIM2->CR1, TIM_CR1_ARPE);
	CLEAR_BIT(TIM2->SMCR, TIM_SMCR_SMS);
	CLEAR_REG(TIM2->CR2);
	MODIFY_REG(TIM2->CR2, TIM_CR2_MMS, 0x0);
	CLEAR_BIT(TIM2->SMCR, TIM_SMCR_MSM);
	SET_BIT(TIM2->DIER, TIM_DIER_UIE);
	SET_BIT(TIM2->CR1, TIM_CR1_CEN);
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn, 1);
}

void TIM3_init()
{
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);
	CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);
	TIM3->PSC = 1000 - 1;
	TIM3->ARR = 1000 - 1;
	CLEAR_BIT(TIM3->CR1, TIM_CR1_DIR);
	CLEAR_BIT(TIM3->CR1, TIM_CR1_CKD);
	CLEAR_BIT(TIM3->CR1, TIM_CR1_ARPE);
	CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS);
	CLEAR_REG(TIM3->CR2);
	MODIFY_REG(TIM3->CR2, TIM_CR2_MMS, 0x0);
	CLEAR_BIT(TIM3->SMCR, TIM_SMCR_MSM);
	SET_BIT(TIM3->DIER, TIM_DIER_UIE);
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);

	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_SetPriority(TIM3_IRQn, 1);
}
int main(void)
{
	RCC_init();
	GPIO_init();
	TIM2_init();
	TIM3_init();
	bt2_state = 1;
	while (1)
	{
		button1 = GPIO_ReadPin(GPIOA, 0);
		button2 = GPIO_ReadPin(GPIOB, 1);
		if (button1)
		{
			flag_button1 = 1;
			if (period > DELAY)
			{
				flag_button1 = 0;
			}
		}
		if (button2)
		{
			flag_button2 = 1;
		}
		else
		{
			flag_button2 = 0;
		}
	}
}
