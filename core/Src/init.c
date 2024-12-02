#include "init.h"

void GPIO_Ini_Self_Def(void)
{
    SET_BIT_SELF(RCC_AHB1ENR,RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); /*|按位取或,a|=b代表a=a|b*/
    SET_BIT_SELF(GPIOB_MODER, GPIO_PIN_7_OUT); /*设置pb0=7 MODER0为01(General purpose output mode)*/
    SET_BIT_SELF(GPIOB_OSPEEDR, GPIO_PIN_7_MED);
    SET_BIT_SELF(GPIOB_BSRR, GPIO_PIN_7_RESET);
     // 配置 PC13 和 PC6 为输入模式
    GPIOC->MODER &= ~(GPIO_MODER_MODE13);
    // // 配置为上拉模式
    // GPIOC->PUPDR |= (GPIO_PUPDR_PUPD13_0 | GPIO_PUPDR_PUPD6_0);

}

void GPIO_Ini_CMSIS(void)
{
    /*使用gpiob点亮pb14红色led */
    SET_BIT(GPIOB->MODER,GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER,GPIO_OTYPER_OT14); /*重置pb14 OTYPER为0(Output push-pull)？？*/
    SET_BIT(GPIOB->OSPEEDR,GPIO_OSPEEDER_OSPEEDR14_0);
    SET_BIT(GPIOB->BSRR,GPIO_BSRR_BR14);

    /*---SET PB0 FOR GREEN LED*/
    SET_BIT(GPIOB->MODER,GPIO_MODER_MODE0_0); /*设置pb0 MODER0为01(General purpose output mode)*/
    SET_BIT(GPIOB->OSPEEDR,GPIO_OSPEEDER_OSPEEDR0_0); /*设置pb0 设置端⼝引脚的速度为01(中速)*/
    SET_BIT(GPIOB->BSRR,GPIO_BSRR_BR0); /*设置pb0 bsrr寄存器复位br0 （置位bs0）*/
   
}