// #include "../Inc/init.h"
// #include "../Inc/it_handlers.h"

// // 定义一些全局变量
// uint16_t DelayTickCount = 0;                 // 延迟计数器
// uint32_t GlobalTickBut1Wait = 0;             // 按钮1的等待时间计数器
// uint32_t GlobalTickBut2Wait = 0;             // 按钮2的等待时间计数器
// uint16_t CurrentLedFrequency = 0;            // 当前LED频率

// // LED配置：每个LED的对应引脚
// const uint8_t Led_Off_set[6] = {0U, 7U, 14U, 8U, 9U, 10U};

// // 当前的状态，初始化为6
// uint8_t Current_state = 6;
// // 当前激活的LED索引
// uint8_t Current_led = 0;
// // 计数器，用于状态循环
// uint8_t Numb = 0;
// // 按钮1和按钮2的计数器
// uint8_t counterbut1 = 0, counterbut2 = 0;
// // 按钮1和按钮2的状态标志
// uint8_t flagbut1 = 0, flagbut2 = 0;
// // 按钮1和按钮2的长按标志
// uint8_t flagbut1long = 0, flagbut2long = 0;

// // LED的频率范围配置：每个频率对应一个LED的频率值
// uint16_t led_freq_range[3][3] = {{1250, 384, 250},{833, 312, 227},{625, 263, 200}};
// // 当前LED的频率：每个LED有2个频率，分别表示两个按键的频率
// uint8_t Current_freq[6][2] = {{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0}};
// // 每个LED的负载频率
// uint16_t Led_load[6] = {0, 0, 0, 0, 0, 0};
// // 每个LED的计数器
// uint16_t Led_count[6] = {0, 0, 0, 0, 0, 0};
// // LED的开关状态标志数组，0表示关闭，1表示开启
// uint8_t Ledflag[2][6] = {{1, 1, 1, 1, 1, 1},{0, 0, 0, 0, 0, 0}};
// // LED1和LED2的开关标志
// uint8_t Led1flag = 0;
// uint8_t Led2flag = 0;

// // LED初始化函数声明
// void LedVal_Init();
// // 控制LED灯亮灭的函数声明
// void Led_light();

// int main(void) 
// { 
//     // 初始化GPIO、时钟、外部中断、SysTick等
//     GPIO_Init();
//     RCC_Init();      // 系统时钟初始化
//     EXTI_ITR_Init(); // 外部中断初始化
//     SysTick_Init();  // SysTick定时器初始化
//     LedVal_Init();   // LED相关值初始化

//     while (1) 
//     {
//         // 遍历6个LED，检查LED计数器是否超过LED负载值，超出则切换LED的状态
//         for (uint8_t i = 0; i < 6; i++){
//             // 如果LED计数器超过负载值，复位计数器并切换LED状态
//             if (Led_count[i] >= Led_load[i])
//             {
//                 Led_count[i] = 0; // 复位LED计数器
//                 if (Ledflag[1][i] == 1)
//                 {         
//                     Ledflag[1][i] = 0; // 关闭LED
//                 }
//                 else
//                 {
//                     Ledflag[1][i] = 1; // 打开LED
//                 }
//                 // 更新LED1和LED2的状态标志
//                 Led1flag = Ledflag[1][0];
//                 Led2flag = Ledflag[1][1];
//             }
//         }

//         // 计算当前状态对应的LED数量
//         Numb = Current_state % 7;
//         // 如果Numb不为0，则点亮对应数量的LED
//         if (Numb != 0)
//         {
//             for (uint8_t i = 0; i < Numb; i++){
//                 Ledflag[0][i] = 1;  // 打开LED
//             }
//         }
//         else
//         {
//             // 否则，关闭所有LED，并重置当前状态
//             for (uint8_t i = 0; i < 6; i++)
//             {
//                 Ledflag[0][i] = 0;  // 关闭LED
//             }
//             Current_state = 0; // 重置当前状态
//         }

//         // 处理按钮1长按事件：如果按钮1长按，修改当前LED的频率
//         if(flagbut1long == 1){ 
//             // 如果频率已达到最大值3，则重置为0
//             if (Current_freq[Current_led][0] >= 3)
//             {
//                 Current_freq[Current_led][0] = 0;
//             }
//             // 根据当前频率值设置LED的负载频率
//             Led_load[Current_led % 6] = led_freq_range[Current_freq[Current_led][1]][Current_freq[Current_led][0]];
//             flagbut1long = 0; // 重置长按标志
//         } 

//         // 处理按钮2长按事件：如果按钮2长按，修改当前LED的频率
//         if(flagbut2long == 1){     
//             // 如果频率已达到最大值3，则重置为0
//             if (Current_freq[Current_led][1] >= 3)
//             {
//                 Current_freq[Current_led][1] = 0;
//             }
//             // 根据当前频率值设置LED的负载频率
//             Led_load[Current_led % 6] = led_freq_range[Current_freq[Current_led][1]][Current_freq[Current_led][0]];
//             flagbut2long = 0; // 重置长按标志
//         }

//         // 检测按钮1短按：如果按钮1短按并且等待时间超过50ms，则重置按钮状态
//         if(flagbut1 == 1 && READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) == 0  && GlobalTickBut1Wait >= 50){ 
//             flagbut1 = 0;
//         }  

//         // 检测按钮2短按：如果按钮2短按并且等待时间超过50ms，则重置按钮状态
//         if(flagbut2 == 1 && READ_BIT(GPIOC->IDR, GPIO_IDR_ID10) == 0 && GlobalTickBut2Wait >= 50){ 
//             flagbut2 = 0;
//         }  

//         // 更新LED的状态
//         Led_light();
//     } 
// }

// // 控制LED灯亮灭的函数
// void Led_light()
// {
//     uint32_t ODR_clear = 0x0UL;  // 存储要清除的位
//     uint32_t ODR_set = 0x0UL;    // 存储要设置的位

//     // 遍历6个LED，根据它们的开关状态来决定是点亮还是熄灭
//     for (uint8_t i = 0; i < 6; i++){
//         if ((Ledflag[0][i] == 1) && (Ledflag[1][i] == 1)){
//             ODR_set = ODR_set + (0x1UL << (Led_Off_set[i]));  // 点亮LED
//         }
//         else{
//             ODR_clear = ODR_clear + (0x1UL << (Led_Off_set[i])); // 熄灭LED
//         }
//     }

//     // 修改GPIOB的输出数据寄存器（ODR），根据LED状态设置对应引脚的电平
//     MODIFY_REG(GPIOB->ODR, ODR_clear, ODR_set);
// }

// // 初始化LED的频率值
// void LedVal_Init()
// {
//     // 初始化每个LED的负载频率，并设置其计数器为负载频率值
//     for (uint8_t i = 0; i < 6; i++){
//         Led_load[i] = led_freq_range[Current_freq[i][0]][Current_freq[i][1]];
//         Led_count[i] = Led_load[i];  // 初始化计数器为负载频率
//     }
// }


#include <stdint.h> 
#include "../Inc/init.h"

volatile uint8_t led_states[4] = {0, 0, 0, 0}; // 记录每个 LED 的状态
volatile uint8_t button_count = 0;            // 按钮按下次数
volatile uint8_t button_flag = 0;

int main(void) {
    uint8_t button_pressed = 0; // 按钮状态
    GPIO_Ini();                 // 初始化 GPIO

    while (1) {
        // 更新按钮状态
        button_flag = READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) ? 1 : 0;

        // 检测按钮是否按下
        if (READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) != 0) { // 按钮按下
            for (volatile int i = 0; i < 100000; i++); // 简单延时，用于消抖
            if (READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) == 0 && button_pressed == 0) {
                button_pressed = 1; // 标记按钮已经按下
                button_count++;     // 增加按钮按下次数

                // 根据按钮按下次数控制灯
                if (button_count >= 1 && button_count <= 4) {
                    // 点亮指定数量的 LED
                    for (uint8_t i = 0; i < 4; i++) {
                        if (i < button_count) {
                            ToggleLED(i + 1, 1); // 打开 LED
                            led_states[i] = 1;  // 更新状态
                        } else {
                            ToggleLED(i + 1, 0); // 关闭 LED
                            led_states[i] = 0;  // 更新状态
                        }
                    }
                } else if (button_count == 5) {
                    // 关闭所有 LED
                    TurnOffAllLEDs();
                    for (uint8_t i = 0; i < 4; i++) {
                        led_states[i] = 0; // 更新状态
                    }
                } else if (button_count == 6) {
                    // 点亮所有 LED
                    TurnOnAllLEDs();
                    for (uint8_t i = 0; i < 4; i++) {
                        led_states[i] = 1; // 更新状态
                    }
                } else if (button_count > 6) {
                    // 重置逻辑并重新开始
                    TurnOffAllLEDs();
                    for (uint8_t i = 0; i < 4; i++) {
                        led_states[i] = 0; // 更新状态
                    }
                    button_count = 0; // 重置计数
                }
            }
        } else {
            button_pressed = 0; // 按钮释放
        }
    }
}