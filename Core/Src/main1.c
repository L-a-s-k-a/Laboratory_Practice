#include "init1.h"
#include "it_handlers1.h"

#define MODE_BLINK  0U
#define MODE_STEADY 1U
#define FREQ_COUNT  3U

static const uint16_t BlinkHalfPeriodMs[FREQ_COUNT] = {
    1667U, 333U, 227U
};

/* Переменные для наблюдения в STMViewer. */
volatile uint8_t Debug_LedPhase = 0;
volatile uint16_t Debug_HalfPeriod = 0;
volatile uint8_t Debug_Mode = MODE_BLINK;
volatile uint8_t Debug_LedIndex = 0;

static void Leds_Update(uint8_t ledIndex, uint8_t ledPhase)
{
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        if (i == ledIndex && ledPhase) {
            Led_On(i);
        } else {
            Led_Off(i);
        }
    }
}

int main(void)
{
    uint8_t ledIndex = 0;
    uint8_t freqIndex = 0;
    uint8_t mode = MODE_BLINK;
    uint8_t ledPhase = 1;
    uint32_t lastToggle;

    RCC_Ini();
    GPIO_Ini();
    EXTI_ITR_Ini();
    SysTick_Init();

    lastToggle = SysTickMs;
    Leds_Update(ledIndex, ledPhase);

    while (1) {
        uint8_t event;

        __disable_irq();
        event = BtnEvent;
        BtnEvent = BTN_EVENT_NONE;
        __enable_irq();

        switch (event) {
        case BTN_EVENT_SHORT:
            ledIndex = (uint8_t)((ledIndex + 1U) % LED_COUNT);
            ledPhase = 1;
            lastToggle = SysTickMs;
            break;

        case BTN_EVENT_HOLD_2S:
            if (mode == MODE_BLINK) {
                freqIndex = (uint8_t)((freqIndex + 1U) % FREQ_COUNT);
                ledPhase = 1;
                lastToggle = SysTickMs;
            }
            break;

        case BTN_EVENT_HOLD_4S:
            mode = (mode == MODE_BLINK) ? MODE_STEADY : MODE_BLINK;
            ledPhase = 1;
            lastToggle = SysTickMs;
            break;

        default:
            break;
        }

        if (mode == MODE_BLINK) {
            uint16_t halfPeriod = BlinkHalfPeriodMs[freqIndex];

            if (SysTickMs - lastToggle >= halfPeriod) {
                lastToggle += halfPeriod;
                ledPhase = !ledPhase;
            }

            Debug_HalfPeriod = halfPeriod;
        } else {
            ledPhase = 1;
            Debug_HalfPeriod = 0;
        }

        Leds_Update(ledIndex, ledPhase);

        Debug_LedPhase = ledPhase;
        Debug_Mode = mode;
        Debug_LedIndex = ledIndex;
    }
}
