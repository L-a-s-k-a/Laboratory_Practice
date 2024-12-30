#include "../Inc/init.h"  
#include "../Inc/it_handlers.h"

uint16_t DelayTickCount = 0;
uint32_t GlobalTickBut1Wait = 0;

// 修改频率为0.4Hz、1.1Hz、1.9Hz对应的周期值（单位：毫秒）
uint16_t LedSetHZ[3] = {526, 2500, 909};  // 0.4Hz, 1.1Hz, 1.9Hz 的周期，单位：毫秒
uint8_t LedCurrHZ = 0;  // 用一个变量来表示当前所有LED的频率（0, 1, 2）
uint16_t LedCount[6] = {0, 0, 0, 0, 0, 0};  // LED计数器
uint8_t LedState;  // 默认状态为闪烁模式
uint8_t Ledflag[2][6] = {{1, 1, 1, 1, 1, 1}, {0, 0, 0, 0, 0, 0}}; // 第一行控制LED是否点亮，第二行控制闪烁
uint8_t flagbutton = 0;
uint8_t longflagbutton = 0;
const uint8_t LedOffset[6] = {0U, 7U, 14U, 8U, 9U, 10U};
uint8_t CurrentState = 6;
uint8_t Numb = 0; // 当前系统状态下需要点亮的LED个数
void LedVal_Init(); // 初始化LED频率的函数
void Led_light();   // 控制LED的显示状态

int main(void) 
{ 
    GPIO_Init();
    RCC_Init(); // Initialize system clock
    ITR_Init(); // Initialize interrupts  
    SysTick_Init();
    LedVal_Init();

    while (1) 
    {
        // LED blinking control
        for (uint8_t i = 0; i < 6; i++) {
            if (LedCount[i] >= LedSetHZ[LedCurrHZ]) { // 使用LedSetHZ[LedCurrHZ]作为周期
                LedCount[i] = 0;
                if (Ledflag[1][i] == 1) {
                    Ledflag[1][i] = 0;
                }
                else {
                    Ledflag[1][i] = 1;
                }
            }
        }

        // Handle the LED pattern based on the current state
        Numb = CurrentState % 7;
        if (Numb != 0) {
            for (uint8_t i = 0; i < Numb; i++) {
                Ledflag[0][i] = 1;
            }
        } else {
            for (uint8_t i = 0; i < 6; i++) {
                Ledflag[0][i] = 0;
            }
            CurrentState = 0;
        }

        // Handle long press for frequency change
        if (longflagbutton == 1) { 
            // 当按钮长按时，统一改变所有LED的频率
            LedCurrHZ = (LedCurrHZ + 1) % 3;  // 切换到下一个频率（0, 1, 2）

            longflagbutton = 0; // 清除长按标志
        }

        // Handle short press (button release) debouncing
        if (flagbutton == 1 && READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) == 0 && GlobalTickBut1Wait >= 50) { 
            flagbutton = 0;
        }

        Led_light();
    } 
} 

void Led_light() 
{
    uint32_t ODR_clear = 0x0UL;
    uint32_t ODR_set = 0x0UL;

    if (LedState == 1) {  // 常亮模式
        for (uint8_t i = 0; i < 6; i++) {
            if (Ledflag[0][i] == 1) {  // 如果该LED处于点亮状态
                ODR_set = ODR_set + (0x1UL << LedOffset[i]);  // 点亮该LED
            } else {
                ODR_clear = ODR_clear + (0x1UL << LedOffset[i]); // 熄灭LED
            }
        }
    } else {  // 闪烁模式
        for (uint8_t i = 0; i < 6; i++) {
            if ((Ledflag[0][i] == 1) && (Ledflag[1][i] == 1)) {
                ODR_set = ODR_set + (0x1UL << LedOffset[i]); // 点亮LED
            } else {
                ODR_clear = ODR_clear + (0x1UL << LedOffset[i]); // 熄灭LED
            }
        }
    }
    MODIFY_REG(GPIOB->ODR, ODR_clear, ODR_set);
}

void LedVal_Init() 
{
    // 初始化时，所有LED使用相同的频率周期
    for (uint8_t i = 0; i < 6; i++) {
        LedCount[i] = LedSetHZ[LedCurrHZ];  // 初始化LED计数器
    }
}
