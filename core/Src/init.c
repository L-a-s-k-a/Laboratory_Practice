#include "../Inc/init.h"

void SystemClock_Config(void)
{
  
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));

    RCC->PLLCFGR = (8 << 0) | (336 << 6) | (0 << 16) | (1 << 22) | (7 << 24);
    
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));
    

    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
    
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;   // AHB = 168MHz
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;  // APB1 = 42MHz
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;  // APB2 = 84MHz
    

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}


void GPIO_Init(void)
{

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // 启用GPIOB时钟（板载LED）
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;  // 启用GPIOC时钟（外接LED）
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // 启用GPIOA时钟（按钮2）
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;  // 启用GPIOG时钟（按钮1）
    
    LED_PORT->MODER &= ~(0x3 << (0 * 2));
    LED_PORT->MODER |= (GPIO_MODE_OUTPUT << (0 * 2));
    LED_PORT->MODER &= ~(0x3 << (7 * 2));
    LED_PORT->MODER |= (GPIO_MODE_OUTPUT << (7 * 2));
    LED_PORT->MODER &= ~(0x3 << (14 * 2));
    LED_PORT->MODER |= (GPIO_MODE_OUTPUT << (14 * 2));
    LED_PORT->OTYPER &= ~(LED1_PIN | LED2_PIN | LED3_PIN);
    LED_PORT->OSPEEDR &= ~(0x3 << (0 * 2) | 0x3 << (7 * 2) | 0x3 << (14 * 2));
    LED_PORT->OSPEEDR |= (0x2 << (0 * 2) | 0x2 << (7 * 2) | 0x2 << (14 * 2));
    LED_PORT->PUPDR &= ~(0x3 << (0 * 2) | 0x3 << (7 * 2) | 0x3 << (14 * 2));

EXT_LED_PORT->MODER &= ~(0x3 << (10 * 2));
EXT_LED_PORT->MODER |= (GPIO_MODE_OUTPUT << (10 * 2));

EXT_LED_PORT->MODER &= ~(0x3 << (11 * 2));
EXT_LED_PORT->MODER |= (GPIO_MODE_OUTPUT << (11 * 2));

EXT_LED_PORT->MODER &= ~(0x3 << (12 * 2));
EXT_LED_PORT->MODER |= (GPIO_MODE_OUTPUT << (12 * 2));

EXT_LED_PORT->OTYPER &= ~(EXT_LED1_PIN | EXT_LED2_PIN | EXT_LED3_PIN);

EXT_LED_PORT->OSPEEDR &= ~(0x3 << (10 * 2) | 0x3 << (11 * 2) | 0x3 << (12 * 2));
EXT_LED_PORT->OSPEEDR |= (0x2 << (10 * 2) | 0x2 << (11 * 2) | 0x2 << (12 * 2));

EXT_LED_PORT->PUPDR &= ~(0x3 << (10 * 2) | 0x3 << (11 * 2) | 0x3 << (12 * 2));
    
    LED_PORT->BSRR = (LED1_PIN << 16) | (LED2_PIN << 16) | (LED3_PIN << 16);
    EXT_LED_PORT->BSRR = (EXT_LED1_PIN << 16) | (EXT_LED2_PIN << 16) | (EXT_LED3_PIN << 16);
    
    BTN1_PORT->MODER &= ~(0x3 << (3 * 2));
    BTN1_PORT->PUPDR &= ~(0x3 << (3 * 2));
    BTN1_PORT->PUPDR |= (GPIO_PULLUP << (3 * 2));

    BTN2_PORT->MODER &= ~(0x3 << (0 * 2));
    BTN2_PORT->PUPDR &= ~(0x3 << (0 * 2));
    BTN2_PORT->PUPDR |= (GPIO_PULLUP << (0 * 2));
}


uint8_t Read_BTN1(void)
{
    return (BTN1_PORT->IDR & BTN1_PIN) ? 0 : 1;
}

uint8_t Read_BTN2(void)
{
    return (BTN2_PORT->IDR & BTN2_PIN) ? 0 : 1;
}

void LED_Write(uint8_t leds)
{
    uint32_t set_mask = 0;
    uint32_t reset_mask = 0;
    
    if(leds & 0x01) set_mask |= LED1_PIN;
    else reset_mask |= LED1_PIN;
    
    if(leds & 0x02) set_mask |= LED2_PIN;
    else reset_mask |= LED2_PIN;
    
    if(leds & 0x04) set_mask |= LED3_PIN;
    else reset_mask |= LED3_PIN;
    
    LED_PORT->BSRR = (reset_mask << 16) | set_mask;
}


void Ext_LED_Write(uint8_t leds)
{
    uint32_t set_mask = 0;
    uint32_t reset_mask = 0;
    
    if(leds & 0x01) set_mask |= EXT_LED1_PIN;
    else reset_mask |= EXT_LED1_PIN;
    
    if(leds & 0x02) set_mask |= EXT_LED2_PIN;
    else reset_mask |= EXT_LED2_PIN;
    
    if(leds & 0x04) set_mask |= EXT_LED3_PIN;
    else reset_mask |= EXT_LED3_PIN;
    
    EXT_LED_PORT->BSRR = (reset_mask << 16) | set_mask;
}