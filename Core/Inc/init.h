#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F411xE/stm32f411xe.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"

// первой строкой обращаемся к RCC регистру, отвечающему за включение шин, второй - включаем шину А
#define RCC_AHB1EN *(uint32_t *)(0x40023800UL + 0x30UL)
#define RCC_GPIOAEN_EN 0x1UL

// первой строкой обращаемся к GPIOA регистру, отвечающему за изменение режима работы портов, второй - устанавливаем пину 4 режим работы на выход
#define RCC_GPIOA_MODER *(uint32_t *)(0x40020000UL + 0x00UL)
#define RCC_GPIOA_P4_OUT 0x100UL

// первой строкой обращаемся к GPIOA регистру, отвечающему за конфигацию типа выхода, второй - устанавливаем двухтактный выход
#define RCC_GPIOA_OTYPER *(uint32_t *)(0x40020000UL + 0x04UL)
#define RCC_GPIOA_OTYPER_P4_PP 0x0UL

// первой строкой обращаемся к GPIOA регистру, отвечающему за скорость работы пинов, второй - устанавливаем среднюю скорость работы
#define RCC_GPIOA_OSPEED *(uint32_t *)(0x40020000UL + 0x08UL)
#define RCC_GPIOA_OSPEED_P4_MED 0x100UL

// первой строкой обращаемся к GPIOA регистру, отвечающему за подтяжку или стяжку, второй - отключаем подтяжку и стяжку
#define RCC_GPIOA_PUPDR *(uint32_t *)(0x40020000UL + 0x0CUL)
#define RCC_GPIOA_PUPDR_P4_RST 0x0UL

// первой строкой обращаемся к GPIOA регистру управления сигналом на портах, второй - устанавливаем высокий уровень напряжения на порте 4,
// третьей - низкий
#define RCC_GPIOA_BSRR *(uint32_t *)(0x40020000UL + 0x18UL)
#define P4_HIGH 0x10UL
#define P4_LOW 0x100000UL

#define SET_BIT(REG, BIT) (REG |= BIT)

void GPIO_INIT(void);
uint8_t time_skeeper(void);
uint8_t diode_mode(uint8_t pusher, uint8_t but_check);
void main_process(uint8_t but_check);