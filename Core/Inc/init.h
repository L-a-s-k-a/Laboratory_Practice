#include <stdint.h>

// Регистр тактирования
#define RCC_GPIO_EN             (*(uint32_t*)(0x40023800UL + 0x30UL))
#define RCC_GPIOB_EN            0x02UL
#define RCC_GPIOC_EN            0x04UL

// Регистры порта GPIOB
#define GPIOB_MODER             (*(uint32_t*)(0x40020400UL + 0x00UL))
#define GPIOB_OTYPER            (*(uint32_t*)(0x40020400UL + 0x04UL))
#define GPIOB_OSPEEDR           (*(uint32_t*)(0x40020400UL + 0x08UL))
#define GPIOB_PUPDR             (*(uint32_t*)(0x40020400UL + 0x0CUL))
#define GPIOB_BSRR              (*(uint32_t*)(0x40020400UL + 0x18UL))

// Регистры порта GPIOC
#define GPIOC_PUPDR             (*(uint32_t*)(0x40020800UL + 0x0CUL))
#define GPIOC_IDR               (*(uint32_t*)(0x40020800UL + 0x10UL))

// Настройки режимов для пинов светодиодов (PB0, PB1, PB2, PB7, PB14, PB15)
#define GPIOB_MODE_LEDS_OUT     0x50004015UL
#define GPIOB_OTYPE_LEDS_PP     0x00000000UL
#define GPIOB_OSPEED_LEDS_MID   0x50004015UL
#define GPIOB_PUPDR_LEDS_NOPUPD 0x00000000UL

// Подтяжка Pull-Down для внешних кнопок PC0 и PC3
#define GPIOC_PUPDR_BTNS_PD     0x00000082UL

// Биты установки (SET) светодиодов в регистре BSRR
#define GPIOB_BSRR_PIN0_SET     0x00000001UL
#define GPIOB_BSRR_PIN7_SET     0x00000080UL
#define GPIOB_BSRR_PIN14_SET    0x00004000UL
#define GPIOB_BSRR_PIN1_SET     0x00000002UL
#define GPIOB_BSRR_PIN2_SET     0x00000004UL
#define GPIOB_BSRR_PIN15_SET    0x00008000UL

// Биты сброса (RESET) светодиодов в регистре BSRR
#define GPIOB_BSRR_PIN0_RESET   0x00010000UL
#define GPIOB_BSRR_PIN7_RESET   0x00800000UL
#define GPIOB_BSRR_PIN14_RESET  0x40000000UL
#define GPIOB_BSRR_PIN1_RESET   0x00020000UL
#define GPIOB_BSRR_PIN2_RESET   0x00040000UL
#define GPIOB_BSRR_PIN15_RESET  0x80000000UL

// Биты чтения кнопок в регистре IDR
#define GPIOC_IDR_PIN13         0x00002000UL
#define GPIOC_IDR_PIN0          0x00000001UL
#define GPIOC_IDR_PIN3          0x00000008UL

// Пользовательские макросы
#define READ_BIT(REG, BIT)      (REG & BIT)
#define SET_BIT(REG, BIT)       (REG |= BIT)

void GPIO_Ini(void);