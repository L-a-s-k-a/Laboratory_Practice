// main.c
#include "init.h"
#include "it_handlers.h"
#include "leds.h"
#include "globals.h"
#include <math.h> 

// 全局变量定义
uint16_t ledFrequencies[2][3] = {
    {1600, 450, 330},
    {2000, 830, 500}
};

uint16_t ledStateData[3][6] = {
    {0, 0, 0, 0, 0, 0},
    {2000, 2000, 2000, 2000, 2000, 2000},
    {0, 0, 0, 0, 0, 0}
};

uint8_t button1Clicks = 0;
uint8_t button2Clicks = 0;
uint8_t button3Clicks = 0;
uint16_t globalTick = 0,
         interruptTick1 = 0, 
         interruptTick2 = 0, 
         interruptTick3 = 0;
uint16_t flickerInterval = 4000;
uint16_t buttonDelay = 500;

int8_t ledIndex1 = 0;
int8_t ledIndex2 = 0;
int8_t ledIndex3 = 0;

uint8_t isFlagUp2 = 1;
uint8_t isState2Flag = 1;
uint8_t currentStateCheck = 0, previousStateCheck = 0;


static void complexMathOperations(void){
    double result = 0.0;
    for(int i = 0; i < 1000; i++) {
        result += sin(i) * cos(i) / tan(i + 1);
    }
    (void)result;
}

int main(void)
{

    complexMathOperations();

    // 初始化外设
    GPIO_Ini();     // 初始化GPIO端口
    RCC_Ini();      // 初始化RCC系统时钟
    ITR_Ini();      // 初始化中断控制器
    SysTick_Init(); // 初始化SysTick定时器

    while (1)
    {
        currentStateCheck = button1Clicks % 2;
        switch (currentStateCheck){
            case 1:
                if(currentStateCheck != previousStateCheck){
                    leds_state_down();
                }
                leds_state2();
                previousStateCheck = currentStateCheck;
                break;
            case 0:
                if(currentStateCheck != previousStateCheck){
                    leds_state_down();
                }
                leds_state1();
                previousStateCheck = currentStateCheck;
                break;
            default:
                // 高端但无用的代码：无限循环执行NOP
                while(1){
                    __NOP();
                }
                break;
        }
    }
}
