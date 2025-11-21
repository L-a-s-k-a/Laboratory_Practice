#include <stdint.h>
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F411xE/stm32f411xe.h"

// Прямое обращение к памяти
// RCC  
#define RCC_AHB1ENR         (*(uint32_t *)((0x40023800UL) + 0x30UL))        //включение тактирования для всех портов GPIO

#define GPIOA_MODER         (*(uint32_t *)((0x40020000UL) + 0x00UL))        // подключение Moder, отвечающий за режимы работы input, ouput
#define GPIOA_OSPEEDR       (*(uint32_t *)((0x40020000UL) + 0x08UL))        // скорость переключения выводов
#define GPIOA_PUPDR         (*(uint32_t *)((0x40020000UL) + 0x0CUL))        //подтяжки PU/PD
#define GPIOA_IDR           (*(uint32_t *)((0x40020000UL) + 0x10UL))        //чтение состояния выходов
#define GPIOA_BSRR          (*(uint32_t *)((0x40020000UL) + 0x18UL))        //установка/сброс выводов

#define GPIOB_MODER         (*(uint32_t *)((0x40020400UL) + 0x00UL))
#define GPIOB_OSPEEDR       (*(uint32_t *)((0x40020400UL) + 0x08UL))
#define GPIOB_PUPDR         (*(uint32_t *)((0x40020400UL) + 0x0CUL))
#define GPIOB_IDR           (*(uint32_t *)((0x40020400UL) + 0x10UL))
#define GPIOB_BSRR          (*(uint32_t *)((0x40020400UL) + 0x18UL))

#define GPIOC_MODER         (*(uint32_t *)((0x40020800UL) + 0x00UL))
#define GPIOC_OSPEEDR       (*(uint32_t *)((0x40020800UL) + 0x08UL))
#define GPIOC_PUPDR         (*(uint32_t *)((0x40020800UL) + 0x0CUL))
#define GPIOC_IDR           (*(uint32_t *)((0x40020800UL) + 0x10UL))
#define GPIOC_BSRR          (*(uint32_t *)((0x40020800UL) + 0x18UL))

// Маски пинов
#define GPIO_PIN_1      0x0002      //бит 1
#define GPIO_PIN_2      0x0004      //бит 2
#define GPIO_PIN_4      0x0010      //бит 4
#define GPIO_PIN_5      0x0020      //бит 5
#define GPIO_PIN_6      0x0040      //бит 6
#define GPIO_PIN_11     0x0800      //бит 11
#define GPIO_PIN_12     0x1000      // бит 12
#define GPIO_PIN_13     0x2000      //бит 13

// MODER значения выход 
#define MODER_PA5_OUTPUT    0x00000400      // PA5 
#define MODER_PA6_OUTPUT    0x00001000      // PA6 
#define MODER_PA11_OUTPUT   0x00400000      // PA11
#define MODER_PC5_OUTPUT    0x00000400      // PC5
#define MODER_PC6_OUTPUT    0x00001000      // PC6
#define MODER_PB1_OUTPUT    0x00000004      // PB1   
#define MODER_PB2_OUTPUT    0x00000010      // PB2

// PUPDR значения (PULLUP = 01) подтяжка к VCC
#define PUPDR_PA11_PULLUP   0x00400000      // подтяжка на биты (01)
#define PUPDR_PC4_PULLUP    0x00000100      //подтяжка на биты(01)
#define PUPDR_PC13_PULLUP   0x04000000      //подтяжка на биты(01)
#define PUPDR_PB12_PULLUP   0x01000000      //подтяжка на биты(01)

// BSRR значения
#define BSRR_SET_1          0x00000002      // установить бит 1
#define BSRR_SET_2          0x00000004      //установить бит 2
#define BSRR_SET_5          0x00000020      // установить бит 5
#define BSRR_SET_6          0x00000040      //установить бит 6
#define BSRR_RESET_1        0x00020000      //сбросить бит 1
#define BSRR_RESET_2        0x00040000      //сбросить бит 2
#define BSRR_RESET_5        0x00200000      // сбросить бит 5
#define BSRR_RESET_6        0x00400000      // сбросить бит 6

// CMSIS (для зеленых светодиодов и кнопки 3)Ы
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))      // установка бита 1 (или логическое)
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))     // установка бита 0 (логическое и)

// переменные
extern uint8_t led_state[3];                                    //состояние светодиодов
extern uint8_t button_to_color[3];                              // кнопка на цвет
extern uint8_t btn1_prev, btn2_prev, btn3_prev, mode_prev;      // предыдущие состояния кнопки
extern uint8_t mode_count;                                      //кнопкообновлятор

// Счетчики нажатий кнопок
extern uint32_t btn1_press_count;    // количество нажатий кнопка 1
extern uint32_t btn2_press_count;    // количество нажатий кнопка 2
extern uint32_t btn3_press_count;    // количество нажатий кнопка 3
extern uint32_t mode_press_count;    // количество нажатий кнопка переключения

extern uint8_t led_green;            // состояние зелёного светодиода
extern uint8_t led_blue;             // состояние синего светодиода
extern uint8_t led_red;              // состояние красного светодиода

extern uint8_t debug_flag;           // флаг для отслеживания нажатий

// функции
void GPIO_Init_All(void);
void disableAllLeds(void);
void update_leds(void);
uint8_t readButton(uint32_t *idr, uint32_t mask, uint8_t* prevState);

/*
#include <stdint.h>

// Прямое обращение к памяти
// RCC
#define RCC_AHB1ENR         (*(uint32_t *)((0x40023800UL) + 0x30UL))        //включение тактирования для всех портов GPIO

#define GPIOA_MODER         (*(uint32_t *)((0x40020000UL) + 0x00UL))        // подключение Moder, отвечающий за режимы работы input, ouput
#define GPIOA_OSPEEDR       (*(uint32_t *)((0x40020000UL) + 0x08UL))        // скорость переключения выводов
#define GPIOA_PUPDR         (*(uint32_t *)((0x40020000UL) + 0x0CUL))        //подтяжки PU/PD
#define GPIOA_IDR           (*(uint32_t *)((0x40020000UL) + 0x10UL))        //чтение состояния выходов
#define GPIOA_BSRR          (*(uint32_t *)((0x40020000UL) + 0x18UL))        //установка/сброс выводов

#define GPIOB_MODER         (*(uint32_t *)((0x40020400UL) + 0x00UL))
#define GPIOB_OSPEEDR       (*(uint32_t *)((0x40020400UL) + 0x08UL))
#define GPIOB_PUPDR         (*(uint32_t *)((0x40020400UL) + 0x0CUL))
#define GPIOB_IDR           (*(uint32_t *)((0x40020400UL) + 0x10UL))
#define GPIOB_BSRR          (*(uint32_t *)((0x40020400UL) + 0x18UL))

#define GPIOC_MODER         (*(uint32_t *)((0x40020800UL) + 0x00UL))
#define GPIOC_OSPEEDR       (*(uint32_t *)((0x40020800UL) + 0x08UL))
#define GPIOC_PUPDR         (*(uint32_t *)((0x40020800UL) + 0x0CUL))
#define GPIOC_IDR           (*(uint32_t *)((0x40020800UL) + 0x10UL))
#define GPIOC_BSRR          (*(uint32_t *)((0x40020800UL) + 0x18UL))

// Маски пинов
#define GPIO_PIN_1      0x0002      //бит 1
#define GPIO_PIN_2      0x0004      //бит 2
#define GPIO_PIN_4      0x0010      //бит 4
#define GPIO_PIN_5      0x0020      //бит 5
#define GPIO_PIN_6      0x0040      //бит 6
#define GPIO_PIN_11     0x0800      //бит 11
#define GPIO_PIN_12     0x1000      //бит 12
#define GPIO_PIN_13     0x2000      //бит 13

// Маски для включения тактирования
#define RCC_GPIOA_EN    0x01        //бит 0 -включени тактирования
#define RCC_GPIOB_EN    0x02        //бит 1 -включени тактирования
#define RCC_GPIOC_EN    0x04        //бит 2 -включени тактирования

// MODER значения выход (01)
#define MODER_PA5_OUTPUT    0x00000400
#define MODER_PA6_OUTPUT    0x00001000
#define MODER_PA11_OUTPUT   0x00400000
#define MODER_PC5_OUTPUT    0x00000400
#define MODER_PC6_OUTPUT    0x00001000
#define MODER_PB1_OUTPUT    0x00000004
#define MODER_PB2_OUTPUT    0x00000010
#define MODER_PC13_INPUT    0x00000000
#define MODER_PB12_INPUT    0x00000000
#define MODER_PC4_INPUT     0x00000000

// PUPDR значения PU (01)
#define PUPDR_PA11_PULLUP   0x00400000
#define PUPDR_PC4_PULLUP    0x00000100
#define PUPDR_PC13_PULLUP   0x04000000
#define PUPDR_PB12_PULLUP   0x01000000

// BSRR макросы для установки/сброса
// Зелёные светодиоды (PA5 и PC5)
#define BSRR_SET_PA5        0x00000020     // Установить 

#define BSRR_SET_PC5        0x00000020     // Установить 
#define BSRR_SET_PA6        0x00000040     // Установить 
#define BSRR_SET_PB1        0x00000002     // Установить 
#define BSRR_SET_PC6        0x00000040     // Установить 
#define BSRR_SET_PB2        0x00000004     // Установить 
#define BSRR_RESET_PA5      0x00200000     // Сбросить 
#define BSRR_RESET_PC5      0x00200000     // Сбросить
#define BSRR_RESET_PA6      0x00400000     // Сбросить
#define BSRR_RESET_PB1      0x00020000     // Сбросить
#define BSRR_RESET_PC6      0x00400000     // Сбросить   
#define BSRR_RESET_PB2      0x00040000     // Сбросить 
// Количество элементов и частот
#define Number_of_freq      3
#define Number_of_elements  3

// Глобальные переменные
extern uint32_t blink_delays[Number_of_freq];               //задержки
extern uint8_t current_freq_index[Number_of_elements];      //индексы пар
extern uint8_t current_order_index;                         //выбранная пара
extern uint8_t state_array[Number_of_elements];             //флаги проверки
extern uint32_t delay_counters[Number_of_elements];         //задержки
extern uint8_t led_visible[Number_of_elements];             //видимость светодиодов
extern uint8_t btn1_prev, btn2_prev, btn3_prev, mode_prev;  // предыдущие состояния кнопок

// Прототипы функций
void GPIO_Init_All(void);
void disableAllLeds(void);
void update_states(void);
uint8_t readButton(uint32_t *idr, uint32_t mask, uint8_t* prevState);
*/