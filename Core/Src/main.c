#include "init.h" 
 
int main(void) 
{ 
    GPIO_Config(); // 配置GPIO
    
    int current_led = 0;
    int led_state = 0; // 用于跟踪第二个按钮的状态

    while (1) {
        if (!(GPIOA_IDR & BUTTON1_PIN)) { // 检测按钮1按下（低电平）
            switch (current_led) {
                case 0:
                    LED1_ON();
                    LED2_OFF();
                    LED3_OFF();
                    current_led = 1;
                    break;
                case 1:
                    LED1_OFF();
                    LED2_ON();
                    LED3_OFF();
                    current_led = 2;
                    break;
                case 2:
                    LED1_OFF();
                    LED2_OFF();
                    LED3_ON();
                    current_led = 0;
                    break;
            }
            while (!(GPIOA_IDR & BUTTON1_PIN)); // 等待按钮释放
        }

        if (!(GPIOA_IDR & BUTTON2_PIN)) { // 检测按钮2按下（低电平）
            if(led_state == 0) { 
                if (!(GPIOB_ODR & LED1_PIN)) {
                    LED1_ON(); 
                }
                if (!(GPIOB_ODR & LED2_PIN)) {
                    LED2_ON(); 
                }
                if (!(GPIOB_ODR & LED3_PIN)) {
                    LED3_ON(); 
                }
                led_state = 1; // 更新状态为打开LED
            } else {
                LED1_OFF();
                LED2_OFF();
                LED3_OFF();
                led_state = 0; // 更新状态为关闭LED
            }
            while (!(GPIOA_IDR & BUTTON2_PIN)); // 等待按钮释放
        }
    }
}