#include "init.h"

static void turnOnLed(uint8_t pin)
{
    // 通过BSRR的置位段(低16位)写1即可
    GPIOB->BSRR = (1 << pin);
}

static void turnOffLed(uint8_t pin)
{
    // 通过BSRR的复位段(高16位)写1即可
    GPIOB->BSRR = (1 << (pin + 16));
}

static uint8_t isButtonPressed(uint8_t pin)
{
    // 按下=0 (因为内部上拉)
    return ( (GPIOC->IDR & (1 << pin)) == 0 );
}

int main(void)
{
    // 初始化时钟、GPIO等
    GPIO_Init();
    
    // 模式变量
    uint8_t mode = 1;  // 默认模式1
    
    while(1)
    {
        // ========== 1. 检测模式切换按键 (PC12) ==========
        if(isButtonPressed(BTN_MODE_PIN))
        {
            // 简易防抖：等待松开
            delay_ms(20);
            if(isButtonPressed(BTN_MODE_PIN))
            {
                // 等按键松开再说
                while(isButtonPressed(BTN_MODE_PIN)) { /*空等*/ }
                
                // 切换模式
                mode++;
                if(mode > 3) mode = 1;
            }
        }
        
        // ========== 2. 根据mode点亮相应的模式指示灯 ==========
        // 先全灭
        turnOffLed(LED_MODE1_PIN);
        turnOffLed(LED_MODE2_PIN);
        turnOffLed(LED_MODE3_PIN);
        
        // 再亮当前
        if(mode == 1) turnOnLed(LED_MODE1_PIN);
        else if(mode == 2) turnOnLed(LED_MODE2_PIN);
        else if(mode == 3) turnOnLed(LED_MODE3_PIN);
        
        // ========== 3. 处理三个“颜色控制”按键 (PC8/9/10) ==========
        // 先把三色LED全灭
        turnOffLed(LED_GREEN_PIN);
        turnOffLed(LED_BLUE_PIN);
        turnOffLed(LED_RED_PIN);
        
        // Button1 => PC8
        if(isButtonPressed(BTN1_PIN))
        {
            switch(mode)
            {
                case 1: turnOnLed(LED_GREEN_PIN); break;
                case 2: turnOnLed(LED_RED_PIN);   break;
                case 3: turnOnLed(LED_BLUE_PIN);  break;
            }
        }
        
        // Button2 => PC9
        if(isButtonPressed(BTN2_PIN))
        {
            switch(mode)
            {
                case 1: turnOnLed(LED_BLUE_PIN);  break;
                case 2: turnOnLed(LED_GREEN_PIN); break;
                case 3: turnOnLed(LED_RED_PIN);   break;
            }
        }
        
        // Button3 => PC10
        if(isButtonPressed(BTN3_PIN))
        {
            switch(mode)
            {
                case 1: turnOnLed(LED_RED_PIN);    break;
                case 2: turnOnLed(LED_BLUE_PIN);   break;
                case 3: turnOnLed(LED_GREEN_PIN);  break;
            }
        }
        
        // 做一点延时，以免轮询过快
        delay_ms(10);
    }
}