#include "stm32f4xx.h"
#include "init.h"
#include <stdint.h>

// 手动定义并初始化
uint32_t SystemCoreClock = 16000000U;

//-------------------------- 全局变量定义 --------------------------//
volatile uint32_t GlobalTickCount = 0;         // SysTick 累计
volatile uint32_t ExternInterruptTickCount = 0; // 用于简单防抖

volatile uint8_t  CurrentMode = 1;  // 默认上电后 => 模式1

//-------------------------------------------------
// 模式1 (对灯) 相关
//-------------------------------------------------
#define NUM_MODE1_FREQ 3
// 0.5Hz => 2000ms, 1.7Hz => ~588ms, 2.2Hz => ~455ms
static const uint32_t Mode1Periods[NUM_MODE1_FREQ] = {2000, 588, 455};
volatile uint8_t  M1FreqIndex = 0;            // 对灯频率档 (0..2)
volatile uint8_t  M1Step = 0;                 // 0..2 => (3&4),(2&5),(1&6)
volatile uint32_t M1NextToggleTime = 0;       // 下次切换时刻

//-------------------------------------------------
// 模式2 (多灯独立闪烁) 相关
//-------------------------------------------------
#define NUM_MODE2_FREQ 3
// 0.3Hz => 3333ms, 0.8Hz => 1250ms, 1.4Hz => 714ms
static const uint32_t Mode2Periods[NUM_MODE2_FREQ] = {3333, 1250, 714};

// 每个 LED 是否激活闪烁
volatile uint8_t  LedActive[6] = {0,0,0,0,0,0};  
// 每个 LED 的频率档位
volatile uint8_t  LedFreqIndex[6] = {0,0,0,0,0,0};
// 记录下次该 LED 进行「翻转」的时刻
static uint32_t   LedNextToggleTime[6] = {0,0,0,0,0,0};
// 当前 LED 的开/关状态 (0=灭,1=亮)
static uint8_t    LedState[6] = {0,0,0,0,0,0};

// 当前选中的 LED (0..5 => 对应“1~6号灯”)
volatile uint8_t  M2SelectedLed = 0; 

//-------------------------------------------------
// LED 与引脚的对应关系 (0..5)
// 0->PB0, 1->PB1, 2->PB2, 3->PB6, 4->PB7, 5->PB14
//-------------------------------------------------
static const uint8_t LED_PIN[6] = {0,7,14,6,2,1};

//----------------- 函数声明 -----------------//
static void SetLed(uint8_t ledIndex, uint8_t on);
void Mode2_SetLedActive(uint8_t ledIndex, uint8_t freqIndex);
void Mode2_DeactivateAll(void);

//-------------------------------------------------
// main()
//-------------------------------------------------
int main(void)
{
    // 1. 初始化系统时钟
    SystemClock_Config();

    // 2. 初始化 SysTick => 1ms一次中断 (假设16MHz, 则 SysTick->LOAD=16000-1)
    SysTick_Init(SystemCoreClock / 1000);

    // 3. 初始化所有 GPIO
    GPIO_Init_All();

    // 4. 初始化 EXTI (3 个按键)
    EXTI_Init_All();

    // 5. 上电默认模式 1, 对灯频率设为最慢(0.5Hz => M1FreqIndex=0)
    M1FreqIndex = 0;
    M1Step = 0;
    M1NextToggleTime = 0; // 立即在主循环里进行第一次切换

    while(1)
    {
        if(CurrentMode == 1)
        {
            //--------- 模式1：对灯循环 ---------
            // 如果到了切换时间
            if(GlobalTickCount >= M1NextToggleTime)
            {
                // 更新下次切换时刻
                M1NextToggleTime = GlobalTickCount + Mode1Periods[M1FreqIndex];

                // M1Step 依次在 0..2 之间循环
                M1Step = (M1Step + 1) % 3;
                
                // 先关掉所有 LED
                for(uint8_t i=0; i<6; i++)
                {
                    SetLed(i, 0);
                }

                // 根据 M1Step 开对应对儿
                // 题目给的三对是 (3&4), (2&5), (1&6)
                // 但要注意：你文档里写的“(3&4)”指第三第四颗LED，
                // 这里在数组里：LED2是PB2, LED3是PB6, LED4是PB7, LED5是PB14... 
                // 有些混淆。此处的写法仅示例，可根据你实际 "编号" 对应关系做调整。
                // 我这里假设：LED0=第1个灯, LED1=第2个灯, LED2=第3个灯, LED3=第4个灯, LED4=第5个灯, LED5=第6个灯
                // 那么 "对" (3&4) => LED2,LED3; (2&5) => LED1,LED4; (1&6) => LED0,LED5
                switch(M1Step)
                {
                    case 0: // (3&4)
                        SetLed(2, 1); 
                        SetLed(3, 1);
                        break;
                    case 1: // (2&5)
                        SetLed(1, 1);
                        SetLed(4, 1);
                        break;
                    case 2: // (1&6)
                        SetLed(0, 1);
                        SetLed(5, 1);
                        break;
                }
            }
        }
        else
        {
            //--------- 模式2：多LED独立闪烁 ---------
            // 逐个LED检查是否激活
            for(uint8_t i=0; i<6; i++)
            {
                if(LedActive[i] == 1)
                {
                    // 看是否到了该翻转的时刻
                    uint32_t period = Mode2Periods[ LedFreqIndex[i] ];
                    if(GlobalTickCount >= LedNextToggleTime[i])
                    {
                        // 翻转
                        if(LedState[i] == 0)
                        {
                            SetLed(i, 1);
                            LedState[i] = 1;
                        }
                        else
                        {
                            SetLed(i, 0);
                            LedState[i] = 0;
                        }

                        // 下次翻转时刻
                        LedNextToggleTime[i] = GlobalTickCount + period/2;
                    }
                }
            }
        }

        // ...可在此做其它任务
    }
}

//-------------------------------------------------
// 设置某个 LED 亮或灭
// ledIndex: 0..5
// on: 0=灭, 1=亮
//-------------------------------------------------
static void SetLed(uint8_t ledIndex, uint8_t on)
{
    // PBx 引脚输出 1 => 实际上是“拉高”或“点亮”?
    // 需根据你板子的 LED 接法决定。这里假设 "1=亮, 0=灭"。
    uint16_t pinMask = 1 << LED_PIN[ledIndex];
    if(on)
    {
        // 置1
        GPIOB->BSRR = pinMask;  
    }
    else
    {
        // 复位
        GPIOB->BSRR = (pinMask << 16);
    }
}

//-------------------------------------------------
// 供 EXTI 中断调用：激活某个 LED 的闪烁 (模式2)
// freqIndex=0 => 0.3Hz, 1=>0.8Hz, 2=>1.4Hz
//-------------------------------------------------
void Mode2_SetLedActive(uint8_t ledIndex, uint8_t freqIndex)
{
    LedActive[ledIndex] = 1;
    LedFreqIndex[ledIndex] = freqIndex;
    // 初始化翻转状态 => 先熄灭
    LedState[ledIndex] = 0;
    SetLed(ledIndex, 0);
    // 马上安排一次翻转 (让它 period/2 后亮起来)
    uint32_t period = Mode2Periods[freqIndex];
    LedNextToggleTime[ledIndex] = GlobalTickCount + period/2;
}

//-------------------------------------------------
// 供 EXTI/切模式时调用：取消所有 LED 闪烁并全灭
//-------------------------------------------------
void Mode2_DeactivateAll(void)
{
    for(uint8_t i=0; i<6; i++)
    {
        LedActive[i] = 0;
        LedState[i] = 0;
        SetLed(i, 0);
    }
}