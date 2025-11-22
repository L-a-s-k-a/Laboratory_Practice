#include "init.h"

// 全局变量 
volatile uint8_t current_mode = 0;          // 当前PC13按钮的功能模式 (0:单灯, 1:双灯, 2:三灯)
volatile uint8_t led_state = 0;             // 当前LED状态 (0:绿, 1:蓝, 2:红)
volatile uint8_t three_led_state = 0;       // 三灯模式下的状态 (0:全亮, 1:全灭)
volatile uint8_t pc13_button_state = 0;     // PC13按钮的实时状态 (0:释放, 1:按下)
volatile uint8_t pg3_button_state = 0;      // PG3按钮的实时状态 (0:释放, 1:按下)
volatile uint8_t current_led_pattern = 0;   // 当前LED模式 (位0:绿, 位1:蓝, 位2:红)
volatile uint8_t button1_press_count = 0;   // PC13按钮按下次数统计
volatile uint8_t button2_press_count = 0;   // PG3按钮按下次数统计

// 简单延时函数
void Delay(uint32_t delay_ms)
{
    for(volatile uint32_t i = 0; i < delay_ms * 5000; i++);
}

// PC13按钮检测函数（按下时为高电平）
uint8_t IsButton1Pressed(void)
{
    // 更新实时状态
    pc13_button_state = ((GPIOC->IDR & GPIO_PIN_13) != 0) ? 1 : 0;
    
    if(pc13_button_state)  // 按下时为高电平
    {
        Delay(50);  // 去抖延时
        pc13_button_state = ((GPIOC->IDR & GPIO_PIN_13) != 0) ? 1 : 0;
        if(pc13_button_state)
        {
            // 等待释放
            while(((GPIOC->IDR & GPIO_PIN_13) != 0))
            {
                pc13_button_state = 1; // 保持按下状态
                Delay(10);
            }
            pc13_button_state = 0; // 更新为释放状态
            button1_press_count++; // 增加按下计数
            return 1;
        }
    }
    return 0;
}

// PG3按钮检测函数（按下时为低电平）
uint8_t IsButton2Pressed(void)
{
    // 更新实时状态
    pg3_button_state = ((GPIOG->IDR & GPIO_PIN_3) == 0) ? 1 : 0;
    
    if(pg3_button_state)  // 按下时为低电平
    {
        Delay(50);  // 去抖延时
        pg3_button_state = ((GPIOG->IDR & GPIO_PIN_3) == 0) ? 1 : 0;
        if(pg3_button_state)
        {
            // 等待释放
            while(((GPIOG->IDR & GPIO_PIN_3) == 0))
            {
                pg3_button_state = 1; // 保持按下状态
                Delay(10);
            }
            pg3_button_state = 0; // 更新为释放状态
            button2_press_count++; // 增加按下计数
            return 1;
        }
    }
    return 0;
}

// 根据模式和状态控制LED
void UpdateLEDs(void)
{
    uint8_t led_pattern = 0;
    
    switch(current_mode)
    {
        case 0: // 模式0：单灯模式
            switch(led_state)
            {
                case 0: led_pattern = 0x01; break; // 绿
                case 1: led_pattern = 0x02; break; // 蓝
                case 2: led_pattern = 0x04; break; // 红
            }
            break;
            
        case 1: // 模式1：双灯模式
            switch(led_state)
            {
                case 0: led_pattern = 0x01 | 0x02; break; // 绿+蓝
                case 1: led_pattern = 0x02 | 0x04; break; // 蓝+红
                case 2: led_pattern = 0x04 | 0x01; break; // 红+绿
            }
            break;
            
        case 2: // 模式2：三灯模式
            if(three_led_state == 0)
            {
                led_pattern = 0x07; // 所有灯都亮
            }
            else
            {
                led_pattern = 0x00; // 所有灯都灭
            }
            break;
    }
    
    current_led_pattern = led_pattern; // 更新全局变量用于监控
    LED_Control(led_pattern);
}

// 模式指示函数
void ModeIndicator(void)
{
    uint8_t original_pattern = current_led_pattern;
    
    // 根据模式闪烁
    switch(current_mode)
    {
        case 0: // 单灯模式：闪烁绿色
            LED_Control(0x01);
            Delay(150);
            LED_Control(0x00);
            Delay(150);
            LED_Control(0x01);
            break;
            
        case 1: // 双灯模式：闪烁蓝色
            LED_Control(0x02);
            Delay(150);
            LED_Control(0x00);
            Delay(150);
            LED_Control(0x02);
            break;
            
        case 2: // 三灯模式：闪烁红色
            LED_Control(0x04);
            Delay(150);
            LED_Control(0x00);
            Delay(150);
            LED_Control(0x04);
            break;
    }
    
    Delay(150);
    LED_Control(original_pattern); // 恢复原始状态
    current_led_pattern = original_pattern; // 更新监控变量
}

// 实时更新按钮状态（用于监控）
void UpdateButtonStates(void)
{
    // PC13按钮状态（按下时为高电平）
    pc13_button_state = ((GPIOC->IDR & GPIO_PIN_13) != 0) ? 1 : 0;
    
    // PG3按钮状态（按下时为低电平）
    pg3_button_state = ((GPIOG->IDR & GPIO_PIN_3) == 0) ? 1 : 0;
}

int main(void)
{
    // 系统时钟配置
    SystemClock_Config();
    
    // GPIO初始化
    GPIO_Init();
    
    // 初始化全局变量
    current_mode = 0;
    led_state = 0;
    three_led_state = 0;
    pc13_button_state = 0;
    pg3_button_state = 0;
    current_led_pattern = 0x01; // 初始为绿灯
    button1_press_count = 0;
    button2_press_count = 0;
    
    // 初始LED状态
    UpdateLEDs();
    
    // 启动指示：显示当前模式
    ModeIndicator();
    
    while(1)
    {
        // 实时更新按钮状态
        UpdateButtonStates();
        
        // 检查按钮1（PC13）- 根据当前模式切换LED状态
        if(IsButton1Pressed())
        {
            // 根据当前模式处理按钮按下
            switch(current_mode)
            {
                case 0: // 单灯模式：循环切换单个LED
                case 1: // 双灯模式：循环切换两个LED组合
                    led_state = (led_state + 1) % 3;
                    break;
                    
                case 2: // 三灯模式：在全亮和全灭之间切换
                    three_led_state = (three_led_state + 1) % 2;
                    break;
            }
            
            UpdateLEDs();
        }
        
        // 检查按钮2（PG3）- 切换PC13按钮的功能模式
        if(IsButton2Pressed())
        {
            current_mode = (current_mode + 1) % 3; // 循环0,1,2
            
            // 重置LED状态到初始位置
            led_state = 0;
            three_led_state = 0; // 三灯模式初始为全亮
            
            // 更新LED显示
            UpdateLEDs();
            
            // 显示模式指示
            ModeIndicator();
        }
        
        Delay(10);  // 主循环延时
    }
}