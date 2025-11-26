#include "../Inc/init.h"
extern volatile uint32_t systemTickCount;
extern volatile uint32_t holdTime;
extern volatile uint8_t shortState;
extern volatile uint8_t mode;
extern volatile uint8_t flag;
extern volatile uint16_t adc;


void EXTI3_IRQHandler(void);
void SysTick_Handler(void);
void ADC_IRQHandler(void);