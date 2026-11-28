#ifndef IT_HANDLERS1_H
#define IT_HANDLERS1_H

#include "init1.h"

#define DEBOUNCE_MS 30U
#define HOLD_2S_MS  2000U
#define HOLD_4S_MS  4000U

#define BTN_EVENT_NONE    0U
#define BTN_EVENT_SHORT   1U
#define BTN_EVENT_HOLD_2S 2U
#define BTN_EVENT_HOLD_4S 3U

extern volatile uint32_t SysTickMs;
extern volatile uint8_t BtnEvent;

void EXTI15_10_IRQHandler(void);
void SysTick_Handler(void);

#endif
