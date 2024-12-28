#include "init.h"
#include "it_handlers.h"
#include <stdbool.h>

// ------------------- 全局变量 -------------------
// 1ms 计数
volatile uint32_t g_systickCount = 0;

// 当前工作模式：1=模式1(对灯)，2=模式2(单独闪烁)
volatile uint8_t  g_workMode = 1;  

// 模式1频率索引(0..2) => 0=>0.5Hz,1=>1.7Hz,2=>2.2Hz
volatile uint8_t  g_mode1FreqIndex = 0;

// 模式2下，每个LED的频率索引(0..2=已激活，或可用-1表示未激活)
volatile uint8_t  g_currentLedIndex[6] = {255,255,255,255,255,255}; 
// 也可用 -1 表示“尚未激活过”，此处 255 仅是演示

// 模式2下，当前选中的LED编号(0..5)
volatile uint8_t  g_selectedLed = 0;


// ------------------- 对灯切换/独立闪烁用的一些参数 -------------------

// 模式1的三档周期(毫秒)
// 例如 0.5Hz => 2000ms 切换下一对
//      1.7Hz => ~588ms
//      2.2Hz => ~455ms (四舍五入)
static const uint16_t mode1_periods[3] = {2000, 588, 455};

// 当前对灯编号(0..2 => 分别表示(3&4),(2&5),(1&6))
static uint8_t   currentPairIndex = 0;
static uint32_t  lastMode1SwitchTime = 0;

// 模式2的三档半周期(或整周期) => 为了闪烁，每过此ms就 toggle
// 频率0.3Hz => 周期=3333ms => toggle一次=3333ms
// 频率0.8Hz => 周期=1250ms
// 频率1.4Hz => 周期= ~714ms
// 也可根据需要用半周期或其他写法
static const uint16_t mode2_periods[3] = {3333, 1250, 714};

// 记录每个LED的“上次toggle时刻”
static uint32_t lastToggleTime[6] = {0,0,0,0,0,0};

// ------------------- 函数声明 -------------------
static void mode1_process(void);
static void mode2_process(void);

int main(void)
{
    // 1. 硬件初始化
    RCC_Ini();
    GPIO_Ini();
    EXTI_ITR_Ini();
    SysTick_Init();

    // 开机默认是模式1, freq=最慢(0.5Hz)
    g_mode1FreqIndex = 0; 
    // 确保“对灯”从 (3&4) 开始
    currentPairIndex = 0;
    lastMode1SwitchTime = g_systickCount;

    // 主循环
    while (1)
    {
        if (g_workMode == 1)
        {
            mode1_process();
        }
        else
        {
            mode2_process();
        }
    }
    // 不会到达这里
    // return 0;
}

// ------------------- 模式1逻辑：对灯切换 -------------------
static void mode1_process(void)
{
    // 计算本模式下的切换周期
    uint16_t period = mode1_periods[g_mode1FreqIndex];

    // 判断是否到切换时间
    uint32_t now = g_systickCount;
    if ((now - lastMode1SwitchTime) >= period)
    {
        lastMode1SwitchTime = now;

        // 切换到下一对
        currentPairIndex = (currentPairIndex + 1) % 3;
    }

    // 根据 currentPairIndex 点亮相应对灯，其余熄灭
    // 三对: pair0=(3,4), pair1=(2,5), pair2=(1,6)
    // LED编号与IO对应关系，请您根据实际修改
    // 假设: LED#1=>PB14, #2=>PB7, #3=>PB0, #4=>PB2, #5=>PB6, #6=>PB1
    // 则 (3,4) => PB0 & PB2, (2,5) => PB7 & PB6, (1,6)=>PB14 & PB1
    // 这里可以直接写“把所有LED关掉，然后打开需要的那两个”

    // 先关全部
    GPIOB->BSRR = ( LED1_PIN | LED2_PIN | LED3_PIN |
                    LED4_PIN | LED5_PIN | LED6_PIN ) << 16;

    switch (currentPairIndex)
    {
    case 0: // (3&4)
        // 对应 LED3_PIN, LED4_PIN
        GPIOB->BSRR = LED3_PIN | LED4_PIN;
        break;
    case 1: // (2&5)
        GPIOB->BSRR = LED2_PIN | LED5_PIN;
        break;
    case 2: // (1&6)
        GPIOB->BSRR = LED1_PIN | LED6_PIN;
        break;
    default:
        break;
    }
}

// ------------------- 模式2逻辑：多LED独立闪烁 -------------------
static void mode2_process(void)
{
    uint32_t now = g_systickCount;

    // 依次检查6个LED
    for (uint8_t i = 0; i < 6; i++)
    {
        // 查看该LED的freqIndex(0..2=>有效, 255或-1=>未激活)
        if (g_currentLedIndex[i] <= 2)
        {
            // 该LED已被激活
            uint16_t period = mode2_periods[g_currentLedIndex[i]];
            // 若到期 => toggle
            if ((now - lastToggleTime[i]) >= period)
            {
                lastToggleTime[i] = now;
                // 翻转: 使用 ODR 方式
                // LED i => PB? => 需与您实际硬件对应
                // 下例假设 i=0=>PB14, i=1=>PB7, i=2=>PB0, ...
                // 为简化，这里直接示例：通过 ODR ^=
                // 您也可用 BSRR 方式

                uint32_t odrMask = 0;
                switch(i)
                {
                case 0: odrMask = GPIO_ODR_OD14; break;
                case 1: odrMask = GPIO_ODR_OD7;  break;
                case 2: odrMask = GPIO_ODR_OD0;  break;
                case 3: odrMask = GPIO_ODR_OD2;  break;
                case 4: odrMask = GPIO_ODR_OD6;  break;
                case 5: odrMask = GPIO_ODR_OD1;  break;
                default: break;
                }
                GPIOB->ODR ^= odrMask; 
            }
        }
        else
        {
            // 未激活 => 保持熄灭
            // 如果您想让未激活LED始终熄灭，可在此手动清零
            // 但只要前面没设置，应该已经是灭的状态
        }
    }

    // 其余逻辑：若需要在模式2下也轮询某种操作，可写在此
}