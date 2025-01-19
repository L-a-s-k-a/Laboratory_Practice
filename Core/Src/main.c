#include "../../CMSIS/Devices/Inc/stm32f4xx.h"
#include "init.h"
#include <stdint.h>

//=== 全局变量 / extern ===
uint8_t LedOn[6] = {0,0,0,0,0,0};
uint8_t NumLedOn = 0;

uint32_t BlinkPeriods[3] = {2500, 909, 526};
uint8_t  FreqIndex = 0;
uint32_t CurrentPeriod = 2500; // 初始=0.4Hz
uint32_t SystemCoreClock = 16000000U;

typedef enum {
    MODE_BLINK=0,
    MODE_CONSTANT
} WorkMode_t;
volatile WorkMode_t CurrentMode = MODE_BLINK;

//=== SysTick 计数 ===
volatile uint32_t GlobalTickCount=0;

//=== 函数声明 ===
static void SetLed(uint8_t index, uint8_t on);
static void ToggleLed(uint8_t index);

int main(void)
{
    // 1. 初始化系统
    SystemClock_Config();
    SysTick_Init(SystemCoreClock/1000); // 1ms
    GPIO_Init_All();
    EXTI_Init_All();

    uint32_t blinkCounter=0;

    while(1)
    {
        if(CurrentMode == MODE_BLINK)
        {
            // 周期翻转
            if(blinkCounter >= (CurrentPeriod/2))
            {
                blinkCounter=0;
                // 对所有 LedOn[i]==1 的灯做翻转
                for(int i=0;i<6;i++){
                    if(LedOn[i]==1) ToggleLed(i);
                }
            }
            else
            {
                blinkCounter++;
                User_Delay(1);
            }
        }
        else
        {
            // 常亮
            for(int i=0;i<6;i++){
                if(LedOn[i]==1) SetLed(i,1);
                else            SetLed(i,0);
            }
            User_Delay(50);
        }
    }
}

//=== 函数实现: SetLed / ToggleLed ===

// 你提到 6个LED分别是 PB0, PB1, PB2, PB6, PB7, PB14 => 需用一个数组
static uint8_t LedPin[6] = {0,1,2,6,7,14};

static void SetLed(uint8_t index, uint8_t on)
{
    // index => 0..5 => PB( LedPin[index] )
    uint8_t pin = LedPin[index];
    if(on)
    {
        GPIOB->BSRR = (1 << pin);  // 置位 => 输出高
    }
    else
    {
        GPIOB->BSRR = (1 << (pin+16)); // 复位 => 输出低
    }
}

static void ToggleLed(uint8_t index)
{
    uint8_t pin = LedPin[index];
    uint32_t odrVal = (GPIOB->ODR >> pin) & 1;
    if(odrVal == 1)
    {
        // 灯目前是亮 => 设置为灭
        GPIOB->BSRR = (1<<(pin+16));
    }
    else
    {
        // 灯灭 => 亮
        GPIOB->BSRR = (1<<pin);
    }
}