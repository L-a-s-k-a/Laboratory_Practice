// int main(void){
//     while(1){}

// }

#include "stm32f7xx.h"
#include "main.h"
#include "system_stm32f7xx.h"
#define LED_MASK ((1U << LED1_PIN) | \
                  (1U << LED2_PIN) | \
                  (1U << LED3_PIN))
static volatile uint32_t delay_counter;

static void delay_ms(uint32_t milliseconds)
{
    while (milliseconds != 0U)
    {
        delay_counter = SystemCoreClock / 8000U;

        while (delay_counter != 0U)
        {
            delay_counter--;
        }

        milliseconds--;
    }
}

static void gpio_init(void)
{
    // Тактирование GPIOB и GPIOC
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN |
                    RCC_AHB1ENR_GPIOCEN;

    // PB0, PB7, PB14 - выходы
    GPIOB->MODER &= ~(
        (3U << (LED1_PIN * 2U)) |
        (3U << (LED2_PIN * 2U)) |
        (3U << (LED3_PIN * 2U))
    );

    GPIOB->MODER |= (
        (1U << (LED1_PIN * 2U)) |
        (1U << (LED2_PIN * 2U)) |
        (1U << (LED3_PIN * 2U))
    );

    // Push-pull, low speed, без подтяжки
    GPIOB->OTYPER &= ~LED_MASK;
    GPIOB->OSPEEDR &= ~(
        (3U << (LED1_PIN * 2U)) |
        (3U << (LED2_PIN * 2U)) |
        (3U << (LED3_PIN * 2U))
    );

    GPIOB->PUPDR &= ~(
        (3U << (LED1_PIN * 2U)) |
        (3U << (LED2_PIN * 2U)) |
        (3U << (LED3_PIN * 2U))
    );

    // PC13 и PC0 - входы
    GPIOC->MODER &= ~(
        (3U << (BUTTON1_PIN * 2U)) |
        (3U << (BUTTON2_PIN * 2U))
    );

    // B1 is active-high on PC13; use a pull-down.
    GPIOC->PUPDR &= ~(
        (3U << (BUTTON1_PIN * 2U)) |
        (3U << (BUTTON2_PIN * 2U))
    );

    GPIOC->PUPDR |= (2U << (BUTTON1_PIN * 2U)) |
                    (1U << (BUTTON2_PIN * 2U));

    // Все светодиоды выключены
    GPIOB->BSRR = LED_MASK << 16U;
}

static void leds_set(uint8_t first_led, uint8_t led_count)
{
    uint32_t output_mask = 0U;

    for (uint8_t i = 0U; i < led_count; i++)
    {
        uint8_t led_number = (uint8_t)((first_led + i) % 3U);

        if (led_number == 0U)
        {
            output_mask |= (1U << LED1_PIN);
        }
        else if (led_number == 1U)
        {
            output_mask |= (1U << LED2_PIN);
        }
        else
        {
            output_mask |= (1U << LED3_PIN);
        }
    }

    // Сначала выключить все LED
    GPIOB->BSRR = LED_MASK << 16U;

    // Затем включить нужные
    GPIOB->BSRR = output_mask;
}

static uint8_t button_pressed(uint32_t pin, uint8_t active_high)
{
    uint32_t pin_state = GPIOC->IDR & (1U << pin);
    uint8_t pressed = active_high != 0U ? pin_state != 0U : pin_state == 0U;

    if (pressed != 0U)
    {
        delay_ms(30U);

        pin_state = GPIOC->IDR & (1U << pin);
        pressed = active_high != 0U ? pin_state != 0U : pin_state == 0U;

        if (pressed != 0U)
        {
            do
            {
                pin_state = GPIOC->IDR & (1U << pin);
                pressed = active_high != 0U ? pin_state != 0U : pin_state == 0U;
            } while (pressed != 0U);

            delay_ms(30U);
            return 1U;
        }
    }

    return 0U;
}

int main(void)
{
    uint8_t first_led = 0U;
    uint8_t led_count = 1U;

    gpio_init();
    leds_set(first_led, led_count);

    while (1)
    {
        // B1: select the next LED; the previous selection is switched off.
        if (button_pressed(BUTTON1_PIN, 1U) != 0U)
        {
            first_led = (uint8_t)((first_led + 1U) % 3U);
            leds_set(first_led, led_count);
        }

        // B2/external button on PC0: change the number of simultaneously lit LEDs.
        if (button_pressed(BUTTON2_PIN, 0U) != 0U)
        {
            led_count++;

            if (led_count > 3U)
            {
                led_count = 1U;
            }

            leds_set(first_led, led_count);
        }
    }
}