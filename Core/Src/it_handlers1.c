#include "it_handlers1.h"

volatile uint32_t SysTickMs = 0;
volatile uint8_t BtnEvent = BTN_EVENT_NONE;

static volatile uint8_t BtnEdgeFlag = 0;
static volatile uint32_t BtnEdgeTime = 0;

static uint8_t BtnState = 0;
static uint8_t BtnHold4S_Triggered = 0;
static uint32_t BtnPressTime = 0;

/* Длительность последнего нажатия для STMViewer. */
volatile uint32_t Debug_Btn_HoldDuration = 0;

void EXTI15_10_IRQHandler(void)
{
    if (READ_BIT(EXTI->PR, EXTI_PR_PR13)) {
        SET_BIT(EXTI->PR, EXTI_PR_PR13);
        BtnEdgeTime = SysTickMs;
        BtnEdgeFlag = 1;
    }
}

void SysTick_Handler(void)
{
    SysTickMs++;

    if (BtnEdgeFlag && (SysTickMs - BtnEdgeTime >= DEBOUNCE_MS)) {
        BtnEdgeFlag = 0;

        uint8_t level = READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) ? 1U : 0U;

        if (level != BtnState) {
            BtnState = level;

            if (level) {
                BtnPressTime = SysTickMs;
                BtnHold4S_Triggered = 0;
            } else {
                uint32_t duration = SysTickMs - BtnPressTime;
                Debug_Btn_HoldDuration = duration;

                if (!BtnHold4S_Triggered) {
                    BtnEvent = (duration >= HOLD_2S_MS)
                             ? BTN_EVENT_HOLD_2S
                             : BTN_EVENT_SHORT;
                }
            }
        }
    }

    if (BtnState && !BtnHold4S_Triggered &&
        SysTickMs - BtnPressTime >= HOLD_4S_MS) {
        BtnEvent = BTN_EVENT_HOLD_4S;
        BtnHold4S_Triggered = 1;
    }
}
