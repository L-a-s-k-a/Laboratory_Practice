#include "stm32f4xx.h"       // 包含 STM32F4 系列的核心与寄存器定义
#include "init.h"           // 包含本项目的初始化函数声明（SystemClock_Config、GPIO_Init_All等）
#include <stdint.h>         // 标准整数类型头文件，如 uint8_t、uint32_t 等

// 手动定义并初始化系统时钟全局变量，用于 SysTick 等频率计算
uint32_t SystemCoreClock = 16000000U; 

//-------------------------- 全局变量定义 --------------------------//
// SysTick 累计计数，每 1ms 加 1，用于延时或周期性事件
volatile uint32_t GlobalTickCount = 0;         

// 外部中断计数器，用于简单防抖等场合（记录距离上次按键的时间）
volatile uint32_t ExternInterruptTickCount = 0; 

// 当前模式：1 表示模式1（对灯循环），2 表示模式2（多灯独立闪烁）
volatile uint8_t  CurrentMode = 1;              // 默认上电后处于模式1

//-------------------------------------------------
// 模式1 (对灯) 相关
//-------------------------------------------------

// 定义可用的对灯闪烁频率个数
#define NUM_MODE1_FREQ 3
// 对应的三种闪烁周期 (单位 ms) => 0.5Hz ~2000ms, 1.7Hz ~588ms, 2.2Hz ~455ms
static const uint32_t Mode1Periods[NUM_MODE1_FREQ] = {2000, 588, 455};

// 当前使用的对灯频率档位 (0..2)
volatile uint8_t  M1FreqIndex = 0;  
// 当前对灯步骤 (0..2)，分别表示 (3&4)、(2&5)、(1&6)
volatile uint8_t  M1Step = 0;       
// 记录下一次进行对灯切换的时间（基于 GlobalTickCount）
volatile uint32_t M1NextToggleTime = 0; 

//-------------------------------------------------
// 模式2 (多灯独立闪烁) 相关
//-------------------------------------------------

// 定义可用的多灯闪烁频率档数
#define NUM_MODE2_FREQ 3
// 对应的三种闪烁周期 (单位 ms)：0.3Hz ~3333ms, 0.8Hz ~1250ms, 1.4Hz ~714ms
static const uint32_t Mode2Periods[NUM_MODE2_FREQ] = {3333, 1250, 714};

// 数组：每个 LED 是否已经被激活进行闪烁 (1=激活, 0=未激活)
volatile uint8_t  LedActive[6] = {0,0,0,0,0,0};  
// 数组：每个 LED 当前使用的频率档位 (0..2)
volatile uint8_t  LedFreqIndex[6] = {0,0,0,0,0,0};
// 数组：每个 LED 下一次翻转（亮↔灭）的时间戳
static uint32_t   LedNextToggleTime[6] = {0,0,0,0,0,0};
// 数组：每个 LED 的当前亮灭状态 (0=灭, 1=亮)
static uint8_t    LedState[6] = {0,0,0,0,0,0};

// 当前选中的 LED 索引 (0..5)，用于模式2下操作“哪个灯”闪烁频率
volatile uint8_t  M2SelectedLed = 0; 

//-------------------------------------------------
// LED 与引脚的对应关系 (0..5)
// 这里数组 LED_PIN[i] 给出第 i 盏灯的物理 PBx 引脚编号
// 例：LED_PIN[0] = 0 => PB0, LED_PIN[1] = 7 => PB7, ...
// 这就让我们在 SetLed 函数里用到正确的位掩码 (1<<pin)
//-------------------------------------------------
static const uint8_t LED_PIN[6] = {0,7,14,6,2,1};

//----------------- 函数声明 -----------------//
// 用于设置指定 LED（索引ledIndex）的亮灭状态
static void SetLed(uint8_t ledIndex, uint8_t on);
// 模式2下，激活指定 LED 并设置频率
void Mode2_SetLedActive(uint8_t ledIndex, uint8_t freqIndex);
// 模式2下，取消所有 LED 闪烁并全部熄灭
void Mode2_DeactivateAll(void);

//-------------------------------------------------
// main() - 主函数
//-------------------------------------------------
int main(void)
{
    // 1. 初始化系统时钟（如设置 HSI=16MHz 或使用 PLL 等）
    SystemClock_Config();

    // 2. 初始化 SysTick，以便 1ms 触发一次中断 (这里假设16MHz)
    SysTick_Init(SystemCoreClock / 1000);

    // 3. 初始化所有 GPIO（包括 LED 引脚为输出、按键引脚为输入等）
    GPIO_Init_All();

    // 4. 初始化 EXTI（配置外部中断，用于检测按键）
    EXTI_Init_All();

    // 5. 上电后默认进入模式1
    //    将对灯频率索引设为 0 => 0.5Hz
    //    M1Step=0 => 对灯序列从(3&4)开始
    //    M1NextToggleTime=0 => 让程序立刻执行一次对灯切换判断
    M1FreqIndex = 0;
    M1Step = 0;
    M1NextToggleTime = 0;

    // 进入无限循环（主循环）
    while(1)
    {
        // 如果当前在模式1 => 对灯循环
        if(CurrentMode == 1)
        {
            // 检查是否到了下一次切换灯的时间
            if(GlobalTickCount >= M1NextToggleTime)
            {
                // 更新下一次切换灯的时间
                M1NextToggleTime = GlobalTickCount + Mode1Periods[M1FreqIndex];

                // 步骤计数往下一个（0->1->2->0循环）
                M1Step = (M1Step + 1) % 3;
                
                // 先把所有 LED 关闭
                for(uint8_t i=0; i<6; i++)
                {
                    SetLed(i, 0);
                }

                // 根据 M1Step 决定哪两盏灯亮
                // (3&4) => LED2,LED3
                // (2&5) => LED1,LED4
                // (1&6) => LED0,LED5
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
            //--------- 模式2：多 LED 独立闪烁 ---------
            // 逐个检查哪些 LED 被激活
            for(uint8_t i=0; i<6; i++)
            {
                // 若该 LED 处于激活状态
                if(LedActive[i] == 1)
                {
                    // 找到该 LED 的闪烁周期
                    uint32_t period = Mode2Periods[ LedFreqIndex[i] ];

                    // 判断是否到了翻转的时间
                    if(GlobalTickCount >= LedNextToggleTime[i])
                    {
                        // 如果当前是灭，就点亮；若是亮，就熄灭
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

                        // 更新下次翻转时间 => 当前时刻 + 半个周期
                        // 这样亮一半时间，灭一半时间，形成完整周期
                        LedNextToggleTime[i] = GlobalTickCount + period/2;
                    }
                }
            }
        }

        // 这里可以加入其它需要在主循环里执行的任务
    }
}

//-------------------------------------------------
// 设置某个 LED（索引 ledIndex）的亮或灭
// on=1 => 亮, on=0 => 灭
//-------------------------------------------------
static void SetLed(uint8_t ledIndex, uint8_t on)
{
    // 获取对应 LED_PIN 数组中的引脚号
    uint16_t pinMask = 1 << LED_PIN[ledIndex];

    // 如果 on=1，就对该引脚置位 (BSRR=pinMask)
    // 否则对该引脚复位 (BSRR=pinMask<<16)
    if(on)
    {
        GPIOB->BSRR = pinMask;  
    }
    else
    {
        GPIOB->BSRR = (pinMask << 16);
    }
}

//-------------------------------------------------
// 模式2下：激活某个 LED 并设置其闪烁频率
// freqIndex=0 => 0.3Hz, 1 => 0.8Hz, 2 => 1.4Hz
//-------------------------------------------------
void Mode2_SetLedActive(uint8_t ledIndex, uint8_t freqIndex)
{
    // 标记该 LED 已激活
    LedActive[ledIndex] = 1;
    // 记录频率档位
    LedFreqIndex[ledIndex] = freqIndex;
    // 初始状态设置为灭
    LedState[ledIndex] = 0;
    // 立即执行一次灭灯操作
    SetLed(ledIndex, 0);

    // 计算下次翻转的时刻 => 当前时间 + 该频率对应周期的一半
    // 这样可以让灯先熄灭一半周期，然后再亮
    uint32_t period = Mode2Periods[freqIndex];
    LedNextToggleTime[ledIndex] = GlobalTickCount + period/2;
}

//-------------------------------------------------
// 模式2下：一次性取消所有 LED 的闪烁并将其熄灭
//-------------------------------------------------
void Mode2_DeactivateAll(void)
{
    // 遍历全部 6 个灯
    for(uint8_t i=0; i<6; i++)
    {
        // 取消激活标记
        LedActive[i] = 0;
        // 当前状态置为 0 (灭)
        LedState[i] = 0;
        // 物理上执行一次灭灯
        SetLed(i, 0);
    }
}