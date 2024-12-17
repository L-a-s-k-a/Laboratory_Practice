#include "init.h" 

int main(void) 
{ 
    GPIO_Ini(); // 配置GPIO
    
    int current_led = 0;
    int button_press_count = 0;

    while (1) {
        if (!(GPIOA_IDR & BUTTON1_PIN)) { // 检测按钮按下（低电平）
            button_press_count++;

            if (button_press_count % 6 == 0) {
                // 每第六次按压点亮所有LED
                LED1_ON();
                LED2_ON();
                LED3_ON();
                LED4_ON();
            } else {
                // 按压次数小于6次，依次点亮LED
                switch (current_led) {
                    case 0:
                        LED1_ON();
                        LED2_OFF();
                        LED3_OFF();
                        LED4_OFF();
                        current_led = 1;
                        break;
                    case 1:
                        LED1_OFF();
                        LED2_ON();
                        LED3_OFF();
                        LED4_OFF();
                        current_led = 2;
                        break;
                    case 2:
                        LED1_OFF();
                        LED2_OFF();
                        LED3_ON();
                        LED4_OFF();
                        current_led = 3;
                        break;
                    case 3:
                        LED1_OFF();
                        LED2_OFF();
                        LED3_OFF();
                        LED4_ON();
                        current_led = 0;
                        break;
                }
            }

            // 等待按钮释放
            while (!(GPIOA_IDR & BUTTON1_PIN)); 
        }
    }
}
