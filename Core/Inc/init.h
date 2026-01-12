 #include <stdint.h>
 #include "stm32f4xx.h"
 #include "system_stm32f4xx.h"
 void GPIO_Init(void);
     // ====================================
     // 1. Директива включения тактирования GPIO
     #define RCC_GPIO_EN                             (*(uint32_t *)(0x40023800UL + 0x30UL))
     // ====================================
     // 2. Смещение адреса тактирования светодиодов и кнопок в GPIOВ, GPIOC
     #define RCC_GPIOB_EN                            0x02UL
     #define RCC_GPIOC_EN                            0x04UL
     // ======================================================
     // 3. Назначение области адресов GPIOB, GPIOC на MODER, OTYPER, OSPEEDR, PUPDR, IDR, BSRR
     #define GPIOB_MODER                             (*(uint32_t *)(0x40020400UL + 0x00UL))
     #define GPIOB_OTYPER                            (*(uint32_t *)(0x40020400UL + 0x04UL))
     #define GPIOB_OSPEEDR                           (*(uint32_t *)(0x40020400UL + 0x08UL))
     #define GPIOB_PUPDR                             (*(uint32_t *)(0x40020400UL + 0x0CUL))
     #define GPIOC_MODER                             (*(uint32_t *)(0x40020800UL + 0x00UL))
     #define GPIOC_PUPDR                             (*(uint32_t *)(0x40020800UL + 0x0CUL))
     #define GPIOB_BSRR                              (*(uint32_t *)(0x40020400UL + 0x18UL))
     // ======================================================
     // 4. Смещение адресов для светодиодов PB5, PB8, PB12 и кнопок РС6 и РС7 на MODER, OTYPER, OSPEEDR, PUPDR,
     #define LED_1_MODE_OUT                         0x0400UL
     #define LED_2_MODE_OUT                         0x010000UL
     #define LED_3_MODE_OUT                         0x01000000UL
     #define LED_1_OTYPE_PP                         0x20UL
     #define LED_2_OTYPE_PP                         0x0100UL
     #define LED_3_OTYPE_PP                         0x1000UL
     #define LED_1_SPEED_MID                        0x0400UL
     #define LED_2_SPEED_MID                        0x010000UL
     #define LED_3_SPEED_MID                        0x01000000UL
     #define LED_1_PUPDR_NO                         0x0C00UL
     #define LED_2_PUPDR_NO                         0x030000UL
     #define LED_3_PUPDR_NO                         0x03000000UL
     #define BUT_1_MODE_IN                          0xC000UL
     #define BUT_2_MODE_IN                          0x3000UL
     #define BUT_1_PUPDR_PUP                        0x4000UL
     #define BUT_2_PUPDR_PUP                        0x1000UL
     #define BUT_2_IDR                              0x40UL
     #define BUT_1_IDR                              0x80UL
     #define LED_1_TURN_ON                          0x0100UL
     #define LED_2_TURN_ON                          0x20UL
     #define LED_3_TURN_ON                          0x1000UL
     #define LED_1_TURN_OFF                         0x01000000UL
     #define LED_2_TURN_OFF                         0x200000UL
     #define LED_3_TURN_OFF                         0x10000000UL
     // ======================================================
     // 5. Считывание состяние кнопок IDR
     #define READ_BUT_1                             ((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x80UL))
     #define READ_BUT_2                             ((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x40UL))




     #define BIT_SET(REG, BIT)       ((REG) |= (BIT))
     #define BIT_CLEAR(REG, BIT)     ((REG) &= ~(BIT))
     #define BIT_READ(REG, BIT)      ((REG) & (BIT))


