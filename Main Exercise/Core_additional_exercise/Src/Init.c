#include "init.h"
<<<<<<< HEAD

void GPIO_Ini(void){
    
=======
#include "../../CMSIS/Devices/STM32F1xx/Inc/STM32f1xx.h"
#include "../../CMSIS/Devices/STM32F1xx/Inc/STM32f103xB/stm32f103xb.h"

void GPIO_Ini(void){
    
    //В начале идёт код, написанный на регистрах
>>>>>>> 1ec0a78e3ece6888e362f4c478aa3eb15ff44c45
    // Включаем тактирование порта A
    *(uint32_t*)(0x40021000 + 0x18) |= (1UL << 2UL); // IOPAEN = 1
    
    // Настраиваем пины PA5–PA10 как выходы push-pull 2MHz
    // MODE=10 (2 MHz), CNF=00 (push-pull) → 0x2
    // PA5–PA7 находятся в CRL, PA8–PA10 — в CRH
    // CRL: PA5, PA6, PA7
    // Очистка всей конфигурации для конкретных пинов
    *(uint32_t*)(0x40010800 + 0x00) &= ~((0x3UL << (20UL)) |
<<<<<<< HEAD
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
    *(uint32_t*)(0x40010804) |=  ((0x2UL << (0UL)) | 
                                  (0x2UL << (4UL)) | 
                                  (0x2UL << (8UL)));

    //Настраиваем кнопки PA0, PA1 как входы с подтяжкой вверх
    *(uint32_t*)(0x40010800) &= ~((0x3UL << (0UL)) |
                                  (0x3UL << (2UL)) |
                                  (0x3UL << (4UL)) |
                                  (0x3UL << (6UL)));

    *(uint32_t*)(0x40010800) |=  ((0x2UL << (2UL)) |
                                  (0x2UL << (6UL)));  // CNF=10, MODE=00

    
                        
    *(uint32_t*)(0x4001080C) |= ((0x1UL << 0UL) | (0x1UL << 1UL)); // подтяжка 
    //Подаём питание на при нажатии



=======
                                     (0x3UL << (22UL)) |
                                     (0x3UL << (24UL)) |
				                     (0x3UL << (26UL)) |
				                     (0x3UL << (28UL)) |
				                     (0x3UL << (30UL)));

    //Устанавливаем частоту PA5, PA6, PA7
    *(uint32_t*)(0x40010800) |=  ((0x2UL << (20UL)) |
                                   (0x2UL << (24UL)) |
                                   (0x2UL << (28UL)));
    
    //Затем идёт код написанный на #define
    //CRH: PA8, PA9, PA10
    GPIO_PORT_A_CRH &= ~(CRH_MODE_8  | 
                         CRH_CNF_8   | 
                         CRH_MODE_9  |
                         CRH_CNF_9   |
                         CRH_MODE_10 |
                         CRH_CNF_10) ;

    //Устанавливаем частоту 2 МГц
    GPIO_PORT_A_CRH |= (CRH_MODE8_2  | 
                        CRH_MODE9_2  | 
                        CRH_MODE10_2);

        
    //Дальше идёт код, написанный с использованием CMSIS
    //Настраиваем кнопки PA0, PA1 как входы с подтяжкой вверх
    CLEAR_BIT(GPIOA->CRL, GPIO_CRL_CNF0);
    CLEAR_BIT(GPIOA->CRL, GPIO_CRL_MODE0);
    CLEAR_BIT(GPIOA->CRL, GPIO_CRL_CNF1);
    CLEAR_BIT(GPIOA->CRL, GPIO_CRL_MODE1);

    SET_BIT(GPIOA->CRL, GPIO_CRL_CNF0_1);
    SET_BIT(GPIOA->CRL, GPIO_CRL_CNF1_1);  // CNF=10, MODE=00

    
                        
    SET_BIT(GPIOA->ODR, GPIO_ODR_ODR0);
    SET_BIT(GPIOA->ODR, GPIO_ODR_ODR1); // подтяжка 
    //Подаём питание на при нажатии
>>>>>>> 1ec0a78e3ece6888e362f4c478aa3eb15ff44c45
}

