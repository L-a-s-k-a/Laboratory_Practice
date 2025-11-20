#include <stdint.h>
#include "../Inc/init.h"

// 全局变量
volatile uint8_t led_count = 1;     // 同时点亮的LED数量（1-6）
volatile uint8_t current_step = 0;  // 当前步骤
volatile uint8_t btn1_state = 0;    // 按钮1前一个状态
volatile uint8_t btn2_state = 1;    // 按钮2前一个状态
volatile uint8_t led2_blink_flag = 0; // LED2闪烁标志


void Delay(uint32_t time)
{
    for(uint32_t i = 0; i < time; i++)
    {
        for(uint32_t j = 0; j < 1000; j++);
    }
}


void LED2_Blink_Task(void)
{
    static uint32_t blink_timer = 0;
    static uint8_t blink_state = 0;
    
    blink_timer++;
    if(blink_timer >= 500)  // 500ms闪烁
    {
        blink_timer = 0;
        if(led2_blink_flag)
        {
            if(blink_state)
            {
                LED2_ON();
            }
            else
            {
                LED2_OFF();
            }
            blink_state = !blink_state;
        }
    }
}


void Handle_BTN1_Press(void)
{
    led_count++;
    if(led_count > 6) 
    {
        led_count = 1;
    }
    
    // 重置当前步骤
    current_step = 0;
    
    // 使用宏命令控制LED2闪烁指示设置变化
    led2_blink_flag = 1;
    
    // 显示当前设置的数量（闪烁显示）
    for(int i = 0; i < 3; i++)
    {
        // 根据设置的数量点亮对应的LED
        if(led_count <= 3)
        {
            LED_Write((1 << led_count) - 1);
            Ext_LED_Write(0);
        }
        else
        {
            LED_Write(0x07);  // 全亮板载LED
            Ext_LED_Write((1 << (led_count - 3)) - 1);
        }
        Delay(200);
        LED_Write(0);
        Ext_LED_Write(0);
        Delay(200);
    }
    
    // 停止LED2闪烁
    led2_blink_flag = 0;
    LED2_OFF();
}


void Handle_BTN2_Press(void)
{
    // 使用宏命令控制LED2在按下时短暂亮起
    LED2_ON();
    Delay(50);
    LED2_OFF();
    
    if(led_count == 6)
    {
        // 数量为6时，全亮/全灭切换
        if(current_step == 0)
        {
            LED_Write(0x07);      // 全亮板载LED
            Ext_LED_Write(0x07);  // 全亮外接LED
            current_step = 1;
        }
        else
        {
            LED_Write(0x00);      // 全灭板载LED
            Ext_LED_Write(0x00);  // 全灭外接LED
            current_step = 0;
        }
    }
    else if(led_count == 5)
    {
        // 数量为5时：亮1-5 → 全亮 → 全灭
        switch(current_step)
        {
            case 0:
                // 第一组：点亮LED1-5
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x03);  // 亮外接LED1-2 (PC10, PC11)
                current_step = 1;
                break;
                
            case 1:
                // 第二组：点亮所有LED (1-6)
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x07);  // 全亮外接LED
                current_step = 2;
                break;
                
            case 2:
                // 第三组：全部熄灭并重置
                LED_Write(0x00);
                Ext_LED_Write(0x00);
                current_step = 0;
                break;
        }
    }
    else if(led_count == 4)
    {
        // 数量为4时：亮1-4 → 全亮 → 全灭
        switch(current_step)
        {
            case 0:
                // 第一组：点亮LED1-4
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x01);  // 亮外接LED1 (PC10)
                current_step = 1;
                break;
                
            case 1:
                // 第二组：点亮所有LED (1-6)
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x07);  // 全亮外接LED
                current_step = 2;
                break;
                
            case 2:
                // 第三组：全部熄灭并重置
                LED_Write(0x00);
                Ext_LED_Write(0x00);
                current_step = 0;
                break;
        }
    }
    else if(led_count == 3)
    {
        // 数量为3时：亮1-3 → 亮1-6 → 全灭
        switch(current_step)
        {
            case 0:
                // 第一组：点亮LED1-3
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x00);  // 外接LED全灭
                current_step = 1;
                break;
                
            case 1:
                // 第二组：点亮所有LED (1-6)
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x07);  // 全亮外接LED
                current_step = 2;
                break;
                
            case 2:
                // 第三组：全部熄灭并重置
                LED_Write(0x00);
                Ext_LED_Write(0x00);
                current_step = 0;
                break;
        }
    }
    else if(led_count == 2)
    {
        // 数量为2时：亮1-2 → 亮1-4 → 亮1-6 → 全灭
        switch(current_step)
        {
            case 0:
                // 第一组：点亮LED1-2
                LED_Write(0x03);      // 亮LED1-2
                Ext_LED_Write(0x00);  // 外接LED全灭
                current_step = 1;
                break;
                
            case 1:
                // 第二组：点亮LED1-4
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x01);  // 亮外接LED1 (PC10)
                current_step = 2;
                break;
                
            case 2:
                // 第三组：点亮所有LED (1-6)
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x07);  // 全亮外接LED
                current_step = 3;
                break;
                
            case 3:
                // 第四组：全部熄灭并重置
                LED_Write(0x00);
                Ext_LED_Write(0x00);
                current_step = 0;
                break;
        }
    }
    else if(led_count == 1)
    {
        // 数量为1时：亮1 → 亮1-2 → 亮1-3 → 亮1-4 → 亮1-5 → 亮1-6 → 全灭
        switch(current_step)
        {
            case 0:
                // 第一组：点亮LED1
                LED_Write(0x01);      // 亮LED1
                Ext_LED_Write(0x00);  // 外接LED全灭
                current_step = 1;
                break;
                
            case 1:
                // 第二组：点亮LED1-2
                LED_Write(0x03);      // 亮LED1-2
                Ext_LED_Write(0x00);  // 外接LED全灭
                current_step = 2;
                break;
                
            case 2:
                // 第三组：点亮LED1-3
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x00);  // 外接LED全灭
                current_step = 3;
                break;
                
            case 3:
                // 第四组：点亮LED1-4
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x01);  // 亮外接LED1 (PC10)
                current_step = 4;
                break;
                
            case 4:
                // 第五组：点亮LED1-5
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x03);  // 亮外接LED1-2 (PC10, PC11)
                current_step = 5;
                break;
                
            case 5:
                // 第六组：点亮所有LED (1-6)
                LED_Write(0x07);      // 全亮板载LED
                Ext_LED_Write(0x07);  // 全亮外接LED
                current_step = 6;
                break;
                
            case 6:
                // 第七组：全部熄灭并重置
                LED_Write(0x00);
                Ext_LED_Write(0x00);
                current_step = 0;
                break;
        }
    }
}


int main(void)
{
    // 系统初始化
    SystemClock_Config();
    GPIO_Init();
    
    // 使用宏命令显示初始设置
    LED2_ON();  // 开启LED2
    Delay(500);
    LED2_OFF(); // 关闭LED2
    Delay(500);
    LED2_ON();
    Delay(500);
    LED2_OFF();
    
    // 显示初始设置的数量（1个LED）
    LED_Write(0x01);
    Delay(1000);
    LED_Write(0x00);
    
    while(1)
    {
        uint8_t current_btn1 = Read_BTN1();
        uint8_t current_btn2 = Read_BTN2();
        
        // LED2闪烁任务
        LED2_Blink_Task();
        
        // 检测按钮1的下降沿（按下）
        if(btn1_state && !current_btn1)
        {
            Delay(10);  // 简单消抖
            if(!Read_BTN1())
            {
                Handle_BTN1_Press();
            }
        }
        
        // 检测按钮2的下降沿（按下）
        if(btn2_state && !current_btn2)
        {
            Delay(10);  // 简单消抖
            if(!Read_BTN2())
            {
                Handle_BTN2_Press();
            }
        }
        
        // 更新按钮状态
        btn1_state = current_btn1;
        btn2_state = current_btn2;
        
        Delay(1);
    }
}