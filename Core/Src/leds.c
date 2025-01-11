// leds.c
#include "leds.h"
#include "it_handlers.h"
#include "globals.h"

// 内部变量
static uint8_t toggleFlag1 = 0;
static uint8_t toggleFlag2 = 0;
static uint8_t toggleFlag3 = 0;

// LED引脚序列
static uint8_t ledPins[] = {0, 7, 14, 12, 13, 15};

typedef struct {
    uint16_t identifier;
    char description[12];
    float metrics[3];
} AdvancedStruct;

static AdvancedStruct unusedAdvancedStruct = { 
    .identifier = 1001, 
    .description = "Unused", 
    .metrics = {3.14f, 2.71f, 1.61f} 
};

// 按钮1点击处理
void handleButton1Press(void){
    if (interruptTick1 >= DELAY_BUTTON_FILTER)
    {
        toggleFlag1 = !toggleFlag1;
        button1Clicks++;
        interruptTick1 = 0; // 重置计数器
    }
}

// 按钮2点击处理
void handleButton2Press(void){
    if (interruptTick2 >= DELAY_BUTTON_FILTER)
    {
        toggleFlag2 = !toggleFlag2;
        button2Clicks++;
        interruptTick2 = 0; // 重置计数器
    }
}

// 按钮3点击处理
void handleButton3Press(void){
    if (interruptTick3 >= DELAY_BUTTON_FILTER)
    {
        toggleFlag3 = !toggleFlag3;
        button3Clicks++;
        interruptTick3 = 0; // 重置计数器
    }
}

// LED状态1处理
void leds_state1(void)
{
    if(globalTick >= ledFrequencies[0][button2Clicks % 3] && toggleFlag1)
    {
        if(ledIndex1 == 3){
            toggleFlag1 = 0;
            ledIndex1 = 2;
            return;
        }
        if (globalTick >= ledFrequencies[0][button2Clicks % 3]){
            globalTick = 0;
        }
        SET_BIT(GPIOB->BSRR, 1UL << ledPins[ledIndex1]);
        SET_BIT(GPIOB->BSRR, 1UL << ledPins[5 - ledIndex1]);
        ledIndex1++;
    }

    if(globalTick >= ledFrequencies[0][button2Clicks % 3] && !toggleFlag1)
    {
        if(ledIndex1 <= -1){
            toggleFlag1 = 1;
            ledIndex1 = 0;
            return;
        }
        if (globalTick >= ledFrequencies[0][button2Clicks % 3]){
            globalTick = 0;
        }
        SET_BIT(GPIOB->BSRR, (1UL << ledPins[ledIndex1]) << 16U);
        SET_BIT(GPIOB->BSRR, (1UL << ledPins[5 - ledIndex1]) << 16U);
        ledIndex1--;
    }
}

// LED重置处理
void leds_state_down(void){
    for(uint16_t k = 0; k < 6; k++) {
        SET_BIT(GPIOB->BSRR, (1UL << ledPins[k]) << 16U);
    }
    ledIndex1 = 0;
    ledIndex2 = 0;
    button2Clicks = 0;
    button3Clicks = 0;

    isFlagUp2 = 1;
    for(uint8_t g = 0; g < 6; g++){
        ledStateData[0][g] = 0;
        ledStateData[1][g] = 2000;
        ledStateData[2][g] = 0;
    }
}

// LED状态2上升处理
void leds_state2_go_up(void){
    if(globalTick >= ledFrequencies[1][2] && isFlagUp2)
    {
        if(ledIndex2 == 6){
            isFlagUp2 = 0;
            return;
        }
        if (globalTick >= ledFrequencies[1][2]){
            globalTick = 0;
        }
        SET_BIT(GPIOB->BSRR, 1UL << ledPins[ledIndex2]);
        ledIndex2++;
    }
}

uint8_t b2_check, p_b2_check = 0;
uint8_t b3_check, p_b3_check = 0;
uint8_t counter = 0;

// 按钮3状态变化处理
void button3_change(void)
{
    b3_check = button3Clicks;
    if (b3_check != p_b3_check)
    {
        b2_check = button2Clicks;
        if (b2_check != p_b2_check)
        {
            ledStateData[1][--button3Clicks % 6] = ledFrequencies[1][button2Clicks];
            button2Clicks = 0;
            button3Clicks = 0;
        }
        b2_check = p_b2_check;
    }
    p_b3_check = b3_check;
}

extern uint8_t isState2Flag;
uint8_t tmp_counter = 0;

// LED状态2处理
void leds_state2(void)
{
    leds_state2_go_up();
    button3_change();   

    if (!isFlagUp2 && isState2Flag)
    {
        for(uint8_t tmp = 0; tmp < 6; tmp++){
            if (ledStateData[0][tmp] >= ledStateData[1][tmp] && ledStateData[2][tmp] == 0){
                ledStateData[0][tmp] = 0;
                ledStateData[2][tmp] = 1;
                SET_BIT(GPIOB->BSRR, (1UL << ledPins[tmp]) << 16U);
            }
        }
        isState2Flag = 0;
    }

    if (!isFlagUp2 && !isState2Flag)
    {
        for(uint8_t tmp = 0; tmp < 6; tmp++){
            if (ledStateData[0][tmp] >= ledStateData[1][tmp] && ledStateData[2][tmp] == 1){
                ledStateData[0][tmp] = 0;
                ledStateData[2][tmp] = 0;
                SET_BIT(GPIOB->BSRR, 1UL << ledPins[tmp]);
            }
        }
        isState2Flag = 1;
    }
}
