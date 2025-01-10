#include "leds.h"
#include "it_handlers.h"

extern uint16_t FLICKER_PERIOD;    // 闪烁周期
extern uint8_t LedState;           // LED 状态
extern uint8_t b1_clicks;          // 按钮1点击次数
extern uint8_t b2_clicks;          // 按钮2点击次数
extern uint8_t b3_clicks;          // 按钮3点击次数
extern uint16_t delay;             // 延迟时间

// 控制 LED 切换的标志
uint8_t flag1 = 0;
uint8_t flag2 = 0;
uint8_t flag3 = 0;

extern uint16_t GlobalTickCount, ExternInterruptTickCount1, ExternInterruptTickCount2, ExternInterruptTickCount3;  // 外部中断计数和全局计数

// LED 状态数据
extern uint16_t LedFreq[2][3];  // LED 闪烁频率数据

// LED 引脚配置
uint8_t led[] = {0, 7, 14, 12, 13, 15}; // 定义LED引脚
extern uint16_t LedState2Data[3][6];   // LED 状态数据

uint8_t flagUp = 1;     // 控制 LED 状态是否上升
extern uint8_t flagUp2; // 控制 LED 状态是否上升
extern int8_t i1, i2, i3;  // LED 状态控制变量

// 按钮1点击事件
void b1_clicked(){
    if (ExternInterruptTickCount1 >= DELAY_BUTTON_FILTER)  // 检查按钮去抖延迟
    {
        // 交换点击标志，记录点击次数
        if (flag1 == 1){
            flag1 = 0;
            b1_clicks++;
        } else {
            flag1 = 1;
            b1_clicks++;
        }
        ExternInterruptTickCount1 = 0; // 重置计数器
    }
}

// 按钮2点击事件
void b2_clicked(){
    if (ExternInterruptTickCount2 >= DELAY_BUTTON_FILTER)  // 检查按钮去抖延迟
    {
        // 交换点击标志，记录点击次数
        if (flag2 == 1){
            flag2 = 0;
            b2_clicks++;
        } else {
            flag2 = 1;
            b2_clicks++;
        }
        ExternInterruptTickCount2 = 0; // 重置计数器
    }
}

// 按钮3点击事件
void b3_clicked(){
    if (ExternInterruptTickCount3 >= DELAY_BUTTON_FILTER)  // 检查按钮去抖延迟
    {
        // 交换点击标志，记录点击次数
        if (flag3 == 1){
            flag3 = 0;
            b3_clicks++;
        } else {
            flag3 = 1;
            b3_clicks++;
        }
        ExternInterruptTickCount3 = 0; // 重置计数器
    }
}

// 控制 LED 状态为状态1
void leds_state1(void)
{
    // 当全局时间大于设定频率，且 flagUp 为 1 时，LED 按照频率上下切换
    if(GlobalTickCount >= LedFreq[0][b2_clicks % 3] && flagUp)
    {
        if(i1 == 3){
            flagUp = 0;
            i1 = 2;
            return;
        }
        if (GlobalTickCount >= LedFreq[0][b2_clicks % 3]){
            GlobalTickCount = 0;  // 重置全局时间
        }
        for(;;){
            // 点亮 LED，并关闭对称 LED
            SET_BIT(GPIOB->BSRR, 1UL << led[i1]);
            SET_BIT(GPIOB->BSRR, 1UL << led[5 - i1]);
            i1++;
            return;
        }
    }

    // 当 flagUp 为 0 时，LED 按照频率逐渐熄灭
    if(GlobalTickCount >= LedFreq[0][b2_clicks % 3] && !flagUp)
    {
        if(i1 <= -1){
            flagUp = 1;
            i1 = 0;
            return;
        }
        if (GlobalTickCount >= LedFreq[0][b2_clicks % 3]){
            GlobalTickCount = 0;  // 重置全局时间
        }
        for(;;){
            // 熄灭 LED，并关闭对称 LED
            SET_BIT(GPIOB->BSRR, 1UL << led[i1] << 16U);
            SET_BIT(GPIOB->BSRR, 1UL << led[5 - i1] << 16U);
            i1--;
            return;
        }
    }
}

// 关闭所有 LED
void leds_state_down(void){
    for(uint16_t k = 0; k < 6; k++) SET_BIT(GPIOB->BSRR, 1UL << led[k] << 16U);
    i1 = 0;
    i2 = 0;
    b2_clicks = 0;
    b3_clicks = 0;
    flagUp2 = 1;

    // 重置 LED 状态数据
    for(uint8_t g = 0; g < 6; g++){
        LedState2Data[0][g] = 0;
        LedState2Data[1][g] = 2000;
        LedState2Data[2][g] = 0;
    }
}

// 控制 LED 状态为状态2（上升阶段）
void leds_state2_goUp(void){
    if(GlobalTickCount >= LedFreq[1][2] && flagUp2)
    {
        if(i2 == 6){
            flagUp2 = 0;
            return;
        }
        if (GlobalTickCount >= LedFreq[1][2]){
            GlobalTickCount = 0;  // 重置全局时间
        }
        for(;;){
            // 点亮 LED
            SET_BIT(GPIOB->BSRR, 1UL << led[i2]);
            i2++;
            return;
        }
    }
}

// 按钮3状态切换
uint8_t b2_check, p_b2_check = 0;
uint8_t b3_check, p_b3_check = 0;
uint8_t counter = 0;

void b3_change(void)
{
    b3_check = b3_clicks;
    if (b3_check != p_b3_check)
    {
        b2_check = b2_clicks;
        if (b2_check != p_b2_check)
        {
            LedState2Data[1][--b3_clicks % 6] = LedFreq[1][b2_clicks];  // 修改 LED 频率
            b2_clicks = 0;  // 重置按钮2点击次数
            b3_clicks = 0;  // 重置按钮3点击次数
        }
        b2_check = p_b2_check;
    }
    p_b3_check = b3_check;
}

// 控制 LED 状态为状态2（下落阶段）
extern uint8_t state2_flag;
uint8_t tmp_counter = 0;

void leds_state2(void)
{
    leds_state2_goUp();  // 执行 LED 上升阶段
    b3_change();         // 检查按钮3是否被点击并修改 LED 状态

    if (!flagUp2 && state2_flag)
    {
        // 执行 LED 下落阶段
        for(uint8_t tmp = 0; tmp < 6; tmp++){
            if (LedState2Data[0][tmp] >= LedState2Data[1][tmp] && LedState2Data[2][tmp] == 0){
                LedState2Data[0][tmp] = 0;
                LedState2Data[2][tmp] = 1;
                SET_BIT(GPIOB->BSRR, 1UL << led[tmp] << 16U);  // 熄灭 LED
            }
        }
        state2_flag = 0;
    }

    if (!flagUp2 && !state2_flag)
    {
        // 执行 LED 恢复阶段
        for(uint8_t tmp = 0; tmp < 6; tmp++){
            if (LedState2Data[0][tmp] >= LedState2Data[1][tmp] && LedState2Data[2][tmp] == 1){
                LedState2Data[0][tmp] = 0;
                LedState2Data[2][tmp] = 0;
                SET_BIT(GPIOB->BSRR, 1UL << led[tmp]);  // 点亮 LED
            }
        }
        state2_flag = 1;
    }
}
