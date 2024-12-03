#include "main.h"
#include "stm32f4xx_it.h"
#include "gpio.h"

extern uint8_t button1, button2;
extern int period, flag_button1, flag_button2, period_bt2, bt1_state, bt2_state,
		bt2_change;
extern int counter_tim2;
extern int mas[6];
extern int once, bt1_current;

void TIM2_IRQHandler(void) {
	if (READ_BIT(TIM2->SR, TIM_SR_UIF)) {
		CLEAR_BIT(TIM2->SR, TIM_SR_UIF);
		if (bt1_state != 0) {
			if (bt1_state != bt1_current) {
				once = 0;
			}
			bt1_current = bt1_state;
			for (uint8_t i = 1; i <= 7; i++) {
				if (i == bt1_state) {
					continue;
				} else {
					GPIO_WritePin(GPIOA, i, 0);
				}
			}
			if (once == 0) {
				GPIO_WritePin(GPIOA, bt1_state, 1);
				once = 1;
			}
			if (mas[bt1_state - 1] == 0) {
				counter_tim2++;
				if (counter_tim2 >= 17) {
					GPIO_TogglePin(GPIOA, bt1_state);
					counter_tim2 = 0;
				}
			}
			if (mas[bt1_state - 1] == 1) {
				counter_tim2++;
				if (counter_tim2 >= 5) {
					counter_tim2 = 0;
					GPIO_TogglePin(GPIOA, bt1_state);
				}
			}
			if (mas[bt1_state - 1] == 2) {
				counter_tim2++;
				if (counter_tim2 >= 3) {
					counter_tim2 = 0;
					GPIO_TogglePin(GPIOA, bt1_state);
				}
			}
		}
		if (bt1_state == 0) {
			for (uint8_t i = 1; i <= 7; i++) {
				GPIO_WritePin(GPIOA, i, 0);
			}
		}
	}
}

void TIM3_IRQHandler(void) {
	if (READ_BIT(TIM3->SR, TIM_SR_UIF)) {
		CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
		if (flag_button1 == 1) {
			period++;
		} else {
			period = 0;
		}
		if (period > DELAY) {
			flag_button1 = 0;
			bt1_state++;
			if (bt1_state > 6) {
				bt1_state = 0;
			}
		}

		if (flag_button2 == 1) {
			period_bt2++;
		}
		if (period_bt2 < 60 && period_bt2 > 10 && flag_button2 == 0) {
			period_bt2 = 0;
			bt2_change++;
			if (bt2_change > 2) {
				bt2_change = 0;
			}
			mas[bt2_state - 1] = bt2_change;
		}
		if (period_bt2 > 144 && flag_button2 == 1) {
			bt2_state++;
			bt2_change = 0;
			period_bt2 = 0;
			if (bt2_state > 6) {
				bt2_state = 1;
			}
			mas[bt2_state - 1] = bt2_change;

		} else {
			flag_button2 = 0;
		}
	}
}
