// #include "../Inc/it_handlers.h"

// // 外部变量声明
// extern uint16_t DelayTickCount;             // 延迟计数器
// extern uint16_t Led_count[6];               // 6个LED的计数器
// extern uint32_t GlobalTickBut1Wait, GlobalTickBut2Wait; // 按钮1和按钮2的等待计数器
// extern uint8_t flagbut1, flagbut2;          // 按钮1和按钮2的状态标志
// extern uint8_t flagbut1long, flagbut2long;  // 按钮1和按钮2的长按标志
// extern uint8_t Current_state;               // 当前状态
// extern uint8_t Current_led;                 // 当前LED编号
// extern uint8_t Current_freq[6][2];          // 存储每个LED的频率

// // 处理 PC6 的外部中断（按键 1）
// void EXTI9_5_IRQHandler(void) 
// {
//     // 判断 PC6 是否被按下（低电平）
//     if(READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) != 0)
//     {
//         // 如果按钮1未被按下，则设置按钮1为按下状态并清零等待计数器
//         if(flagbut1 == 0){
//             flagbut1 = 1;             // 按钮1状态设置为按下
//             GlobalTickBut1Wait = 0;   // 重置按钮1的等待时间
//         }
//     }
//     else{
//         // 如果按钮1被长按超过预定时间，设置长按标志，并增加当前LED的频率
//         if (flagbut1 == 1 && GlobalTickBut1Wait >= LONG_PRESS_TIME)
//         {
//             flagbut1long = 1;                // 设置按钮1为长按状态
//             Current_freq[Current_led][0]++;  // 当前LED的频率+1

//             flagbut1 = 0;                    // 重置按钮1状态
//         }
//         // 如果按钮1被短按，则切换到下一个状态
//         else if (flagbut1 == 1 && GlobalTickBut1Wait >= SHORT_PRESS_TIME)
//         {
//             flagbut1 = 0;            // 重置按钮1状态
//             Current_state++;         // 切换到下一个状态
//         }
//     }
//     // 清除 EXTI 中断标志位，表示中断处理完成
//     SET_BIT(EXTI->PR, EXTI_PR_PR6); 
// }

// // 处理 PC13 的外部中断（按键 2）
// void EXTI15_10_IRQHandler(void) 
// {
//     // 判断 PC13 是否被按下（低电平）
//     if(READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) != 0)
//     {
//         // 如果按钮2未被按下，则设置按钮2为按下状态并清零等待计数器
//         if(flagbut2 == 0){
//             flagbut2 = 1;             // 按钮2状态设置为按下
//             GlobalTickBut2Wait = 0;   // 重置按钮2的等待时间
//         }
//     }
//     else{
//         // 如果按钮2被长按超过预定时间，设置长按标志，并增加当前LED的频率
//         if (flagbut2 == 1 && GlobalTickBut2Wait >= LONG_PRESS_TIME)
//         {
//             flagbut2long = 1;                // 设置按钮2为长按状态
//             Current_freq[Current_led][1]++;  // 当前LED的频率+1
//             flagbut2 = 0;                    // 重置按钮2状态
//         }
//         // 如果按钮2被短按，则切换到下一个LED
//         else if (flagbut2 == 1 && GlobalTickBut2Wait >= SHORT_PRESS_TIME)
//         {
//             flagbut2 = 0;            // 重置按钮2状态
//             Current_led++;           // 切换到下一个LED
//             if (Current_led >= 6)    // 如果当前LED超出范围，则返回第一个LED
//             {
//                 Current_led = 0;
//             }
//         }
//     }
//     // 清除 EXTI 中断标志位，表示中断处理完成
//     SET_BIT(EXTI->PR, EXTI_PR_PR13); 
// }

// // SysTick 中断处理程序，1ms 定时器
// void SysTick_Handler(void) 
// {  
//     DelayTickCount++; // 每次 SysTick 中断时增加延迟计数器
//     // 如果按钮1被按下，则增加按钮1的等待时间
//     if(flagbut1 == 1){
//         GlobalTickBut1Wait++;
//     }
//     // 如果按钮2被按下，则增加按钮2的等待时间
//     if(flagbut2 == 1){
//         GlobalTickBut2Wait++;
//     }
//     // 增加所有LED的计数器
//     for (uint8_t i = 0; i < 6; i++)
//     {
//         Led_count[i]++; // 每个LED计数器加1
//     }
// } 

// // 自定义延时函数，基于 SysTick 计时
// void User_Delay(uint32_t delay)  
// {
//     // 如果延迟计数器超过所需延迟时间，则重置计数器
//     if(DelayTickCount >= delay) DelayTickCount = 0;
//     // 在延迟时间内保持等待
//     while(DelayTickCount < delay){} // 程序通过循环实现延时
// }
