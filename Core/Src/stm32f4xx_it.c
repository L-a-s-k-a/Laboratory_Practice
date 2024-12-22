#include "main.h"
#include "stm32f4xx_it.h"
#include "gpio.h"

#define SINGLE_DOUBLE_TIME 400
#define LONG_TIME 2000
#define DEBOUNCE_DELAY 200
#define HZ_0 500000
#define HZ_1 100000
#define HZ_2 50000

extern int enc, button, mas[6];
extern uint32_t counter_tim2, on_time, counter, counter_tim3;
extern uint8_t click_count, button_click_flag, current_state, current_led,
	btn_flag;
int flag_turn;
void TIM2_IRQHandler(void)
{
	if (READ_BIT(TIM2->SR, TIM_SR_UIF))
	{
		CLEAR_BIT(TIM2->SR, TIM_SR_UIF);
		if (button == 0)
		{
			btn_flag = 1;
		}
		if (btn_flag)
		{
			counter_tim2++;
			if (counter_tim2 < SINGLE_DOUBLE_TIME)
			{
				if (counter_tim2 > DEBOUNCE_DELAY && button == 0 && button_click_flag == 0)
				{
					button_click_flag = 1;
					click_count = 2;
				}
				if (counter_tim2 > DEBOUNCE_DELAY && button == 1 && button_click_flag == 0)
				{
					click_count = 1;
				}
			}
			if (counter_tim2 > SINGLE_DOUBLE_TIME && button == 1)
			{
				if (click_count >= 2)
				{
					button_click_flag = 0;
					mas[current_led - 1] = (mas[current_led - 1] + 1) % 3;
					click_count = 0;
					btn_flag = 0;
					counter_tim2 = 0;
				}
				else if (click_count <= 1)
				{
					current_state = !current_state;
					click_count = 0;
					btn_flag = 0;
					counter_tim2 = 0;
				}
			}
			if (counter_tim2 > LONG_TIME && button == 0)
			{
				counter_tim2 = 0;
				click_count = 0;
				btn_flag = 0;
				button_click_flag = 0;
				current_led++;
				if (current_led > 6)
				{
					current_led = 0;
				}
			}
		}
	}
}

void TIM3_IRQHandler(void)
{
	if (READ_BIT(TIM3->SR, TIM_SR_UIF))
	{
		CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
		counter_tim3++;
		on_time = (enc * 100) / 32;
		counter++;
		if (mas[current_led - 1] == 0)
		{
			if (counter_tim3 > HZ_0)
			{
				counter_tim3 = 0;
				flag_turn = !flag_turn;
			}
		}
		if (mas[current_led - 1] == 1)
		{
			if (counter_tim3 > HZ_1)
			{
				counter_tim3 = 0;
				flag_turn = !flag_turn;
			}
		}
		if (mas[current_led - 1] == 2)
		{
			if (counter_tim3 > HZ_2)
			{
				counter_tim3 = 0;
				flag_turn = !flag_turn;
			}
		}

		if (flag_turn && current_state == 1)
		{
			if (counter <= on_time)
			{
				for (uint8_t i = 1; i <= 7; i++)
				{
					if (i == current_led)
					{
						GPIO_WritePin(GPIOA, i, 1);
					}
					else
					{
						GPIO_WritePin(GPIOA, i, 0);
					}
				}
			}
			else
			{
				for (uint8_t i = 1; i <= 7; i++)
				{
					GPIO_WritePin(GPIOA, i, 0);
				}
			}
		}
		else
		{
			for (uint8_t i = 1; i <= 7; i++)
			{
				GPIO_WritePin(GPIOA, i, 0);
			}
		}
		if (counter >= 100)
		{
			counter = 0;
		}
	}
}
