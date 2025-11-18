#include "init.h"

// 设置PC8为输出模式（用于控制LED4）Настройка PC8 как выхода (для управления светодиодом LED4)
void set_PC8_as_output(void)
{
    // 设置为输出模式 (01) Установка режима вывода(01)
    MODIFY_REG(GPIOC->MODER, 
               GPIO_MODER_MODE8,
               GPIO_MODER_MODE8_0);
    // 推挽输出 Режим push-pull
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_8);
    // 中等速度 Средняя скорость
    MODIFY_REG(GPIOC->OSPEEDR,
               GPIO_OSPEEDR_OSPEED8,
               GPIO_OSPEEDR_OSPEED8_1);
    // 无上拉/下拉
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD8);
}

// 设置PC8为输入模式（用于检测按钮）Настройка PC8 как входа (для обнаружения нажатия кнопки)
void set_PC8_as_input(void)
{
    // 设置为输入模式 (00) Установка режима ввода (00)
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE8);
    // 启用上拉 Включение подтяжки pull-up (сверху
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD8_0);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD8_1);
}

void GPIO_Init_Self_Def(void)
{
    // 1. 使能GPIOB和GPIOC时钟 Включение тактирования GPIOB и GPIOC
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    // 2. 初始化PC8和PC13
    // ---------- PC8 (按钮1 + LED4) ----------
    // 初始化为输入模式（上拉），用于按钮检测
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE8);   // PC8 input
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD8_0);   // 上拉电阻
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD8_1);

    // ---------- PC13 (按钮2) ----------
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE13);  // PC13 input
    //  PC13应该使用下拉
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD13_0); // 
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD13_1);   // 设置bit1，启用下拉 

    // 3. 直接内存访问配置 LED1 (PB0) Использование прямого доступа к памяти для настройки светодиода LED1 (PB0)
    // 配置 PB0 为输出模式 (01) -> MODER[1:0] = 01 Настройка PB0 как вывод (01) -> MODER[1:0] = 01
    uint32_t temp_mod = *(volatile uint32_t*)((uint32_t)GPIOB + 0x00); // 读取 MODER
    temp_mod &= ~(0x3UL << (2 * LED1_PIN)); // 清除 PB0 的 MODER 位 
    temp_mod |=  (0x1UL << (2 * LED1_PIN)); // 设置 PB0 为输出 
    *(volatile uint32_t*)((uint32_t)GPIOB + 0x00) = temp_mod; // 写回 MODER

    // 设置 PB0 为推挽输出 (OTYPER 默认为 0，已经是推挽，可选)
    // CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0); // 这行可选，因为默认值是0

    // 设置 PB0 为中等速度 (OSPEEDER[1:0] = 01) -> OSPEEDR[1:0] = 01
    uint32_t temp_spd = *(volatile uint32_t*)((uint32_t)GPIOB + 0x08); // 
    temp_spd &= ~(0x3UL << (2 * LED1_PIN)); // 清除 PB0 的 OSPEEDER 位
    temp_spd |=  (0x1UL << (2 * LED1_PIN)); // 设置 PB0 为中等速度 
    *(volatile uint32_t*)((uint32_t)GPIOB + 0x08) = temp_spd; //

    // 设置 PB0 无上拉/下拉 
    uint32_t temp_pup = *(volatile uint32_t*)((uint32_t)GPIOB + 0x0C); // 
    temp_pup &= ~(0x3UL << (2 * LED1_PIN)); // 清除 PB0 的 PUPDR 位 (00)
    // 无需再或上值，因为清除后就是00
    *(volatile uint32_t*)((uint32_t)GPIOB + 0x0C) = temp_pup; // 

    // 4. 使用自定义宏和SET_BIT配置 LED2 (PB7) Использование пользовательских макросов и SET_BIT для настройки светодиода LED2 (PB7)
    // 配置 PB7 为输出模式 
    // 1. 清除 PB7 的模式位 
    CLEAR_BIT(GPIOB->MODER, (0x3UL << (LED2_PIN * 2U)));
    // 2. 设置 PB7 为输出模式 (MODE7_0 = 1) -> MODER[14] = 1
    SET_BIT(GPIOB->MODER, (0x1UL << (LED2_PIN * 2U)));

    // 配置 PB7 为推挽输出 (OTYPER[7] = 0) -> 默认值，可选设置
    CLEAR_BIT(GPIOB->OTYPER, (0x1UL << LED2_PIN));

    // 配置 PB7 为中等速度 (OSPEEDR[15:14] = 01) -> OSPEEDR[14] = 1
    // 清除位
    CLEAR_BIT(GPIOB->OSPEEDR, (0x3UL << (LED2_PIN * 2U)));
    // 设置位
    SET_BIT(GPIOB->OSPEEDR, (0x1UL << (LED2_PIN * 2U)));

    // 配置 PB7 无上拉/下拉 (PUPDR[15:14] = 00) -> PUPDR[14] = 0, PUPDR[15] = 0
    CLEAR_BIT(GPIOB->PUPDR, (0x3UL << (LED2_PIN * 2U)));

    // 5. 初始化 LED3 (PB14) 
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0); // PB14 output
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14); // push-pull
    MODIFY_REG(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED14, GPIO_OSPEEDR_OSPEED14_1); // medium speed
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD14); // no pull-up/pull-down
}