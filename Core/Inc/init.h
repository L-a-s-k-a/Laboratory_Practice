#ifndef INIT_H
#define INIT_H
 

#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include <stdint.h>

// Адреса портов и пинов
#define LED_GREEN   (*(uint32_t*)(0x40020C00UL + 0x18UL)) // PD1
#define LED_BLUE    (*(uint32_t*)(0x40020000UL + 0x18UL)) // PA0
#define LED_RED     (*(uint32_t*)(0x40021000UL + 0x18UL)) // PE0

//LID
#define RCC_GPIO_EN                 (*(uint32_t*)(0x40023800+0x30UL))
#define RCC_GPIOE_EN                0x10UL

//GPIOE_REG
#define GPIOE_MODER_REG          (*(uint32_t*)(0x40021000UL + 0x00UL)) // настроили порт A0 на выход
#define GPIOE_OTYPER_REG         (*(uint32_t*)(0x40021000UL + 0x04UL)) //настройка типа выхода
#define GPIOE_OSPEEDR_REG        (*(uint32_t*)(0x40021000UL + 0x08UL)) //устанавливаем скорость работы порта
#define GPIOE_PUPDR_REG          (*(uint32_t*)(0x40021000UL + 0x0CUL)) //no pull-resistor
#define GPIOE_BSRR_REG           (*(uint32_t*)(0x40021000UL + 0x18UL)) //записываем или стираем бит

//GPIOE_BIT
#define GPIOE_MODER_BIT          0b01UL
#define GPIOE_OTYPER_BIT         0b0UL
#define GPIOE_OSPEEDR_BIT        0b01Ul
#define GPIOE_PUPDR_BIT          0b00UL
#define GPIOE_BSRR_BIT           0x10000UL

//button
#define RCC_GPIO_C               (*(uint32_t*)(0x40021000UL + 0x00UL))
#define RCC_GPIOC                0x30UL

#define GPIOC_OTYPER_REG         (*(uint32_t*)(0x40021000UL + 0x04UL))
#define GPIOC_PUPDR_REG          (*(uint32_t*)(0x40021000UL + 0x0CUL))

#define GPIOC_OTYPER_BIT         0x4UL
#define GPIOC_PUPDR_BIT          0x10UL


#define MY_READ_BIT(REG, BIT)          ((REG) &= ~(BIT)) 
#define MY_SET_BIT(REG, BIT)           ((REG) |= (BIT)) 

// Инициализация портов
void Init_PORT_D_withMemory();      // PD1 - светодиод (синий)
void Init_PORT_A_withMemory();      // PA0 - светодиод (зелёный)
void Init_PORT_E_withMemory();      // PE0 - светодиод (красный)

void Init_Port_D_withMemory_Button(); // PD0 - кнопка 1
void Init_Port_A_withMemory_Button(); // PA4 - кнопка 2
void Init_Port_E_withMemory_Button(); // PE2 - кнопка 3

void Init_Port_C_with_Memory_Button(); //P13 - кнопка


void HandleButton(uint8_t btnState, uint8_t base, uint8_t rot);

//-----------------------КОД ДЛЯ ЗАЩИТЫ-----------------------//

void handle_button(uint8_t btnNum,   // номер кнопки: 0, 1, 2 (для кнопок 1,2,3)
                          uint8_t b,        // текущее состояние кнопки (0/1)
                          uint8_t b4,       // состояние кнопки 4 (для prevB4)
                          uint8_t rot, 
                          uint8_t *prevB, 
                          uint8_t *prevB4,
                          uint8_t *activeLed,
                          uint8_t *freqIndex);

void blink_active_led(uint8_t activeLed, uint8_t freqIndex);

#endif