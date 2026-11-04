#include "main.h"

//Инициализация с CMSIS
void GPIO_Init_CMSIS(void) {
    //Тактирование портов A и B
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);

    // PA6, PA7, PB0 — выходы 
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODE6_0);   // PA6
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODE7_0);   // PA7
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);   // PB0

    // PA4 — вход с pull-up (по умолчанию)
    CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE4);
    SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD4_0);   // pull-up
    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD4_1);

    // PA0 — вход с pull-up (вторая кнопка)
    CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE0);
    SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD0_0);   // pull-up
    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD0_1);

    clear_all_leds_CMSIS();
}

void set_led_CMSIS(uint8_t led_num) {
    if (led_num == 1){
        SET_BIT(GPIOA->MODER, GPIO_MODER_MODE4_0);
        CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE4_1);
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS4);   // PA4 = 1
    }
    if (led_num == 2){
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS6);
    }
    if (led_num == 3){
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS7);
    }
    if (led_num == 4){
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
    }
}

void clear_led_CMSIS(uint8_t led_num) {
    if (led_num == 1){
        CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE4);   // PA4 - вход
        SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD4_0);   // pull-up
        CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD4_1);
    }
    if (led_num == 2){
        GPIOA->BSRR = GPIO_BSRR_BR6;  //сброс
    }
    if (led_num == 3){
        GPIOA->BSRR = GPIO_BSRR_BR7;  //сброс
    }
    if (led_num == 4){
        GPIOB->BSRR = GPIO_BSRR_BR0;  //сброс
    }
}

void clear_all_leds_CMSIS(void) {
    clear_led_CMSIS(1);
    clear_led_CMSIS(2);
    clear_led_CMSIS(3);
    clear_led_CMSIS(4);
}
void apply_leds_CMSIS(uint8_t led_a_on, uint8_t led_b_on, uint8_t led_c_on) {
    if (led_a_on) set_led_CMSIS(2); else clear_led_CMSIS(2);
    if (led_b_on) set_led_CMSIS(3); else clear_led_CMSIS(3);
    if (led_c_on) set_led_CMSIS(4); else clear_led_CMSIS(4);
}

void update_leds_CMSIS(uint8_t count) {
    clear_led_CMSIS(2);
    clear_led_CMSIS(3);
    clear_led_CMSIS(4);

    if (count >= 1) set_led_CMSIS(2);  // PA6
    if (count >= 2) set_led_CMSIS(3);  // PA7
    if (count >= 3) set_led_CMSIS(4);  // PB0
}

uint8_t read_btn1_CMSIS(void) {
    if ((GPIOA->MODER & GPIO_MODER_MODE4) == 0) {
        if ((GPIOA->IDR & GPIO_IDR_ID4) == 0) {
            return 1;  // нажата
        }
    }
    return 0;
}

uint8_t read_btn2_CMSIS(void) {
    if ((GPIOA->IDR & GPIO_IDR_ID0) == 0) {
        return 1;  // нажата
    }
    return 0;
}

void delay_ms(uint32_t ms) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < ms; i++) {
        while(count < 18000){
            count++;
        }
    }
}

void do_with_CMSIS(void){
    uint8_t led_a_on = 0;
    uint8_t led_b_on = 0;
    uint8_t led_c_on = 0;

    uint8_t saved_a = 0;    //состояние PA6
    uint8_t saved_b = 0;    //состояние PA7
    uint8_t saved_c = 0;    //состояние PB0

    uint8_t btn2_pressed = 0;   // 0 - поочередное включение, 1 - PA4 горит
    uint8_t btn1_released = 1;  //первая кнопка отпущена
    apply_leds_CMSIS(led_a_on, led_b_on, led_c_on);

    while (1) {
        //вторая кнопка: первое нажатие
        uint8_t btn2_now = read_btn2_CMSIS();
        if (btn2_now == 1 && btn2_pressed == 0) {
            delay_ms(20);

            if (read_btn2_CMSIS() == 1) {
                btn2_pressed = 1;
                //запись состояния трёх светодиодов
                saved_a = led_a_on;
                saved_b = led_b_on;
                saved_c = led_c_on;
                //выключаем
                led_a_on = 0;
                led_b_on = 0;
                led_c_on = 0;
                apply_leds_CMSIS(led_a_on, led_b_on, led_c_on);


                //включаем первый светодиод (PA4 = выход на 3.3 В)
                set_led_CMSIS(1);

                //ожидание отпускания
                while (read_btn2_CMSIS() == 1) {
                    delay_ms(1);
                }
            }
        }
        //второе нажатие
        if (btn2_now == 1 && btn2_pressed == 1) {
            delay_ms(20);
            if (read_btn2_CMSIS() == 1) {
                btn2_pressed = 0;
                //выключаем первый светодиод (PA4 - вход, pull-up)
                clear_led_CMSIS(1);

                //возврат состояний
                led_a_on = saved_a;
                led_b_on = saved_b;
                led_c_on = saved_c;
                apply_leds_CMSIS(led_a_on, led_b_on, led_c_on);


                //ожидание отпускания
                while (read_btn2_CMSIS() == 1) {
                    delay_ms(1);
                }
            }
        }
        //первая кнопка
        if (btn2_pressed == 0) {
            uint8_t btn1_now = read_btn1_CMSIS();
            //ожидание отпускания
            if (btn1_now == 0) {
                if (btn1_released == 0) {
                    delay_ms(20);
                    if (read_btn1_CMSIS() == 0) {
                        btn1_released = 1;
                    }
                }
            }
            //считывание нажатия
            if (btn1_now == 1 && btn1_released == 1) {
                delay_ms(20);

                if (read_btn1_CMSIS() == 1) {
                    btn1_released = 0;

                    //поочерное включение
                    if (!led_a_on && !led_b_on && !led_c_on) {
                        led_a_on = 1;
                    } else if (led_a_on && !led_b_on && !led_c_on) {
                        led_b_on = 1;
                    } else if (led_a_on && led_b_on && !led_c_on) {
                        led_c_on = 1;
                    } else if (led_a_on && led_b_on && led_c_on) {
                        led_a_on = 0;
                        led_b_on = 0;
                        led_c_on = 0;
                    }
                    apply_leds_CMSIS(led_a_on, led_b_on, led_c_on);

                }
            }
        }
    }
}




//Без CMSIS и макросов

void GPIO_Init_without_CMSIS_and_macros(void) {
    //тактирование GPIOA (бит 0) и GPIOB (бит 1)
    RCC_AHB1ENR |= (1u << 0);
    RCC_AHB1ENR |= (1u << 1);

    //PA6, PA7 - выход (01 в MODER[13:12] и [15:14])
    GPIOA_MODER &= ~((3u << 12) | (3u << 14));
    GPIOA_MODER |=  ((1u << 12) | (1u << 14));

    //PB0 - выход
    GPIOB_MODER &= ~(3u << 0);
    GPIOB_MODER |=  (1u << 0);

    //PA4 - вход (00) + pull-up (01 в PUPDR[9:8])
    GPIOA_MODER &= ~(3u << 8);
    GPIOA_PUPDR &= ~(3u << 8);
    GPIOA_PUPDR |=  (1u << 8);

    //PA0 - вход + pull-up
    GPIOA_MODER &= ~(3u << 0);
    GPIOA_PUPDR &= ~(3u << 0);
    GPIOA_PUPDR |=  (1u << 0);

    clear_all_leds_without();
}

void set_led_without(uint8_t led_num) {
    if (led_num == 1) {
        GPIOA_MODER &= ~(3u << 8);
        GPIOA_MODER |=  (1u << 8);
        GPIOA_BSRR = (1u << 4);
    }
    if (led_num == 2) GPIOA_BSRR = (1u << 6);
    if (led_num == 3) GPIOA_BSRR = (1u << 7);
    if (led_num == 4) GPIOB_BSRR = (1u << 0);
}

void clear_led_without(uint8_t led_num) {
    if (led_num == 1) {
        GPIOA_MODER &= ~(3u << 8);
        GPIOA_PUPDR &= ~(3u << 8);
        GPIOA_PUPDR |=  (1u << 8);
    }
    if (led_num == 2) GPIOA_BSRR = (1u << (6 + 16));
    if (led_num == 3) GPIOA_BSRR = (1u << (7 + 16));
    if (led_num == 4) GPIOB_BSRR = (1u << (0 + 16));
}

void clear_all_leds_without(void) {
    clear_led_without(1);
    clear_led_without(2);
    clear_led_without(3);
    clear_led_without(4);
}

uint8_t read_btn1_without(void) {
    if ((GPIOA_MODER & (3u << 8)) == 0u) {
        if ((GPIOA_IDR & (1u << 4)) == 0u) {
            return 1u;
        }
    }
    return 0u;
}

uint8_t read_btn2_without(void) {
    if ((GPIOA_IDR & (1u << 0)) == 0u) {
        return 1u;
    }
    return 0u;
}


void apply_leds_without(uint8_t led_a_on, uint8_t led_b_on, uint8_t led_c_on) {
    if (led_a_on) set_led_without(2); else clear_led_without(2);
    if (led_b_on) set_led_without(3); else clear_led_without(3);
    if (led_c_on) set_led_without(4); else clear_led_without(4);
}

void do_without_CMSIS_and_macros(void){

    uint8_t led_a_on = 0;   //PA6 
    uint8_t led_b_on = 0;   //PA7
    uint8_t led_c_on = 0;   //PB0
    apply_leds_without(led_a_on, led_b_on, led_c_on);

    uint8_t saved_a = 0;    //состояние PA6 
    uint8_t saved_b = 0;    //состояние PA7
    uint8_t saved_c = 0;    //состояние PB0

    uint8_t btn2_pressed = 0;   // 0 - поочередное включение, 1 - PA4 горит
    uint8_t btn1_released = 1;  //первая кнопка отпущена
    
    while (1) {
        //вторая кнопка: первое нажатие
        uint8_t btn2_now = read_btn2_without();
        if (btn2_now == 1 && btn2_pressed == 0) {
            delay_ms(20);

            if (read_btn2_without() == 1) {
                btn2_pressed = 1;
                //запись состояния трёх светодиодов
                saved_a = led_a_on;
                saved_b = led_b_on;
                saved_c = led_c_on;
                //выключаем
                led_a_on = 0;
                led_b_on = 0;
                led_c_on = 0;
                apply_leds_without(led_a_on, led_b_on, led_c_on);

                //включаем первый светодиод (PA4 = выход на 3.3 В)
                set_led_without(1);

                //ожидание отпускания
                while (read_btn2_without() == 1) {
                    delay_ms(1);
                }
            }
        }
        //второе нажатие
        if (btn2_now == 1 && btn2_pressed == 1) {
            delay_ms(20);
            if (read_btn2_without() == 1) {
                btn2_pressed = 0;
                //выключаем первый светодиод (PA4 - вход, pull-up)
                clear_led_without(1);

                //возврат состояний
                led_a_on = saved_a;
                led_b_on = saved_b;
                led_c_on = saved_c;
                apply_leds_without(led_a_on, led_b_on, led_c_on);

                //ожидание отпускания
                while (read_btn2_without() == 1) {
                    delay_ms(1);
                }
            }
        }
        //первая кнопка
        if (btn2_pressed == 0) {
            uint8_t btn1_now = read_btn1_without();
            //ожидание отпускания
            if (btn1_now == 0) {
                if (btn1_released == 0) {
                    delay_ms(20);
                    if (read_btn1_without() == 0) {
                        btn1_released = 1;
                    }
                }
            }
            //считывание нажатия
            if (btn1_now == 1 && btn1_released == 1) {
                delay_ms(20);

                if (read_btn1_without() == 1) {
                    btn1_released = 0;

                    //поочерное включение
                    if (!led_a_on && !led_b_on && !led_c_on) {
                        led_a_on = 1;
                    } else if (led_a_on && !led_b_on && !led_c_on) {
                        led_b_on = 1;
                    } else if (led_a_on && led_b_on && !led_c_on) {
                        led_c_on = 1;
                    } else if (led_a_on && led_b_on && led_c_on) {
                        led_a_on = 0;
                        led_b_on = 0;
                        led_c_on = 0;
                    }
                    apply_leds_without(led_a_on, led_b_on, led_c_on);
                }
            }
        }
    }
}


//С макросами
void GPIO_Init_macros(void) {
    CLK_GPIOA_ON();
    CLK_GPIOB_ON();

    //PA6, PA7, PB0 — выходы
    PIN_MODE_OUT(GPIOA, 6);
    PIN_MODE_OUT(GPIOA, 7);
    PIN_MODE_OUT(GPIOB, 0);

    //PA4 — вход с pull-up
    PIN_MODE_IN(GPIOA, 4);
    PIN_PULLUP(GPIOA, 4);

    //PA0 — вход с pull-up
    PIN_MODE_IN(GPIOA, 0);
    PIN_PULLUP(GPIOA, 0);

    clear_all_leds_macros();
}

void set_led_macros(uint8_t led_num) {
    if (led_num == 1) LED1_ON();
    if (led_num == 2) LED2_ON();
    if (led_num == 3) LED3_ON();
    if (led_num == 4) LED4_ON();
}

void clear_led_macros(uint8_t led_num) {
    if (led_num == 1) LED1_OFF();
    if (led_num == 2) LED2_OFF();
    if (led_num == 3) LED3_OFF();
    if (led_num == 4) LED4_OFF();
}

void clear_all_leds_macros(void) {
    clear_led_macros(1);
    clear_led_macros(2);
    clear_led_macros(3);
    clear_led_macros(4);
}

void apply_leds_macros(uint8_t led_a_on, uint8_t led_b_on, uint8_t led_c_on) {
    if (led_a_on) set_led_macros(2); else clear_led_macros(2);
    if (led_b_on) set_led_macros(3); else clear_led_macros(3);
    if (led_c_on) set_led_macros(4); else clear_led_macros(4);
}

uint8_t read_btn1_macros(void) {
    return BTN1_PRESSED() ? 1u : 0u;
}

uint8_t read_btn2_macros(void) {
    return BTN2_PRESSED() ? 1u : 0u;
}

void do_with_macros(){
    uint8_t led_a_on = 0;   //PA6
    uint8_t led_b_on = 0;   //PA7
    uint8_t led_c_on = 0;   //PB0

    uint8_t saved_a = 0;    //состояние PA6
    uint8_t saved_b = 0;    //состояние PA7
    uint8_t saved_c = 0;    //состояние PB0

    uint8_t btn2_pressed = 0;   // 0 - поочередное включение, 1 - PA4 горит
    uint8_t btn1_released = 1;  //первая кнопка отпущена

    GPIO_Init_macros();
    apply_leds_macros(led_a_on, led_b_on, led_c_on);

    while (1) {
        //вторая кнопка: первое нажатие
        uint8_t btn2_now = read_btn2_macros();
        if (btn2_now == 1 && btn2_pressed == 0) {
            delay_ms(20);

            if (read_btn2_macros() == 1) {
                btn2_pressed = 1;
                //запись состояния трёх светодиодов
                saved_a = led_a_on;
                saved_b = led_b_on;
                saved_c = led_c_on;
                //выключаем
                led_a_on = 0;
                led_b_on = 0;
                led_c_on = 0;
                apply_leds_macros(led_a_on, led_b_on, led_c_on);

                //включаем первый светодиод (PA4 = выход на 3.3 В)
                set_led_macros(1);

                //ожидание отпускания
                while (read_btn2_macros() == 1) {
                    delay_ms(1);
                }
            }
        }
        //второе нажатие
        if (btn2_now == 1 && btn2_pressed == 1) {
            delay_ms(20);
            if (read_btn2_macros() == 1) {
                btn2_pressed = 0;
                //выключаем первый светодиод (PA4 - вход, pull-up)
                clear_led_macros(1);

                //возврат состояний
                led_a_on = saved_a;
                led_b_on = saved_b;
                led_c_on = saved_c;
                apply_leds_macros(led_a_on, led_b_on, led_c_on);

                //ожидание отпускания
                while (read_btn2_macros() == 1) {
                    delay_ms(1);
                }
            }
        }
        //первая кнопка
        if (btn2_pressed == 0) {
            uint8_t btn1_now = read_btn1_macros();
            //ожидание отпускания
            if (btn1_now == 0) {
                if (btn1_released == 0) {
                    delay_ms(20);
                    if (read_btn1_macros() == 0) {
                        btn1_released = 1;
                    }
                }
            }
            //считывание нажатия
            if (btn1_now == 1 && btn1_released == 1) {
                delay_ms(20);

                if (read_btn1_macros() == 1) {
                    btn1_released = 0;

                    //поочерное включение
                    if (!led_a_on && !led_b_on && !led_c_on) {
                        led_a_on = 1;
                    } else if (led_a_on && !led_b_on && !led_c_on) {
                        led_b_on = 1;
                    } else if (led_a_on && led_b_on && !led_c_on) {
                        led_c_on = 1;
                    } else if (led_a_on && led_b_on && led_c_on) {
                        led_a_on = 0;
                        led_b_on = 0;
                        led_c_on = 0;
                    }
                    apply_leds_macros(led_a_on, led_b_on, led_c_on);
                }
            }
        }
    }
}

//Дополнительное задание
void GPIO_Init_dop(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;   //тактирование портов A и B

    // Пины светодиодов  на выход (Output mode - 01)
    // PA5, PA6, PA7
    GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOA->MODER |= (GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
    
    // PB0, PB4
    GPIOB->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER4);
    GPIOB->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER4_0);

    // Пины кнопок на вход (Input mode - 00)
    // PA4 (BTN1) и PA0 (BTN2)
    GPIOA->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER0);

    // Подтяжка к питанию (Pull-up - 01) для кнопок
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4 | GPIO_PUPDR_PUPDR0);
    GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR4_0 | GPIO_PUPDR_PUPDR0_0);
}

void light(uint8_t brightness, uint8_t led_count){
    uint8_t on_time;
    if (brightness == 1){
        on_time = 5;
    }
    if (brightness == 2){
        on_time = 70;
    }
    if (brightness == 3){
        on_time = 200;
    }
    int16_t tmp = 255;
    
    if (led_count == 1){
        while (tmp >= on_time){
            GPIOA->BSRR &= ~GPIO_BSRR_BS5;
            GPIOA->BSRR |= GPIO_BSRR_BR5;
            tmp--;
            }
        }
        while ((tmp < on_time)&(tmp > 0)){
            GPIOA->BSRR |= GPIO_BSRR_BS5;
            tmp--;
        }
 
    if (led_count == 2){
        while (tmp >= on_time){
            GPIOA->BSRR &= ~GPIO_BSRR_BS5;
            GPIOA->BSRR |= GPIO_BSRR_BR5;
            GPIOA->BSRR &= ~GPIO_BSRR_BS6;
            GPIOA->BSRR |= GPIO_BSRR_BR6;
            tmp--;
        }
        while ((tmp < on_time)&(tmp > 0)){
            GPIOA->BSRR |= GPIO_BSRR_BS5;
            GPIOA->BSRR |= GPIO_BSRR_BS6;
            tmp--;
        }
    }
    if (led_count == 3){
        while (tmp >= on_time){
            GPIOA->BSRR &= ~GPIO_BSRR_BS5;
            GPIOA->BSRR |= GPIO_BSRR_BR5;
            GPIOA->BSRR &= ~GPIO_BSRR_BS6;
            GPIOA->BSRR |= GPIO_BSRR_BR6;
            GPIOB->BSRR &= ~GPIO_BSRR_BS0;
            GPIOB->BSRR |= GPIO_BSRR_BR0;
            tmp--;
        }
        while ((tmp < on_time)&(tmp > 0)){
            GPIOA->BSRR |= GPIO_BSRR_BS5;
            GPIOA->BSRR |= GPIO_BSRR_BS6;
            GPIOB->BSRR |= GPIO_BSRR_BS0;
            tmp--;
        }
    }
    if (led_count == 4){
        while (tmp >= on_time){
            GPIOA->BSRR &= ~GPIO_BSRR_BS5;
            GPIOA->BSRR |= GPIO_BSRR_BR5;
            GPIOA->BSRR &= ~GPIO_BSRR_BS6;
            GPIOA->BSRR |= GPIO_BSRR_BR6;
            GPIOB->BSRR &= ~GPIO_BSRR_BS0;
            GPIOB->BSRR |= GPIO_BSRR_BR0;
            GPIOB->BSRR &= ~GPIO_BSRR_BS4;
            GPIOB->BSRR |= GPIO_BSRR_BR4;
            tmp--;
        }
        while ((tmp < on_time)&(tmp > 0)){
            GPIOA->BSRR |= GPIO_BSRR_BS5;
            GPIOA->BSRR |= GPIO_BSRR_BS6;
            GPIOB->BSRR |= GPIO_BSRR_BS0;
            GPIOB->BSRR |= GPIO_BSRR_BS4;
            tmp--;
        }

    }
    if (led_count == 5){
        while (tmp >= on_time){
            GPIOA->BSRR &= ~GPIO_BSRR_BS5;
            GPIOA->BSRR |= GPIO_BSRR_BR5;
            GPIOA->BSRR &= ~GPIO_BSRR_BS6;
            GPIOA->BSRR |= GPIO_BSRR_BR6;
            GPIOB->BSRR &= ~GPIO_BSRR_BS0;
            GPIOB->BSRR |= GPIO_BSRR_BR0;
            GPIOB->BSRR &= ~GPIO_BSRR_BS4;
            GPIOB->BSRR |= GPIO_BSRR_BR4;
            GPIOA->BSRR &= ~GPIO_BSRR_BS7;
            GPIOA->BSRR |= GPIO_BSRR_BR7;
            tmp--;
        }
        while ((tmp < on_time)&(tmp > 0)){
            GPIOA->BSRR |= GPIO_BSRR_BS5;
            GPIOA->BSRR |= GPIO_BSRR_BS6;
            GPIOB->BSRR |= GPIO_BSRR_BS0;
            GPIOB->BSRR |= GPIO_BSRR_BS4;
            GPIOA->BSRR |= GPIO_BSRR_BS7;
            tmp--;
        }
    }
    if (led_count == 0){
        GPIOA->BSRR &= ~GPIO_BSRR_BS5;
        GPIOA->BSRR |= GPIO_BSRR_BR5;
        GPIOA->BSRR &= ~GPIO_BSRR_BS6;
        GPIOA->BSRR |= GPIO_BSRR_BR6;
        GPIOB->BSRR &= ~GPIO_BSRR_BS0;
        GPIOB->BSRR |= GPIO_BSRR_BR0;
        GPIOB->BSRR &= ~GPIO_BSRR_BS4;
        GPIOB->BSRR |= GPIO_BSRR_BR4;
        GPIOA->BSRR &= ~GPIO_BSRR_BS7;
        GPIOA->BSRR |= GPIO_BSRR_BR7;
    }
}

void green_lights(){
    uint8_t led_count = 0;
    uint8_t brightness = 1;
    GPIO_Init_dop();
    while (1) {
        light(brightness, led_count);
        //первая кнопка
        if ((GPIOA->IDR & GPIO_IDR_IDR_4) == 0) {
            int32_t c = 300;
            while(c){
                c--;
                light(brightness, led_count);
            }
            
            led_count++;
            if (led_count > 5){
                led_count = 0; 
            }
            
            while ((GPIOA->IDR & GPIO_IDR_IDR_4) == 0) {
                light(brightness, led_count);
            }
        }
        
        //вторая кнопка
        if ((GPIOA->IDR & GPIO_IDR_IDR_0) == 0){
            int32_t c = 300;
            int32_t counter = 0;
            while(c){
                c--;
                light(brightness, led_count);
            }

            while ((GPIOA->IDR & GPIO_IDR_IDR_0) == 0){
                counter++;
                light(brightness, led_count);
            }
            
            if (counter < 300){
                if (brightness < 3){
                    brightness++;
                }
            }
            else{
                if (brightness > 1){
                    brightness--;
                }
            }
        }
    }
}
