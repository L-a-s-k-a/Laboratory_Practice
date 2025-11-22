#include "stm32f429xx.h"
#include "init.h"
#include "init.c"
// 全局变量
volatile uint8_t current_led = 0;
volatile uint8_t function_swapped = 0;
volatile uint8_t btn1_pressed = 0;
volatile uint8_t btn2_pressed = 0;
volatile uint8_t btn3_pressed = 0;

// 延时函数
void delay(uint32_t count)
{
    for(volatile uint32_t i = 0; i < count; i++);
}

// 顺序打开LED
void Sequence_On(void)
{
    if(current_led < 6) {
        current_led++;
        LED_On(current_led);
    }
}

// 顺序关闭LED  
void Sequence_Off(void)
{
    if(current_led > 0) {
        LED_Off(current_led);
        current_led--;
    }
}

int main(void)
{
    SystemClock_Config();
    GPIO_Init();
    All_LEDs_Off();
    
    // 启动指示
    for(uint8_t i = 1; i <= 6; i++) LED_On(i);
    delay(300000);
    for(uint8_t i = 1; i <= 6; i++) LED_Off(i);
    delay(300000);
    
    while(1) {
        // 读取按钮状态
        static uint32_t last_check = 0;
        static uint32_t tick_count = 0;
        
        tick_count++;
        
        // 消抖时间为1000（约1ms）
        if(tick_count - last_check > 1000) {
            last_check = tick_count;
            
            uint8_t btn1_current = Button_Read(1);
            uint8_t btn2_current = Button_Read(2);
            uint8_t btn3_current = Button_Read(3);
            
            // 检测按钮1按下（上升沿）
            if(!btn1_pressed && btn1_current) {
                if(!function_swapped) {
                    Sequence_On();  // 正常模式：顺序打开
                } else {
                    Sequence_Off(); // 交换模式：顺序关闭
                }
            }
            btn1_pressed = btn1_current;
            
            // 检测按钮2按下
            if(!btn2_pressed && btn2_current) {
                if(!function_swapped) {
                    Sequence_Off(); // 正常模式：顺序关闭
                } else {
                    Sequence_On();  // 交换模式：顺序打开
                }
            }
            btn2_pressed = btn2_current;
            
            // 检测按钮3按下（功能交换）
            if(!btn3_pressed && btn3_current) {
                function_swapped = !function_swapped;
                
                // 使用视觉反馈
                for(int i = 0; i < 2; i++) { 
                    for(uint8_t j = 1; j <= 6; j++) LED_On(j);
                    delay(100000);  // 减少延时
                    for(uint8_t j = 1; j <= 6; j++) LED_Off(j);
                    delay(100000);  // 减少延时
                }
                
                // 恢复之前的LED状态
                for(uint8_t i = 1; i <= current_led; i++) {
                    LED_On(i);
                }
            }
            btn3_pressed = btn3_current;
        }
        
        delay(100); 
    }
}