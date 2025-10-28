#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F411xE/stm32f411xe.h"

#define RCC_GPIO_EN                 (*(uint32_t *)(0x40023800UL + 0x30UL))

#define RCC_GPIOA_EN                0x01UL

#define GPIOA_MODER                 (*(uint32_t *)(0x40020000UL + 0x00UL))
#define GPIOA_OTYPER                (*(uint32_t *)(0x40020000UL + 0x04UL))
#define GPIOA_OSPEEDR               (*(uint32_t *)(0x40020000UL + 0x08UL))
#define GPIOA_PUPDR                 (*(uint32_t *)(0x40020000UL + 0x0CUL))
#define GPIOA_BSRR                  (*(uint32_t *)(0x40020000UL + 0x18UL))

#define GPIOA_MODE_PIN0_OUT         0x01UL
#define GPIOA_OTYPE_PIN0_PP         0x00UL
#define GPIOA_OSPEED_PIN0_MID       0x01UL
#define GPIOA_PUPDR_PIN0_NUPUPD     0x00UL
#define GPIOA_BSSR_PIN0_SET         0x01UL
#define GPIOA_BSSR_PIN0_RESET       0x10000UL

#define SET_BIT(REG, BIT)           (REG |= BIT)

void GPIO_Ini(void);                                            // Инициализация переменных
uint8_t click_counter(uint8_t control, uint8_t click);          // Счетчик нажатий
void turning_on_the_LED(uint8_t LED);                           // Основное задание
void LED_flickering(uint8_t LED);                               // Дополнительное задание
void change_PC12(uint8_t LED_pc13);                             // Изменение параметров работы порта PC12