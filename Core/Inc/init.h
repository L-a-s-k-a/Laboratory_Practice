#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"
#define CLEAR_bit(REG,BIT)REG&= ~BIT
#define READ_bit(REG,BIT)REG &BIT
#define SET_bit (REG, BIT) REG|= BIT
#define GPIOB_address 0x40020400UL
#define GPIOB_MODER (0x40020400UL + 0x00)
#define GPIOB_MODER (0x40020400UL + 0x04)
#define GPIOB_MODER (0x40020400UL + 0x08)
#define GPIOB_MODER (0x40020400UL + 0x0C)
#define GPID_MODERX_0(X) (0x1UL << (X*2))
#define GPID_MODERX_1(X) (0x2UL << (X*2))
#define GPID_OSPEEDRX_0(X) (0x1UL << (X*2))
#define GPID_OSPEEDRX_1(X) (0x2UL << (X*2))
void GPIO_Init();
void GPIO_Init_const_LED_button();
void LED1_out(); //LED1 - PA3
void LED2_out(); //LED2 - PC0
void LED3_out(); //LED3 - PC3
void LED1_in();
void LED2_in();
void LED3_in();
