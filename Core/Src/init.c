#include "init.h"

// 2-ой вариант - через макросы
// Взять адреса из функции GPIO_Ini_Classic
#define RCC_AHB1ENR                 *(uint32_t*)(0x40023800UL + 0x30UL)
#define GPIOB_EN                    0x02UL
#define GPIOC_EN                    0x04UL

#define GPIOB_MODER                 *(uint32_t*)(0x40020400UL + 0x00UL)
#define GPIOB_OTYPER                *(uint32_t*)(0x40020400UL + 0x04UL)
#define GPIOB_OSPEEDR               *(uint32_t*)(0x40020400UL + 0x08UL)
#define GPIOB_PUPDR                 *(uint32_t*)(0x40020400UL + 0x0CUL)
#define GPIOB_BSRR                  *(uint32_t*)(0x40020400UL + 0x18UL)

#define GPIOC_MODER                 *(uint32_t*)(0x40020800UL + 0x00UL)
#define GPIOC_OTYPER                *(uint32_t*)(0x40020800UL + 0x04UL)
#define GPIOC_OSPEEDR               *(uint32_t*)(0x40020800UL + 0x08UL)
#define GPIOC_PUPDR                 *(uint32_t*)(0x40020800UL + 0x0CUL)
#define GPIOC_IDR                   *(uint32_t*)(0x40020800UL + 0x10UL)

// Макросы для настройки режимов светодиодов
// PB0
#define GPIOB_MODE_PIN0_OUT         0x1UL << 0
#define GPIOB_OTYPE_PIN0_PP         0x00
#define GPIOB_OSPEEDR_PIN0_MID      0x1UL << 0
#define GPIOB_PUPDR_PIN0_NOPUPDR    0x00 
//PB7
#define GPIOB_MODE_PIN7_OUT         0x1UL << 14
#define GPIOB_OTYPE_PIN7_PP         0x00
#define GPIOB_OSPEEDR_PIN7_MID      0x1UL << 14
#define GPIOB_PUPDR_PIN7_NOPUPDR    0x00 
//PB14
#define GPIOB_MODE_PIN14_OUT        0x1UL << 28
#define GPIOB_OTYPE_PIN14_PP        0x00
#define GPIOB_OSPEEDR_PIN14_MID     0x1UL << 28
#define GPIOB_PUPDR_PIN14_NOPUPDR   0x00 

//Макросы для настройки режимов кнопок
//PC8
#define GPIOC_MODE_PIN8_IN          0x00
#define GPIOC_OTYPE_PIN8_PP         0x00
#define GPIOC_OSPEEDR_PIN8_MID      0x00
#define GPIOC_PUPDR_PIN8_PD         0x2UL << 16
//PC9
#define GPIOC_MODE_PIN9_IN          0x00
#define GPIOC_OTYPE_PIN9_PP         0x00
#define GPIOC_OSPEEDR_PIN9_MID      0x00
#define GPIOC_PUPDR_PIN9_PD         0x2UL << 18
//PC10
#define GPIOC_MODE_PIN10_IN         0x00
#define GPIOC_OTYPE_PIN10_PP        0x00
#define GPIOC_OSPEEDR_PIN10_MID     0x00
#define GPIOC_PUPDR_PIN10_PD        0x2UL << 20
//PC13
#define GPIOC_MODE_PIN13_IN         0x00
#define GPIOC_OTYPE_PIN13_PP        0x00
#define GPIOC_OSPEEDR_PIN13_MID     0x00
#define GPIOC_PUPDR_PIN13_PD        0x2UL << 26

#define READ_BIT(REG, BIT)          (REG & BIT)
#define SET_BIT(REG, BIT)           (REG |= BIT)

#define READ_PIN8_BIT               0x1UL << 8
#define READ_PIN9_BIT               0x1UL << 9
#define READ_PIN10_BIT              0x1UL << 10
#define READ_PIN13_BIT              0x1UL << 13
#define SET_PIN0_BIT                0x1UL << 0
#define SET_PIN7_BIT                0x1UL << 7
#define SET_PIN14_BIT               0x1UL << 14
#define RESET_PIN0_BIT              0x1UL << 16               
#define RESET_PIN7_BIT              0x1UL << 23
#define RESET_PIN14_BIT             0x1UL << 30

//Инициализация системы без использования CMSIS и макросов
void GPIO_Ini_Classic(void){
    // Для начала необходимо щаписать адреса интересующих ячеек памяти
    // RCC_AHB1ENR -> 0x40023800, offset -> 0x30  
    // GPIOB -> 0x02     GPIOC -> 0x04

    // Настройка режимов работы пина
    // Мне необходимо настроить пины: PB0, PB7, PB14; PC8, PC9, PC10, PC13
    // GPIOB -> 0x40020400, GPIOC -> 0x40020800
    // GPIOx_MODER -> 0x00
    // PB0 -> 0x1 << 0, PB7 -> 0x1 << 7, PB14 -> 0x1 << 14;
    // Для портов можно установить значение по умолчанию
    // GPIOx_OTYPER -> 0x04 : Настраивается только для выхода
    // Оставляем значение по умолчанииюю
    // GPIOx_SPEEDR -> 0x08 : Устанавливаем среднюю скорость
    // PB0 -> 0x1 << 0; PB7 -> 0x1 << 7; PB14 -> 0x1 << 14;
    // GPIO_PUPDR -> 0x0C : Устанавливаем только на кн опки во избежание поломки светодиода
    // PC8 -> 0x2 << 8; PC9 -> 0x2 << 9; PC10 -> 0x2 << 10; PC13 -> 0x2 << 13
    
    //Настройка тактирования
    *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02 | 0x04;

    //Настройка пользовательских светодиодов
    //PB0 (green)
    *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x1UL << 0;
    *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00;
    *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x1UL << 0;
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00;
    //PB7 (blue)
    *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x1UL << 14;
    *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00;
    *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x1UL << 14;
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00;
    //PB14 (red)
    *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x1UL << 28;
    *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00;
    *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x1UL << 28;
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00;

    //Настройка кнопок
    //PC8
    *(uint32_t*)(0x40020800UL + 0x00UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x04UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x08UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x0CUL) |= 0x2UL << 16;
    //PC9
    *(uint32_t*)(0x40020800UL + 0x00UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x04UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x08UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x0CUL) |= 0x2UL << 18;
    //PC10
    *(uint32_t*)(0x40020800UL + 0x00UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x04UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x08UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x0CUL) |= 0x2UL << 20;
    //PC13
    *(uint32_t*)(0x40020800UL + 0x00UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x04UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x08UL) |= 0x00;
    *(uint32_t*)(0x40020800UL + 0x0CUL) |= 0x2UL << 26;
}    

//Инициализация системы с использованием макросов
void GPIO_Ini_Macros(void){
    //Настройка тактирования
    RCC_AHB1ENR |= GPIOB_EN + GPIOC_EN;

    //Настройка режимов пользовательских светодиодов
    //PB0
    GPIOB_MODER |= GPIOB_MODE_PIN0_OUT;
    GPIOB_OTYPER |= GPIOB_OTYPE_PIN0_PP;
    GPIOB_OSPEEDR |= GPIOB_OSPEEDR_PIN0_MID;
    GPIOB_PUPDR |= GPIOB_PUPDR_PIN0_NOPUPDR;
    //PB7
    GPIOB_MODER |= GPIOB_MODE_PIN7_OUT;
    GPIOB_OTYPER |= GPIOB_OTYPE_PIN7_PP;
    GPIOB_OSPEEDR |= GPIOB_OSPEEDR_PIN7_MID;
    GPIOB_PUPDR |= GPIOB_PUPDR_PIN7_NOPUPDR;
    //PB14
    GPIOB_MODER |= GPIOB_MODE_PIN14_OUT;
    GPIOB_OTYPER |= GPIOB_OTYPE_PIN14_PP;
    GPIOB_OSPEEDR |= GPIOB_OSPEEDR_PIN14_MID;
    GPIOB_PUPDR |= GPIOB_PUPDR_PIN14_NOPUPDR;

    //Настройка режимов кнопок
    //PC8
    GPIOC_MODER |= GPIOC_MODE_PIN8_IN;
    GPIOC_OTYPER |= GPIOC_OTYPE_PIN8_PP;
    GPIOC_OSPEEDR |= GPIOC_OSPEEDR_PIN8_MID;
    GPIOC_PUPDR |= GPIOC_PUPDR_PIN8_PD;
    //PC9
    GPIOC_MODER |= GPIOC_MODE_PIN9_IN;
    GPIOC_OTYPER |= GPIOC_OTYPE_PIN9_PP;
    GPIOC_OSPEEDR |= GPIOC_OSPEEDR_PIN9_MID;
    GPIOC_PUPDR |= GPIOC_PUPDR_PIN9_PD;
    //PC10
    GPIOC_MODER |= GPIOC_MODE_PIN10_IN;
    GPIOC_OTYPER |= GPIOC_OTYPE_PIN10_PP;
    GPIOC_OSPEEDR |= GPIOC_OSPEEDR_PIN10_MID;
    GPIOC_PUPDR |= GPIOC_PUPDR_PIN10_PD;
    //PC13
    GPIOC_MODER |= GPIOC_MODE_PIN13_IN;
    GPIOC_OTYPER |= GPIOC_OTYPE_PIN13_PP;
    GPIOC_OSPEEDR |= GPIOC_OSPEEDR_PIN13_MID;
    GPIOC_PUPDR |= GPIOC_PUPDR_PIN13_PD;
}

//Инициализация системы с использованием CMSIS
void GPIO_Ini_CMSIS(void){
    // Настройка портов B и C
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); //Power on B and C ports

    // Настроить светодиоды: LD1 - PB0, LD2 - PB7, LD3 - PB14
    // Режим пина - выход (Output mode -> 01)
    // Otyper - ставили 0
    // Ospeed - ставили Medium Speed (01)
    // GPIO_PUPD - отключали все резисторы (00)



    // Настроить кнопки на пинах PC8 ... PC10
    // Режим пина - вход (Input mode -> 00)
    // Otyper - настраивается только для выхода
    // Ospeed - тоже настраивается только для выхода
    // GPIO_PUPR - настроить pull_down резистор

    //Настойка светодиодов
    //Светодиод LD1 -> PB0 (Зеленый)
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0);
    //Светодиод LD2 -> PB7 (Синий)
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0);   
    //Светодиод LD3 -> PB14 (Красный)
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14_0);


    //Настройка кнопок
    //Кнопка 1 - PC8
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE8_0 | GPIO_MODER_MODE8_1);
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_8);
    CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0 | GPIO_OSPEEDER_OSPEEDR8_1);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR8_1);
    //Кнопка 2 - PC9
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE9_0 | GPIO_MODER_MODE9_1);
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_9);
    CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR9_0 | GPIO_OSPEEDER_OSPEEDR9_1);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR9_1);
    //Кнопка 3 - PC10
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE10_0 | GPIO_MODER_MODE10_1);
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_10);
    CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR10_0 | GPIO_OSPEEDER_OSPEEDR10_1);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR10_1);
    //Кнопка 4 - PC13
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE13_0 | GPIO_MODER_MODE13_1);
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_13);
    CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR13_0 | GPIO_OSPEEDER_OSPEEDR13_1);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR13_1);
}

//Код для выполнения лабораторной работы без использования CMSIS и макросов
void Task_Classic(void){
    uint8_t light_button_1 = 0;
    uint8_t light_button_2 = 7;
    uint8_t light_button_3 = 14;
    uint8_t tmp;
    while(1){
        //Считываем значение с кнопки P8
        //Значение считывается с регистра GPIO_IDR
        //GPIOC -> 0x40020800UL, GPIO_IDR -> 0x10
        //Считать нужно значение из 8 бита
        //Значение кнопки считываем с порта C
        if((*(uint32_t*)(0x40020800UL + 0x10UL) & (0x1UL << 8)) != 0){
            //Теперь необходимо занести значения в регистр GPIO_BSRR
            //GPIOB -> 0x40020400UL, GPIO_BSRR -> 0x18
            //Если на кнопку нажали, то значение надо занести в BS
            //Значение для включения диода заносим в порт B
            if (light_button_1 == 0){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 0;
            }
            if (light_button_1 == 7){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 7;
            }
            if (light_button_1 == 14){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 14;
            }
        }
        else{
            //Чтобы отключить кнопку, надо занести "1" в бит BR
            if (light_button_1 == 0){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 16;
            }
            if (light_button_1 == 7){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 23;
            }
            if (light_button_1 == 14){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 30;
            }
        }
        //Настройка кнопки PC9
        if((*(uint32_t*)(0x40020800UL + 0x10UL) & (0x1UL << 9)) != 0){
            //Теперь необходимо занести значения в регистр GPIO_BSRR
            //GPIOB -> 0x40020400UL, GPIO_BSRR -> 0x18
            //Если на кнопку нажали, то значение надо занести в BS
            //Значение для включения диода заносим в порт B
            if (light_button_2 == 0){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 0;
            }
            if (light_button_2 == 7){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 7;
            }
            if (light_button_2 == 14){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 14;
            }
        }
        else{
            //Чтобы отключить кнопку, надо занести "1" в бит BR
            if (light_button_2 == 0){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 16;
            }
            if (light_button_2 == 7){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 23;
            }
            if (light_button_2 == 14){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 30;
            }
        }
        //Настройка кнопки PC10
        if((*(uint32_t*)(0x40020800UL + 0x10UL) & (0x1UL << 10)) != 0){
            //Теперь необходимо занести значения в регистр GPIO_BSRR
            //GPIOB -> 0x40020400UL, GPIO_BSRR -> 0x18
            //Если на кнопку нажали, то значение надо занести в BS
            //Значение для включения диода заносим в порт B
            if (light_button_3 == 0){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 0;
            }
            if (light_button_3 == 7){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 7;
            }
            if (light_button_3 == 14){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 14;
            }
        }
        else{
            //Чтобы отключить кнопку, надо занести "1" в бит BR
            if (light_button_3 == 0){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 16;
            }
            if (light_button_3 == 7){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 23;
            }
            if (light_button_3 == 14){
                *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 30;
            }
        }
        //Настройка кнопки PC13
        if((*(uint32_t*)(0x40020800UL + 0x10UL) & (0x1UL << 13)) != 0){
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x1UL << 16 | 0x1UL << 23 | 0x1UL << 30;
            tmp = light_button_1;
            light_button_1 = light_button_2;
            light_button_2 = light_button_3;
            light_button_3 = tmp;
            while((*(uint32_t*)(0x40020800UL + 0x10UL) & (0x1UL << 13)) != 0){
                ;
            }
        } 
    }
}

//Код для выполнения лабораторной работы с использованием макросов
void Task_Macros(void){
    uint8_t light_button_1 = 0;
    uint8_t light_button_2 = 7;
    uint8_t light_button_3 = 14;
    uint8_t tmp;
    while(1){
        if(READ_BIT(GPIOC_IDR, READ_PIN8_BIT) != 0){
            //Теперь необходимо занести значения в регистр GPIO_BSRR
            //GPIOB -> 0x40020400UL, GPIO_BSRR -> 0x18
            //Если на кнопку нажали, то значение надо занести в BS
            //Значение для включения диода заносим в порт B
            if (light_button_1 == 0){
                SET_BIT(GPIOB_BSRR, SET_PIN0_BIT);
            }
            if (light_button_1 == 7){
                SET_BIT(GPIOB_BSRR, SET_PIN7_BIT);
            }
            if (light_button_1 == 14){
                SET_BIT(GPIOB_BSRR, SET_PIN14_BIT);
            }
        }
        else{
            //Чтобы отключить кнопку, надо занести "1" в бит BR
            if (light_button_1 == 0){
                SET_BIT(GPIOB_BSRR, RESET_PIN0_BIT);
            }
            if (light_button_1 == 7){
                SET_BIT(GPIOB_BSRR, RESET_PIN7_BIT);
            }
            if (light_button_1 == 14){
                SET_BIT(GPIOB_BSRR, RESET_PIN14_BIT);
            }
        }
        //Настройка кнопки PC9
        if(READ_BIT(GPIOC_IDR, READ_PIN9_BIT) != 0){
            //Теперь необходимо занести значения в регистр GPIO_BSRR
            //GPIOB -> 0x40020400UL, GPIO_BSRR -> 0x18
            //Если на кнопку нажали, то значение надо занести в BS
            //Значение для включения диода заносим в порт B
            if (light_button_2 == 0){
                SET_BIT(GPIOB_BSRR, SET_PIN0_BIT);
            }
            if (light_button_2 == 7){
                SET_BIT(GPIOB_BSRR, SET_PIN7_BIT);
            }
            if (light_button_2 == 14){
                SET_BIT(GPIOB_BSRR, SET_PIN14_BIT);
            }
        }
        else{
            //Чтобы отключить кнопку, надо занести "1" в бит BR
            if (light_button_2 == 0){
                SET_BIT(GPIOB_BSRR, RESET_PIN0_BIT);
            }
            if (light_button_2 == 7){
                SET_BIT(GPIOB_BSRR, RESET_PIN7_BIT);
            }
            if (light_button_2 == 14){
                SET_BIT(GPIOB_BSRR, RESET_PIN14_BIT);
            }
        }
        //Настройка кнопки PC10
        if(READ_BIT(GPIOC_IDR, READ_PIN10_BIT) != 0){
            //Теперь необходимо занести значения в регистр GPIO_BSRR
            //GPIOB -> 0x40020400UL, GPIO_BSRR -> 0x18
            //Если на кнопку нажали, то значение надо занести в BS
            //Значение для включения диода заносим в порт B
            if (light_button_3 == 0){
                SET_BIT(GPIOB_BSRR, SET_PIN0_BIT);
            }
            if (light_button_3 == 7){
                SET_BIT(GPIOB_BSRR, SET_PIN7_BIT);
            }
            if (light_button_3 == 14){
                SET_BIT(GPIOB_BSRR, SET_PIN14_BIT);
            }
        }
        else{
            //Чтобы отключить кнопку, надо занести "1" в бит BR
            if (light_button_3 == 0){
                SET_BIT(GPIOB_BSRR, RESET_PIN0_BIT);
            }
            if (light_button_3 == 7){
                SET_BIT(GPIOB_BSRR, RESET_PIN7_BIT);
            }
            if (light_button_3 == 14){
                SET_BIT(GPIOB_BSRR, RESET_PIN14_BIT);
            }
        }
        //Настройка кнопки PC13
        if(READ_BIT(GPIOC_IDR, READ_PIN13_BIT) != 0){
            SET_BIT(GPIOB_BSRR, RESET_PIN0_BIT | RESET_PIN7_BIT | RESET_PIN14_BIT);
            tmp = light_button_1;
            light_button_1 = light_button_2;
            light_button_2 = light_button_3;
            light_button_3 = tmp;
            while(READ_BIT(GPIOC_IDR, READ_PIN13_BIT) != 0){
                ;
            }
        } 
    }
}

//Код для выполнения лабораторной работы с использованием CMSIS
void Task_CMSIS(void){
    uint8_t light_button_1 = 0;
    uint8_t light_button_2 = 7;
    uint8_t light_button_3 = 14;
    uint8_t tmp;
    while(1){
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_8) != 0){
            if (light_button_1 == 0){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
            }
            if (light_button_1 == 7){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            }
            if (light_button_1 == 14){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            }
        else{
            if (light_button_1 == 0){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
            if (light_button_1 == 7){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            }
            if (light_button_1 == 14){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            }
        }
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_9) != 0){
            if (light_button_2 == 0){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
            }
            if (light_button_2 == 7){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            }
            if (light_button_2 == 14){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            }
        }
        else{
            if (light_button_2 == 0){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
            if (light_button_2 == 7){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            }
            if (light_button_2 == 14){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            }
        }

        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_10) != 0){
            if (light_button_3 == 0){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
            }
            if (light_button_3 == 7){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            }
            if (light_button_3 == 14){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            }
        }
        else{
            if (light_button_3 == 0){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
            if (light_button_3 == 7){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            }
            if (light_button_3 == 14){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            }
        } 
        
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0){
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14);
            tmp = light_button_1;
            light_button_1 = light_button_2;
            light_button_2 = light_button_3;
            light_button_3 = tmp;
            while (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0){
                ;
            }
        }   
    }
    }
}

// Доп. задание для лабораторной работы с использованием CMSIS
void Extra_Task_CMSIS(void){
    uint8_t light_button_1 = 0;
    uint8_t light_button_2 = 7;
    uint8_t light_button_3 = 14;
    uint8_t tmp;
    int16_t zero_time = 0;
    int16_t one_time = 255;
    int16_t count = 0;
    uint8_t stop_time = 250;
        while(1){
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_8) != 0){
            if(light_button_1 == 0){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
            if(light_button_1 == 7){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            }
            if(light_button_1 == 14){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            }
            tmp = light_button_1;
            light_button_1 = light_button_2;
            light_button_2 = light_button_3;
            light_button_3 = tmp;  
            while(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_8) != 0){

            }
        }
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_9) != 0){
            zero_time++;
            one_time--;
            if (zero_time > 255){
                zero_time = 255;
                one_time = 0;
            }
            while(count != stop_time){
                count++;
            }
            count = 0;
        }
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_10) != 0){
            one_time++;
            zero_time--;
            if (one_time > 255){
                one_time = 255;
                zero_time =  0;
            }
            while(count != stop_time){
                count++;
            }
            count = 0;
        }
            
        while(count != one_time){
            if(light_button_1 == 0){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
            }
            if(light_button_1 == 7){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            }
            if(light_button_1 == 14){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            }
            count++;
        }
        count = 0;

        while(count != zero_time){
            if(light_button_1 == 0){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
            if(light_button_1 == 7){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            }
            if(light_button_1 == 14){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            }
            count++;
        }
        count = 0;
    }
}