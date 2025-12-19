#include "init.h"

void RCC_Init(void)
{
   MODIFY_REG(RCC->CR,RCC_CR_HSITRIM, 0x80U);//Калибровка точности внутренного генератора на заводское значение 
   CLEAR_REG(RCC->CFGR);// Сброс всех настроек делителей и мультиплексоров
   while(READ_BIT(RCC->CFGR,RCC_CFGR_SWS) != RESET);//Ожидание пока система переключится на источник тактирования по умолчанию
   CLEAR_BIT(RCC->CR, RCC_CR_PLLON);//Выключение системы умножения частоты PLL
   while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);//Ожидание полного выключения PLL
   CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);// Одновременное выключение внешнего кварцевого генератора HSE и детектора отказа тактирования CSS
   while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);//Ожидание полного выключения тактирования
   CLEAR_BIT(RCC->CR,RCC_CR_HSEBYP);//Отключение режима обхода HSE
   
   SET_BIT(RCC->CR, RCC_CR_HSEON);//Включение внешнего кварцевого генератора HSE
   while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);//Ожидание стабилизации HSE
   CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);// Повторное подтверждение использования HSE
   SET_BIT(RCC->CR, RCC_CR_CSSON);//Включение детектора отказа HSE, если он откажет, то система автоматически переключится на HSI

   CLEAR_REG(RCC->PLLCFGR);//Полная очистка регистра PLL
   SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);// Выбор HSE как источника тактирования PLL
   MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2);// Установка делителя PLLM равным 4
   MODIFY_REG(RCC->PLLCFGR,RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | 
    RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7);//Установка умножителя PLLN равным 180
   CLEAR_BIT(RCC->PLLCFGR,RCC_PLLCFGR_PLLP_Msk);// Установка делителя PLLP равным 2
   SET_BIT(RCC->CR, RCC_CR_PLLON);//Запуск PLL с новой конфигурацией
   while(READ_BIT(RCC->CR, RCC_CR_PLLRDY));//Ожидание готовности PLL

   MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);//Переключение системного тактирования SYSCLK на выход PLL
   MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);//Установка делителя шины AHB равным 1
   MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);//Установка делителя шины APB1 равным 4
   MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);//Установка делителя шины APB2 равным 2
   MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_Msk);//Установка максимального делителя для выхода MCO2
   CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);// Выбор SYSCLK как источника для MCO2
   MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);//Установка задержки Flash памяти равным 5 циклам ожидания для корректного чтения
}

void GPIO_Init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);//Включение тактирования GPIOB
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);//Установка режима ввода для пина PB0
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT0);//Установка пина PB0 как выход push-pull
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);//Установка низкой скорости переключения для PB0
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR0_1);//Отключение подтягивающих резисторов для PB0

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);//Установка режима ввода для пина PB7
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT7);//Установка пина PB7 как выход push-pull
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);//Установка низкой скорости переключения для PB7
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR7_1);//Отключение подтягивающих резисторов для PB7

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);//Установка режима ввода для пина PB14
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT14);//Установка пина PB14 как выход push-pull
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);//Установка низкой скорости переключения для PB14
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14_0 | GPIO_PUPDR_PUPDR14_1);//Отключение подтягивающих резисторов для PB14

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);//Включение тактирования GPIOD
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE0_0);//Установка режима ввода для пина PD0
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT0);//Установка пина PD0 как выход push-pull
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);//Установка низкой скорости переключения для PD0
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR0_1);//Отключение подтягивающих резисторов для PD0

    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE1_0);//Установка режима ввода для пина PD1
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT1);//Установка пина PD1 как выход push-pull
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR1_0);//Установка низкой скорости переключения для PD1
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR1_1);//Отключение подтягивающих резисторов для PD1

    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE2_0);//Установка режима ввода для пина PD2
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT2);//Установка пина PD2 как выход push-pull
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR2_0);//Установка низкой скорости переключения для PD2
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR2_1);//Отключение подтягивающих резисторов для PD2

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);//Включение тактирования GPIOC
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE13);// Установка пина PC13 в режим входа

    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD13);//Отключение подтягивающих резисторов для пина PC13
}

void SetLED(uint8_t led_num, uint8_t state)
{
    switch(led_num)//Проверка всех возможных значений счётчика led
    {
        case 1:
            if (state) {//Если нужно включить первый светодиод
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);//Выключение светодиода на пине PD2
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);//Включение светодиода на пине PB0
            } else {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);//Иначе выключение PB0
            }
            break;//Выход
        case 2:
            if (state) {//Если нужно включить второй светодиод
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);//Выключение светодиода на пине PB0
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);//Включение светодиода на пине PB7
            } else {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//Иначе выключение PB7
            }
            break;//Выход
        case 3:
            if (state) {//Если нужно включить третий светодиод
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//Выключение светодиода на пине PB7
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);//Включение светодиода на пине PB14
            } else {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);//Иначе выключение PB14
            }
            break;//Выход
        case 4:
            if (state) {//Если нужно включить четвертый светодиод
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);//Выключение светодиода на пине PB14
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS0);//Включение светодиода на пине PD0
            } else {
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR0);//Иначе выключение PD0
            }
            break;//Выход
        case 5:
            if (state) {//Если нужно включить пятый светодиод
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR0);//Выключение светодиода на пине PD0
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS1);//Включение светодиода на пине PD1
            } else {
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR1);//Иначе выключение PD1
            }
            break;//Выход
        case 6:
            if (state) {//Если нужно включить шестой светодиод
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR1);//Выключение светодиода на пине PD1
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);//Включение светодиода на пине PD2
            } else {
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);//Иначе выключение PD2
            }
            break;//Выход
    }
}