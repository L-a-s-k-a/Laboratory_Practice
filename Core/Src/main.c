#include "init.h"
#include "stdint.h"

int flag;
int count;
int flag_button;
int but_state;
#define DEBOUNCE_DELAY 50
int last_butt_state = 0;

int last_button_press_time = 0;
int last_button_press_time_0 = 0;

float timer_cnt;
int time_count;


int main(void){
	*(uint32_t *)(0x40020400UL + 0x00UL) |= 0x800; //output

    *(uint32_t *)(0x40020400UL + 0x04UL) |= 0x00; 
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x400; 
    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x00; 

    while(1){
        timer_cnt = time_count++/16000;

		if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13 ) !=0) {
			int current_time = timer_cnt;
			if ((current_time - last_button_press_time_0) > DEBOUNCE_DELAY) {
				if (last_butt_state == 0) {
					flag = !flag;
				}
				last_butt_state = 1;
			} else {
				last_butt_state = 0;
			}
			last_button_press_time_0 = current_time;
		}

		if (flag == 1) {
			switch (count) {
			case 0:
				SET_BIT(GPIOB->BSRR,
						GPIO_BSRR_BR3 | GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR13 | GPIO_BSRR_BR14 | GPIO_BSRR_BR15);
				break;
			case 1:
				SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS3);
				SET_BIT(GPIOB->BSRR,
						GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR13 | GPIO_BSRR_BR14 | GPIO_BSRR_BR15);
				break;
			case 2:
				SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS3 | GPIO_BSRR_BS4);
				SET_BIT(GPIOB->BSRR,
						GPIO_BSRR_BR5 | GPIO_BSRR_BR13 | GPIO_BSRR_BR14 | GPIO_BSRR_BR15);
				break;
			case 3:
				SET_BIT(GPIOB->BSRR,
						GPIO_BSRR_BS3 | GPIO_BSRR_BS4 | GPIO_BSRR_BS5);
				SET_BIT(GPIOB->BSRR,
						GPIO_BSRR_BR13 | GPIO_BSRR_BR14 | GPIO_BSRR_BR15);
				break;
			case 4:
				SET_BIT(GPIOB->BSRR,
						GPIO_BSRR_BS3 | GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS13);
				SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14 | GPIO_BSRR_BR15);
				break;
			case 5:
				SET_BIT(GPIOB->BSRR,
						GPIO_BSRR_BS3 | GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS13 | GPIO_BSRR_BS14);
				SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
				break;
			case 6:
				SET_BIT(GPIOB->BSRR,
						GPIO_BSRR_BS3 | GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS13 | GPIO_BSRR_BS14 | GPIO_BSRR_BS15);
				break;
			}
		}
		if (flag == 0) {
			SET_BIT(GPIOB->BSRR,
					GPIO_BSRR_BR3 | GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR13 | GPIO_BSRR_BR14 | GPIO_BSRR_BR15);
		}

		if (flag_button == 0) {
			if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_10) != 0) {
				int current_time = timer_cnt;
				if ((current_time - last_button_press_time) > DEBOUNCE_DELAY) {
					flag_button = 1;
					last_button_press_time = current_time;
				}
			}
		}

		if (flag_button == 1) {
			if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_10) == 0) {
				count++;
				if (count > 6) {
					count = 0;
				}
				flag_button = 0;
			}
		}
        // if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0){
        //     SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET);
        // }
        // else{
        //     SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET);
        // }
        // if (READ_GPIO_C13 != 0){
        //     SET_GPIO_B7;
        // }
        // else{
        //     RESET_GPIO_B7;
        // }
    }
}