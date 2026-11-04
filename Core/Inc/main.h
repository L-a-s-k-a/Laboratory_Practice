#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "stm32f4xx.h"

// 1 = PA4 (первый светодиод, отдельный)
// 2 = PA6 (второй светодиод)
// 3 = PA7 (третий светодиод)
// 4 = PB0 (четвёртый светодиод)


//CMSIS
#define LED1_PIN   4    // PA4
#define LED2_PIN   6    // PA6
#define LED3_PIN   7    // PA7
#define LED4_PIN   0    // PB0

#define BTN1_PIN   4    // PA4
#define BTN2_PIN   0    // PA0


//Без CMSIS и макросов
#define RCC_AHB1ENR     (*(volatile uint32_t *)0x40023830u)
//MODER/PUPDR: 2 бита на пин, смещение pin*2 
//BSRR: бит pin = set, бит pin+16 = reset
#define GPIOA_MODER     (*(volatile uint32_t *)0x40020000u)
#define GPIOA_PUPDR     (*(volatile uint32_t *)0x4002000Cu)
#define GPIOA_IDR       (*(volatile uint32_t *)0x40020010u)
#define GPIOA_BSRR      (*(volatile uint32_t *)0x40020018u)

#define GPIOB_MODER     (*(volatile uint32_t *)0x40020400u)
#define GPIOB_PUPDR     (*(volatile uint32_t *)0x4002040Cu)
#define GPIOB_IDR       (*(volatile uint32_t *)0x40020410u)
#define GPIOB_BSRR      (*(volatile uint32_t *)0x40020418u)


//С макросами
//Базовые битовые операции
#define BIT(n)                  (1u << (n))
#define REG_SET(reg, mask)      ((reg) |=  (mask))
#define REG_CLR(reg, mask)      ((reg) &= ~(mask))

//тактирование
#define CLK_GPIOA_ON()          REG_SET(RCC->AHB1ENR, BIT(0))   //GPIOAEN
#define CLK_GPIOB_ON()          REG_SET(RCC->AHB1ENR, BIT(1))   //GPIOBEN

//режимы пинов (MODER: 2 бита на пин),  00 = input, 01 = output, 10 = AF, 11 = analog
#define PIN_MODE_IN(port, pin)   REG_CLR((port)->MODER, (3u << ((pin)*2)))
#define PIN_MODE_OUT(port, pin)  do { REG_CLR((port)->MODER, (3u << ((pin)*2))); \
                                      REG_SET((port)->MODER, (1u << ((pin)*2))); } while (0)

//подтяжка (PUPDR: 2 бита на пин),  00 = none, 01 = pull-up, 10 = pull-down
#define PIN_PULLUP(port, pin)    do { REG_CLR((port)->PUPDR, (3u << ((pin)*2))); \
                                      REG_SET((port)->PUPDR, (1u << ((pin)*2))); } while (0)

//установка/сброс выхода через BSRR
#define PIN_HIGH(port, pin)      ((port)->BSRR = (1u << (pin)))
#define PIN_LOW(port, pin)       ((port)->BSRR = (1u << ((pin) + 16)))

//чтение входа
#define PIN_READ(port, pin)      (((port)->IDR >> (pin)) & 1u)

//светодиоды
#define LED1_ON()   do { PIN_MODE_OUT(GPIOA, 4); PIN_HIGH(GPIOA, 4); } while (0)
#define LED1_OFF()  do { PIN_MODE_IN(GPIOA, 4); PIN_PULLUP(GPIOA, 4); } while (0)

#define LED2_ON()   PIN_HIGH(GPIOA, 6)
#define LED2_OFF()  PIN_LOW(GPIOA, 6)
#define LED3_ON()   PIN_HIGH(GPIOA, 7)
#define LED3_OFF()  PIN_LOW(GPIOA, 7)
#define LED4_ON()   PIN_HIGH(GPIOB, 0)
#define LED4_OFF()  PIN_LOW(GPIOB, 0)

//кнопки
#define BTN1_IS_INPUT()          (((GPIOA->MODER >> (4*2)) & 3u) == 0u)
#define BTN1_PRESSED()           (BTN1_IS_INPUT() && (PIN_READ(GPIOA, 4) == 0u))
#define BTN2_PRESSED()           (PIN_READ(GPIOA, 0) == 0u)



//Дополнительное задание
#define BTN1_PIN_dop 4   // PA4 - первая кнопка 
#define BTN2_PIN_dop 0   // PA0 - вторая кнопка 

#define LED1_PIN_dop 5   // PA5
#define LED2_PIN_dop 6   // PA6
#define LED3_PIN_dop 0   // PB0
#define LED4_PIN_dop 4   // PB4
#define LED5_PIN_dop 7   // PA7

//глобальные переменные
extern uint8_t led_count;     //количество включенных светодиодов
extern uint8_t brightness;    //градация яркости (1 - тускло, 2 - средне, 3 - ярко)

//прототипы функций




// Функции
//CMSIS
void GPIO_Init_CMSIS(void);
void set_led_CMSIS(uint8_t led_num);
void clear_led_CMSIS(uint8_t led_num);
void clear_all_leds_CMSIS(void);
void apply_leds_CMSIS(uint8_t led_a_on, uint8_t led_b_on, uint8_t led_c_on);
void update_leds_CMSIS(uint8_t count);
uint8_t read_btn1_CMSIS(void);
uint8_t read_btn2_CMSIS(void);

//Без CMSIS и макросов
void GPIO_Init_without_CMSIS_and_macros(void);
void set_led_without(uint8_t led_num);
void clear_led_without(uint8_t led_num);
void clear_all_leds_without(void);
uint8_t read_btn1_without(void);
uint8_t read_btn2_without(void);
void apply_leds_without(uint8_t led_a_on, uint8_t led_b_on, uint8_t led_c_on);

//С макросами
void GPIO_Init_macros(void);
void set_led_macros(uint8_t led_num);
void clear_led_macros(uint8_t led_num);
void clear_all_leds_macros(void);
uint8_t read_btn1_macros(void);
uint8_t read_btn2_macros(void);
void apply_leds_macros(uint8_t led_a_on, uint8_t led_b_on, uint8_t led_c_on);

// Общая функция
void delay_ms(uint32_t ms);

//Фунции разных main-ов
void do_with_CMSIS(void);
void do_without_CMSIS_and_macros(void);
void do_with_macros(void);

//Функции для дополнительного задания
void GPIO_Init_dop(void);
void light(uint8_t brightness, uint8_t led_count);
#endif