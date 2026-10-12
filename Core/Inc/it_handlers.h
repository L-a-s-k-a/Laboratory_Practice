#define IT_HANDLERS_H
#ifndef IT_HANDLERS_H

#include "init.h"
#include <stdint.h>

extern volatile uint32_t GlobalTickCount;

void SysTick_Handler(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void DWT_Init(void);
void GPIO_Init(void);
void INIT_SVETODIOD();
void INIT_UPRAVLENI_CCW(GPIO_TypeDef *port_ccw, uint8_t n2);
void INIT_UPRAVLENI_CW( GPIO_TypeDef *port_cw ,uint8_t n1);
void SET_SVETODIOD(char svet);
void RESET_SVETODIOD(char svet);
void MoveCW(GPIO_TypeDef *port_cw, uint8_t n, uint32_t steps, uint32_t min_time);
void MoveCCW(GPIO_TypeDef *port_ccw, uint8_t n, uint32_t steps, uint32_t min_time );
void POVOROT_UGL(GPIO_TypeDef *port_ccw, uint8_t n, uint16_t ugol);
void OSTANOVKA(GPIO_TypeDef *port_ccw, uint8_t n);
#endif