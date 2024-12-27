
#include <init.h>
#include "init.c"
#include <it_handlers.h>
#include "it_handlers.c"

volatile uint8_t work_mode = 1; // 1=闪烁，0=常亮
volatile uint32_t button2_timer = 0; // 按钮2按住计时
uint8_t flag_led = 0; // 当前点亮的LED索引
uint8_t flagbutton1,flagbutton2;
uint8_t frequency_led = 0; // （0=0.2Hz, 1=0.8Hz, 2=1.3Hz）
uint8_t button2_state = 0; // 0=未按下，1=按下中，2=长按



uint32_t systick_counter = 0; // 计数器
const uint32_t blink_periods[] = {5000, 1250, 769}; // 对应 0.2Hz、0.8Hz、1.3Hz 1/0.2=5c 
const uint32_t led_pins[] = {GPIO_ODR_OD14, GPIO_ODR_OD7, GPIO_ODR_OD0, GPIO_ODR_OD2, GPIO_ODR_OD6, GPIO_ODR_OD1};

int main(void) {
    GPIO_Ini(); 
    RCC_Ini(); 
    EXTI_ITR_Ini(); 
    SysTick_Init(); 

    while (1) {
        flagbutton1=READ_BIT(GPIOC->IDR,GPIO_IDR_ID13) != 0;
        flagbutton2=READ_BIT(GPIOC->IDR,GPIO_IDR_ID6) != 0;
        if (READ_BIT(GPIOC->IDR,GPIO_IDR_ID6) != 0) { // 按钮按下
            if (button2_state == 0) { // 首次检测到按下
                button2_state = 1;  // 切换到按下中状态
                button2_timer = systick_counter; // 记录按下的时刻
            } else if (button2_state == 1) { // 按下中
                uint32_t press_duration = systick_counter - button2_timer;
                if (press_duration >= 2000) { // 检测到长按
                    work_mode ^= 1;        // 切换工作模式
                    button2_state = 2;     // 标记为长按已处理
                }
            }
        } else { // 按钮释放
            if (button2_state == 1) { // 按下后释放，未达到长按时间
                frequency_led = (frequency_led + 1) % 3; // 切换频率
            }
            // 重置按钮状态
            button2_state = 0;
        }

        // LED 控制逻辑
        static uint32_t blink_counter = 0;
        if (work_mode == 0) { // 闪烁模式
            if (systick_counter - blink_counter >= blink_periods[frequency_led]) {
                blink_counter = systick_counter;
                GPIOB->ODR ^= led_pins[flag_led]; // 翻转当前 LED
            }
        } else { // 常亮模式
        if (flag_led==2)
        {
           continue;
        }
        update_led_state(); // 点亮当前 LED
        }
    }
}
