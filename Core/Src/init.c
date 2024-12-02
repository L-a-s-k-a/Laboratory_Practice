#include "init.h"
void GPIO_Ini_1(void) {
    // 开启GPIOB和GPIOC的时钟
    RCC_GPIO_EN |= (RCC_GPIOB_EN | RCC_GPIOC_EN); 

    // 配置GPIOB的引脚（LED）
    GPIOB_MODER |= (GPIOB_MODE_PIN0_OUT | GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT); 
    GPIOB_OTYPER |= (GPIOB_OTYPE_PIN0_PP | GPIOB_OTYPE_PIN7_PP | GPIOB_OTYPE_PIN14_PP); 
    GPIOB_OSPEEDR |= (GPIOB_OSPEED_PIN0_MID | GPIOB_OSPEED_PIN7_MID | GPIOB_OSPEED_PIN14_MID); 
    GPIOB_PUPDR |= (GPIOB_PUPDR_PIN0_NOPUPD | GPIOB_PUPDR_PIN7_NOPUPD | GPIOB_PUPDR_PIN14_NOPUPD); 

    // 配置GPIOC的引脚（按钮）
    // 假设按钮1连接在PC13，按钮2连接在PC12
    GPIOC_IDR |= (GPIOC_IDR_PIN13 | GPIOC_IDR_PIN12); 
}
