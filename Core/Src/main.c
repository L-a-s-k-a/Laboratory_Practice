#include <init.h>
#include "it_handlers.h"
#include "leds.h"

// LED 闪烁频率数据
uint16_t LedFreq[2][3] = {{1600, 450, 330}, {2000, 830, 500}};

// LED 状态数据
uint16_t LedState2Data[3][6] = {{0, 0, 0, 0, 0, 0}, {2000, 2000, 2000, 2000, 2000, 2000}, {0, 0, 0, 0, 0, 0}};

// 按钮点击次数
uint8_t b1_clicks = 0;
uint8_t b2_clicks = 0;
uint8_t b3_clicks = 0;

// 全局时间计数
uint16_t GlobalTickCount,
        ExternInterruptTickCount1, 
        ExternInterruptTickCount2, 
        ExternInterruptTickCount3;

// 闪烁周期
uint16_t FLICKER_PERIOD = 4000;
uint16_t delay = 500;

// LED 状态控制变量
int8_t i1 = 0;
int8_t i2 = 0;
int8_t i3 = 0;

// 控制标志
uint8_t flagUp2 = 1;
uint8_t state2_flag = 1;

// 按钮状态检查
uint8_t state_check, p_state_check = 0;

int main(void)
{
        GPIO_Ini();     // 初始化 GPIO
        RCC_Ini();      // 初始化系统时钟
        ITR_Ini();      // 初始化中断
        SysTick_Init(); // 初始化系统定时器

        // 主循环
        while (1)
        {
                state_check = b1_clicks % 2;
                switch (b1_clicks % 2)
                {
                        case 1:
                            if(state_check != p_state_check) leds_state_down();
                            leds_state2();
                            p_state_check = state_check;
                            break;
                        case 0:
                            if(state_check != p_state_check) leds_state_down();
                            leds_state1();
                            p_state_check = state_check;
                            break;
                }
        }
}
