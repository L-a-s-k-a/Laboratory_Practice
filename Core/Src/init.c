#include "init.h"

void SystemClock_Config(void)
{
    // 启用HSE和PLL，配置系统时钟
    
    // 1. 启用HSE
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));
    
    // 2. 配置PLL
    // PLL配置: HSE/8 * 336 / 2 = 168MHz
    RCC->PLLCFGR = (8 << 0)   |  // PLLM = 8
                   (336 << 6) |  // PLLN = 336
                   (0 << 16)  |  // PLLP = 2 (00: PLLP=2)
                   (7 << 24)  |  // PLLQ = 7
                   RCC_PLLCFGR_PLLSRC_HSE;  // PLL源选择HSE
    
    // 3. 启用PLL
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));
    
    // 4. 配置闪存延迟
    FLASH->ACR = FLASH_ACR_LATENCY_5WS | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
    
    // 5. 配置时钟分频器并切换到PLL
    RCC->CFGR = (RCC_CFGR_HPRE_DIV1  |   // AHB 预分频 = 1
                 RCC_CFGR_PPRE1_DIV4 |   // APB1 预分频 = 4
                 RCC_CFGR_PPRE2_DIV2 |   // APB2 预分频 = 2
                 RCC_CFGR_SW_PLL);       // 系统时钟切换至PLL
    
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void GPIO_Init(void)
{
    // 使能GPIO时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // 启用GPIOB时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;  // 启用GPIOC时钟  
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;  // 启用GPIOG时钟
    
    // 特别：对于PC13（备份域引脚），需要启用PWR时钟并设置DBP位
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // 启用PWR时钟
    PWR->CR |= PWR_CR_DBP;                // 禁用备份域写保护
    
    // 给时钟一点时间稳定
    volatile uint32_t delay = 1000;
    while(delay--);
    
    // 配置LED引脚（PB0, PB7, PB14）- 输出模式
    // PB0 - 绿色LED
    GPIOB->MODER &= ~(3U << (0 * 2));     // 清除模式位
    GPIOB->MODER |= (1U << (0 * 2));      // 输出模式(01)
    GPIOB->OTYPER &= ~(1U << 0);          // 推挽输出
    GPIOB->OSPEEDR |= (3U << (0 * 2));    // 高速
    GPIOB->PUPDR &= ~(3U << (0 * 2));     // 无上拉/下拉
    
    // PB7 - 蓝色LED
    GPIOB->MODER &= ~(3U << (7 * 2));
    GPIOB->MODER |= (1U << (7 * 2));
    GPIOB->OTYPER &= ~(1U << 7);
    GPIOB->OSPEEDR |= (3U << (7 * 2));
    GPIOB->PUPDR &= ~(3U << (7 * 2));
    
    // PB14 - 红色LED
    GPIOB->MODER &= ~(3U << (14 * 2));
    GPIOB->MODER |= (1U << (14 * 2));
    GPIOB->OTYPER &= ~(1U << 14);
    GPIOB->OSPEEDR |= (3U << (14 * 2));
    GPIOB->PUPDR &= ~(3U << (14 * 2));
    
    // 配置按钮引脚 - 输入模式
    // PC13 - 按钮1（特别注意：备份域引脚）
    GPIOC->MODER &= ~(3U << (13 * 2));    // 输入模式(00)
    // 对于PC13，使用下拉电阻可能更可靠（按下时为高电平）
    GPIOC->PUPDR &= ~(3U << (13 * 2));
    GPIOC->PUPDR |= (2U << (13 * 2));     // 下拉(10)
    
    // PG3 - 按钮2
    GPIOG->MODER &= ~(3U << (3 * 2));     // 输入模式(00)
    GPIOG->PUPDR &= ~(3U << (3 * 2));
    GPIOG->PUPDR |= (1U << (3 * 2));      // 上拉(01)
}

void LED_Control(uint8_t led_pattern)
{
    // 使用BSRR寄存器设置LED状态
    // 先关闭所有LED
    GPIOB->BSRR = (LED_GREEN_PIN << 16) | (LED_BLUE_PIN << 16) | (LED_RED_PIN << 16);
    
    // 根据模式点亮相应LED
    if(led_pattern & 0x01)  // 绿色LED (PB0)
        GPIOB->BSRR = LED_GREEN_PIN;
    
    if(led_pattern & 0x02)  // 蓝色LED (PB7)
        GPIOB->BSRR = LED_BLUE_PIN;
    
    if(led_pattern & 0x04)  // 红色LED (PB14)
        GPIOB->BSRR = LED_RED_PIN;
}