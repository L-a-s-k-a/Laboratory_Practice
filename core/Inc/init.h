
#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"

/* RCC address offset */
#define RCC_AHB1ENR          *(uint32_t *)(0x40023800UL + 0x30UL) 

#define RCC_AHB1ENR_GPIOBEN  0X02
#define RCC_AHB1ENR_GPIOCEN  0X04

/* GPIOB addresses */
#define GPIOB_MODER          *(uint32_t *)(0x40020400UL + 0x00UL)  
#define GPIOB_OSPEEDR        *(uint32_t *)(0x40020400UL + 0x08UL)
#define GPIOB_BSRR           *(uint32_t *)(0x40020400UL + 0x18UL)

/* Custom bit masks for PB7 setup */
#define GPIO_PIN_7_OUT       0x4000UL  
#define GPIO_PIN_7_MED       0x4000UL 
#define GPIO_PIN_7_SET       0x80UL  /* Blue LED */
#define GPIO_PIN_1_SET       0x00004000 /* Red LED */
#define GPIO_PIN_2_SET       0x00000001 /* Green LED */
#define GPIO_PIN_7_RESET     0x800000UL 

/* LED control bits */
#define LED2_ON              0x80UL 
#define LED1_ON              0x00004000 
#define LED3_ON              0x00000001 
#define LED1_OFF             (0x80UL << 16)       
#define LED2_OFF             (0x00004000UL << 16) 
#define LED3_OFF             (0x00000001UL << 16) 

/* Button pins */
#define BUTTON1_PIN          (1U << 13) /* PC13 */
#define BUTTON2_PIN          (1U << 6)  /* PC6 */

/* For convenience */
#define LED_PORT             GPIOB
#define BUTTON_PORT          GPIOC

/* GPIOC input data register offset */
#define GPIOC_IDR            *(uint32_t *)(0x40020800UL + 0x10UL)  
#define GPIO_PIN_6           0x00000040UL 
#define GPIO_PIN_13          0x2000UL 

/* Bit manipulation macros */
#define READ_BIT_SELF(REG, BIT)  ((REG) & (BIT))
#define SET_BIT_SELF(REG, BIT)   ((REG) |= (BIT))

/* Function prototypes */
void InitializeGPIOSelf(void);
void InitializeGPIOCMSIS(void);
void DelayLoop(volatile uint32_t cycleCount);

#endif /* INIT_H */
