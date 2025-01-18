// leds.h
#ifndef LEDS_H
#define LEDS_H

#include "init.h"
#include "globals.h"

// 按钮点击处理函数
void handleButton1Press(void);
void handleButton2Press(void);
void handleButton3Press(void);

// LED状态处理函数
void leds_state1(void);
void leds_state2(void);
void leds_state_down(void);

#endif // LEDS_H
