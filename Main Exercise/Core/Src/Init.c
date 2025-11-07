#include "init.h"

#include "../../CMSIS/Devices/STM32F1xx/Inc/STM32f1xx.h"
#include "../../CMSIS/Devices/STM32F1xx/Inc/STM32f103xB/stm32f103xb.h"

void GPIO_Ini(void){
    
    // Включаем тактирование порта A
    *(uint32_t*)(0x40021000 + 0x18) |= (1UL << 2UL); // IOPAEN = 1
    
    // Настраиваем пины PA5–PA10 как выходы push-pull 2MHz
    // MODE=10 (2 MHz), CNF=00 (push-pull) → 0x2
    // PA5–PA7 находятся в CRL, PA8–PA10 — в CRH
    // CRL: PA5, PA6, PA7
    // Очистка всей конфигурации для конкретных пинов
    *(uint32_t*)(0x40010800 + 0x00) &= ~((0x3UL << (20UL)) |
                                         (0x3UL << (22UL)) |
                                         (0x3UL << (24UL)) |
					                     (0x3UL << (26UL)) |
					                     (0x3UL << (28UL)) |
					                     (0x3UL << (30UL)));

    //Устанавливаем частоту PA5, PA6, PA7
    *(uint32_t*)(0x40010800) |=  ((0x2UL << (20UL)) |
                                  (0x2UL << (24UL)) |
                                  (0x2UL << (28UL)));
    
    // CRH: PA8, PA9, PA10
    *(uint32_t*)(0x40010804) &= ~((0x3UL << (0UL))  | 
                                           (0x3UL << (2UL))  | 
                                           (0x3UL << (4UL))  |
                                           (0x3UL << (6UL))  |
                                           (0x3UL << (8UL))  |
                                           (0x3UL << (10UL))) ;

    //Устанавливаем частоту 2 МГц
    SET_BIT(GPIOA->CRH, GPIO_CRH_MODE8_1);
    SET_BIT(GPIOA->CRH, GPIO_CRH_MODE9_1);
    SET_BIT(GPIOA->CRH, GPIO_CRH_MODE10_1);

    //Настраиваем кнопки PA0, PA1 как входы с подтяжкой вверх
    GPIO_PORT_A_CRL &= ~(CRH_MODE_8 |
                                  CRH_CNF_8 |
                                  CRH_MODE9_2 |
                                  CRH_CNF_9);

    GPIO_PORT_A_CRL |=  (CRL_CNF0_1 |
                                  CRL_CNF1_1);  // CNF=10, MODE=00

    
                        
    GPIO_PORT_A_ODR |= (ODR0_1 | ODR1_1); // подтяжка 
    //Подаём питание на при нажатии



}

