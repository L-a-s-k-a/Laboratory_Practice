#include "init.h"

uint8_t button1_pressed = 0; // 按钮1状态变量
uint8_t button2_pressed = 0; // 按钮2状态变量
uint8_t button3_pressed = 0; // 按钮3状态变量
// uint8_t mode1 = 0; // 按钮1的工作模式，0: 开启LED，1: 关闭LED
// uint8_t mode2 = 0; // 按钮2的工作模式，0: 开启LED，1: 关闭LED
// uint8_t mode3 = 0; // 按钮2的工作模式，0: 开启LED，1: 关闭LED

#define READ_BIT(REG, BIT)   ((REG) & (BIT))
#define SET_BIT(REG, BIT)    ((REG) |= (BIT))
#define RESET_BIT(REG, BIT)  ((REG) &= ~(BIT))


// 按钮标志（用于外部分析）
uint8_t button1_flag = 0;
uint8_t button2_flag = 0;
uint8_t button3_flag = 0;

//LED状态变量（0：灭，1：亮）
uint8_t led_1_state = 0;    // 红色LED状态
uint8_t led_2_state = 0;   // 蓝色LED状态
uint8_t led_3_state = 0; // 黄色LED状态
uint8_t led_4_state = 0; // 外部LED状态

#define DEBOUNCE_DELAY 200 // 防抖延迟时间，单位为毫秒

int main(void) {
    GPIO_Ini_1(); // 初始化GPIO端口
    GPIO_Ini_2(); // 初始化GPIO端口

    while (1) {
        // 检查按钮1状态（PC12）
        button1_flag =! button1_pressed;
        button2_flag =! button2_pressed;
        button3_flag =! button3_pressed;
        // led_1_state =! mode1;
        // led_2_state =! mode2;
        // led_3_state =! mode3;
        // led_4_state =! mode3;
        if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0)) { // 按钮1按下时，PC12 为 0
            delay(DEBOUNCE_DELAY); // 防抖延迟
            if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) {
                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET);
                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET); // 点亮 LED红色
                led_1_state = 1;  // 设置GPIOC_IDR_PIN6为输入模式
                button1_pressed = 1; // 更新按钮状态为已按下
                button2_pressed = 0;
            }
        } else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) == 0) {
                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_RESET); // 熄灭 LED红色
                led_1_state = 0;  // 设置GPIOC_IDR_PIN6为输入模式
                button1_pressed = 0; // 重置按钮状态
            }        
         // 检查按钮2状态（PC13）
        if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0)) { // 按钮2按下时，PC13 为 0
            delay(DEBOUNCE_DELAY); // 防抖延迟
            if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0) {
                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); // 点亮 LED黄色
                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_RESET);
                led_2_state = 1; // 切换模式                
                button2_pressed = 1;
                button1_pressed = 0; // 更新按钮状态为已按下                
            }
        } else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) {
                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET); // 熄灭 LED黄色
                led_2_state = 0; // 切换模式
                button2_pressed = 0; // 重置按钮状态
        }       
        // 检查按钮3状态（PC13）
        if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN6) != 0) && READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) { // 按钮3按下时，PC6 为 0
            delay(DEBOUNCE_DELAY); // 防抖延迟              
                    SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); // 点亮LED3蓝色
                    
                button3_pressed = 1; // 更新按钮状态为已按下
                button1_pressed = 1;
                led_3_state = 1;
                }else {                   
                    SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET); 
                    led_3_state = 0;
                    button3_pressed = 0;
                }                 

        if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN6) != 0) && READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0) { // 按钮3按下时，PC6 为 0
            delay(DEBOUNCE_DELAY); // 防抖延迟              
                    SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN8_SET); // 点亮LED4
                    
                    button3_pressed = 1;
                    button2_pressed = 1;
                    led_4_state = 1;
                }else{                    
                    SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN8_RESET);
                    led_4_state = 0;
                    button3_pressed = 0;
                } 
        delay(200);
    }
}