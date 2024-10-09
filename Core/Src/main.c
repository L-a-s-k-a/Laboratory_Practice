#include "init.h"
#include "stdint.h"
#include <string.h>

int turn;
int count;
int flag_button_1;
int flag_button_2;
#define DEBOUNCE_DELAY 50
int last_button_press_time = 0;
int last_button_press_time_0 = 0;
int current_time;
int time_count;
#define LED_COUNT 6
uint8_t current_led_position = 0;
uint8_t all_leds_on = 1;

#define GPIO_LED_MASK_B (GPIO_BSRR_BR13 | GPIO_BSRR_BR14 | GPIO_BSRR_BR15) 
#define GPIO_LED_MASK_D (GPIO_BSRR_BR3 | GPIO_BSRR_BR4) 

void update_leds() {
	*(uint32_t*)(0x40020400UL + 0x18UL) &= ~(1 << 5);
	*(uint32_t*)(0x40020400UL + 0x18UL) |= (1 << 21);
    SET_BIT(GPIOB->BSRR, GPIO_LED_MASK_B);
    SET_BIT(GPIOD->BSRR, GPIO_LED_MASK_D);
    if (count == 0) {
        *(uint32_t*)(0x40020400UL + 0x18UL) &= ~(1 << 5);
		*(uint32_t*)(0x40020400UL + 0x18UL) |= (1 << 21);
		SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13 | GPIO_BSRR_BR14 | GPIO_BSRR_BR15);
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR3 | GPIO_BSRR_BR4);
    }
    else if (count == 6) {
        if (all_leds_on) {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= (1 << 5);
			SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS13 | GPIO_BSRR_BS14 | GPIO_BSRR_BS15);
            SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS3 | GPIO_BSRR_BS4 );
        } else {
            SET_BIT(GPIOB->BSRR, GPIO_LED_MASK_B);
            SET_BIT(GPIOD->BSRR, GPIO_LED_MASK_D);
			*(uint32_t*)(0x40020400UL + 0x18UL) &= ~(1 << 5);
			*(uint32_t*)(0x40020400UL + 0x18UL) |= (1 << 21);
        }
    }
    else {
        for (uint8_t i = 0; i < count; i++) {
            uint8_t led_index = (current_led_position + i) % LED_COUNT;
            switch (led_index) {
                case 0: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS3); break;
                case 1: *(uint32_t*)(0x40020400UL + 0x18UL) |= (1 << 5); break;
                case 2: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS5); break;
                case 3: SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS13); break;
                case 4: SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14); break;
                case 5: SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15); break;
            }
        }
    }
}
int main(void){
	GPIO_Ini();
	*(uint32_t *)(0x40020400UL + 0x00UL) |= 0x800; 
    *(uint32_t *)(0x40020400UL + 0x04UL) |= 0x00; 
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x400; 
    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x00;

    while(1){
		current_time = time_count++ / 16000;
		if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_0) == 1) {
			flag_button_1 = !flag_button_1;
		}
		if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_10) != 0) {
			flag_button_2 = !flag_button_2;
		}
		if (flag_button_1) {
			if ((current_time - last_button_press_time) > DEBOUNCE_DELAY / 5) {
				turn = !turn;

				if (count == 6) {
					all_leds_on = !all_leds_on;
					update_leds();
				} else if (turn == 1 || turn == 0) {
					current_led_position = (current_led_position + 1)
							% LED_COUNT;
					update_leds();
				}

				last_button_press_time = current_time;
			}
			flag_button_1 = 0;
		}

		if (flag_button_2) {
			if ((current_time - last_button_press_time_0) > DEBOUNCE_DELAY) {
				count++;
				if (count > 6) {
					count = 0;
				}
				update_leds();
				last_button_press_time_0 = current_time;
			}
			flag_button_2 = 0;
    }
}