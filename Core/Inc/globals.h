// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

// 全局变量声明
extern uint16_t ledFrequencies[2][3];
extern uint16_t ledStateData[3][6];
extern uint8_t button1Clicks;
extern uint8_t button2Clicks;
extern uint8_t button3Clicks;
extern uint16_t globalTick;
extern uint16_t interruptTick1;
extern uint16_t interruptTick2;
extern uint16_t interruptTick3;
extern uint16_t flickerInterval;
extern uint16_t buttonDelay;
extern int8_t ledIndex1;
extern int8_t ledIndex2;
extern int8_t ledIndex3;
extern uint8_t isFlagUp2;
extern uint8_t isState2Flag;
extern uint8_t currentStateCheck;
extern uint8_t previousStateCheck;

#endif // GLOBALS_H
