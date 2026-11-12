#include <stdint.h>
#include <stm32f429xx.h>
#define MAX_TIME_LED 5.32e6
#define MAX_TIME_SHIM 100
#define DREBEZG_TIME 20000
#define SWITCH_TIME 125000

void led_switch(_Bool direction);
void led_brightness(uint32_t time1, uint32_t time2);
void shim(void);
void leds_on(void);
void leds_off(void);

void run_advance(void);

uint32_t test = 0;
_Bool work_mode = 0;
uint8_t brightness = 1;
uint8_t led_amount = 4;
uint32_t time_b1 = 0;
uint32_t time_b2 = 0;

uint32_t button1_counter = 0;
uint32_t button2_counter = 0;
uint32_t volatile drebezg_counter = 0;
uint32_t led_on_counter = 0;
uint32_t led_off_counter = 0;

int main(void){
    run_advance();
}

void run_advance(void) {
    // LEDS - PB4 PB5 PB6 PB8
    // BUTTONS - PB9 PB10
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    GPIOB->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER8); //Clean LED bits
    GPIOB->MODER |= (GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER8_0); //Set output for LED

    GPIOB->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10); //Clean button bits
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR9 | GPIO_PUPDR_PUPDR10); //Clean pupdr bits
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0); //Pull up for bittons

    while(1) {
        test++;
        shim();

        if((GPIOB->IDR & GPIO_IDR_ID9) == 0) {
            while(drebezg_counter <= DREBEZG_TIME) drebezg_counter++;
            drebezg_counter = 0;
            if((GPIOB->IDR & GPIO_IDR_ID9) == 0) {
                button1_counter = 0;
                while((GPIOB->IDR & GPIO_IDR_ID9) == 0) button1_counter++; //Wait holding
                if(button1_counter <= SWITCH_TIME) {
                    led_switch(0);
                } else {
                    time_b1 = button1_counter;
                }
            }
        }

        if((GPIOB->IDR & GPIO_IDR_ID10) == 0) {
            while(drebezg_counter <= DREBEZG_TIME) drebezg_counter++;
            drebezg_counter = 0;
            if((GPIOB->IDR & GPIO_IDR_ID10) == 0) {
                button2_counter = 0;
                while((GPIOB->IDR & GPIO_IDR_ID10) == 0) button2_counter++; //Wait holding
                if(button2_counter <= SWITCH_TIME) {
                    led_switch(1);
                    button2_counter = 0;
                } else {
                    time_b2 = button2_counter;
                    led_brightness(time_b1, time_b2);
                }
            }
        }
    }
}

void leds_on(void) {
    switch(led_amount)
    {
    case 1:
        GPIOB->BSRR = (GPIO_BSRR_BS4);
        break;
    case 2:
        GPIOB->BSRR = (GPIO_BSRR_BS4 | GPIO_BSRR_BS5);
        break;
    case 3:
        GPIOB->BSRR = (GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS6);
        break;
    case 4:
        GPIOB->BSRR = (GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS8);
        break;
    }
}

void leds_off(void) {
    switch(led_amount)
    {
    case 1:
        GPIOB->BSRR = (GPIO_BSRR_BR4);
        break;
    case 2:
        GPIOB->BSRR = (GPIO_BSRR_BR4 | GPIO_BSRR_BR5);
        break;
    case 3:
        GPIOB->BSRR = (GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR6);
        break;
    case 4:
        GPIOB->BSRR = (GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR8);
        break;
    }
}

void shim(void) {
    uint32_t duration_on = ((uint32_t)(brightness) * MAX_TIME_SHIM) / 100;

    if(led_on_counter <= duration_on) {
        led_on_counter++;
        leds_on();
    } else {
        if(led_off_counter <= MAX_TIME_SHIM - duration_on) {
            led_off_counter++;
            leds_off();
        } else {
            led_on_counter = 0;
            led_off_counter = 0;
        }
    }
}

void led_switch(_Bool direction) {
    if(direction) {
        if(led_amount < 4) {
            led_amount++;
        }
    } else {
        if(led_amount > 0) {
            led_amount--;
        }
    }
}

void led_brightness(uint32_t time1, uint32_t time2) {
    uint32_t time = 0;
    if(time1 > time2) {
        time = time1 - time2;
    } else {
        time = time2 - time1;
    }

    uint32_t temp_brightness = (uint32_t)(((double)(time) / MAX_TIME_LED) * 100.0);
    if(temp_brightness >= 100) {
        brightness = 100;
    } else if(temp_brightness >= 0) {
        brightness = temp_brightness;
    } else {
        brightness = 0;
    }
}
