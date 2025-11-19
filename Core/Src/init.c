#include "E:/VSCode-file/Laboratory_Practice/Core/Inc/init.h"

// 全局监测变量定义 - 便于STMViewer监测
SystemMonitor_t g_system_monitor = {
    .current_mode = 1,
    .pg3_press_count = 0,
    .onboard_green_state = 0,
    .onboard_blue_state = 0,
    .onboard_red_state = 0,
    .external_green_state = 0,
    .external_blue_state = 0,
    .external_red_state = 0,
    .pg3_key_state = 1,      // 初始状态：未按下 (内部上拉)
    .pc6_key_state = 1,      // 初始状态：未按下 (内部上拉)
    .pc8_key_state = 1,      // 初始状态：未按下 (内部上拉)
    .pc9_key_state = 1,      // 初始状态：未按下 (内部上拉)
    .pg3_pressed_flag = 0,
    .pc6_pressed_flag = 0,
    .pc8_pressed_flag = 0,
    .pc9_pressed_flag = 0,
    .loop_counter = 0,
    .last_mode_change = 0,
    .pg3_pulse_counter = 0    // 新增：脉冲计数器初始化为0
};

void GPIO_Init_Memory(void) {
    // 启用GPIO时钟 - 直接内存访问 (使用CMSIS定义的RCC_BASE)
    *((volatile uint32_t*)(RCC_BASE + AHB1ENR_OFFSET)) |= (1U << 1) | (1U << 2) | (1U << 6);

    // 配置PG3为输入模式+上拉 - 直接内存访问 (使用CMSIS定义的GPIOG_BASE)
    *((volatile uint32_t*)(GPIOG_BASE + MODER_OFFSET)) &= ~(3U << (KEY_MODE_PIN * 2));
    *((volatile uint32_t*)(GPIOG_BASE + PUPDR_OFFSET)) = 
        (*((volatile uint32_t*)(GPIOG_BASE + PUPDR_OFFSET)) & ~(3U << (KEY_MODE_PIN * 2))) | 
        (1U << (KEY_MODE_PIN * 2));

    // 配置PC6、PC8、PC9为输入模式+上拉 - 直接内存访问 (使用CMSIS定义的GPIOC_BASE)
    *((volatile uint32_t*)(GPIOC_BASE + MODER_OFFSET)) &= 
        ~((3U << (KEY_CTRL1_PIN * 2)) | (3U << (KEY_CTRL2_PIN * 2)) | (3U << (KEY_CTRL3_PIN * 2)));
    
    *((volatile uint32_t*)(GPIOC_BASE + PUPDR_OFFSET)) = 
        (*((volatile uint32_t*)(GPIOC_BASE + PUPDR_OFFSET)) & 
         ~((3U << (KEY_CTRL1_PIN * 2)) | (3U << (KEY_CTRL2_PIN * 2)) | (3U << (KEY_CTRL3_PIN * 2)))) | 
        ((1U << (KEY_CTRL1_PIN * 2)) | (1U << (KEY_CTRL2_PIN * 2)) | (1U << (KEY_CTRL3_PIN * 2)));

    // ====== 配置板载LED引脚 - 直接内存访问 (使用CMSIS定义的GPIOB_BASE) ======
    // 板载绿色LED (PB0)
    *((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) = 
        (*((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) & ~(3U << (ONBOARD_GREEN_PIN * 2))) | 
        (1U << (ONBOARD_GREEN_PIN * 2));
    *((volatile uint32_t*)(GPIOB_BASE + OTYPER_OFFSET)) &= ~(1U << ONBOARD_GREEN_PIN);
    *((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) = 
        (*((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) & ~(3U << (ONBOARD_GREEN_PIN * 2))) | 
        (2U << (ONBOARD_GREEN_PIN * 2));
    *((volatile uint32_t*)(GPIOB_BASE + PUPDR_OFFSET)) &= ~(3U << (ONBOARD_GREEN_PIN * 2));

    // 板载蓝色LED (PB7)
    *((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) = 
        (*((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) & ~(3U << (ONBOARD_BLUE_PIN * 2))) | 
        (1U << (ONBOARD_BLUE_PIN * 2));
    *((volatile uint32_t*)(GPIOB_BASE + OTYPER_OFFSET)) &= ~(1U << ONBOARD_BLUE_PIN);
    *((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) = 
        (*((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) & ~(3U << (ONBOARD_BLUE_PIN * 2))) | 
        (2U << (ONBOARD_BLUE_PIN * 2));
    *((volatile uint32_t*)(GPIOB_BASE + PUPDR_OFFSET)) &= ~(3U << (ONBOARD_BLUE_PIN * 2));

    // 板载红色LED (PB14)
    *((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) = 
        (*((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) & ~(3U << (ONBOARD_RED_PIN * 2))) | 
        (1U << (ONBOARD_RED_PIN * 2));
    *((volatile uint32_t*)(GPIOB_BASE + OTYPER_OFFSET)) &= ~(1U << ONBOARD_RED_PIN);
    *((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) = 
        (*((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) & ~(3U << (ONBOARD_RED_PIN * 2))) | 
        (2U << (ONBOARD_RED_PIN * 2));
    *((volatile uint32_t*)(GPIOB_BASE + PUPDR_OFFSET)) &= ~(3U << (ONBOARD_RED_PIN * 2));

    // ====== 配置外接LED引脚 - 使用纯地址级宏定义 ======
    EXTERNAL_GREEN_MODER_OUT();
    EXTERNAL_GREEN_OTYPER_PP();
    EXTERNAL_GREEN_OSPEEDR_HIGH();
    EXTERNAL_GREEN_PUPDR_NOPULL();

    EXTERNAL_BLUE_MODER_OUT();
    EXTERNAL_BLUE_OTYPER_PP();
    EXTERNAL_BLUE_OSPEEDR_HIGH();
    EXTERNAL_BLUE_PUPDR_NOPULL();

    EXTERNAL_RED_MODER_OUT();
    EXTERNAL_RED_OTYPER_PP();
    EXTERNAL_RED_OSPEEDR_HIGH();
    EXTERNAL_RED_PUPDR_NOPULL();

    // 初始状态：所有LED熄灭
    All_LEDs_Off();
}

// 板载LED控制 - 直接内存访问
static void Onboard_Green_LED_On(void) {
    *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << ONBOARD_GREEN_PIN);
    g_system_monitor.onboard_green_state = 1;
}

static void Onboard_Green_LED_Off(void) {
    *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << (ONBOARD_GREEN_PIN + 16U));
    g_system_monitor.onboard_green_state = 0;
}

static void Onboard_Blue_LED_On(void) {
    *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << ONBOARD_BLUE_PIN);
    g_system_monitor.onboard_blue_state = 1;
}

static void Onboard_Blue_LED_Off(void) {
    *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << (ONBOARD_BLUE_PIN + 16U));
    g_system_monitor.onboard_blue_state = 0;
}

static void Onboard_Red_LED_On(void) {
    *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << ONBOARD_RED_PIN);
    g_system_monitor.onboard_red_state = 1;
}

static void Onboard_Red_LED_Off(void) {
    *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << (ONBOARD_RED_PIN + 16U));
    g_system_monitor.onboard_red_state = 0;
}

// 外接LED控制 - 使用纯地址级宏定义
static void External_Green_LED_On(void) {
    EXTERNAL_GREEN_ON();
    g_system_monitor.external_green_state = 1;
}

static void External_Green_LED_Off(void) {
    EXTERNAL_GREEN_OFF();
    g_system_monitor.external_green_state = 0;
}

static void External_Blue_LED_On(void) {
    EXTERNAL_BLUE_ON();
    g_system_monitor.external_blue_state = 1;
}

static void External_Blue_LED_Off(void) {
    EXTERNAL_BLUE_OFF();
    g_system_monitor.external_blue_state = 0;
}

static void External_Red_LED_On(void) {
    EXTERNAL_RED_ON();
    g_system_monitor.external_red_state = 1;
}

static void External_Red_LED_Off(void) {
    EXTERNAL_RED_OFF();
    g_system_monitor.external_red_state = 0;
}

// LED组控制函数
void Green_LEDs_On(void) {
    Onboard_Green_LED_On();
    External_Green_LED_On();
}

void Green_LEDs_Off(void) {
    Onboard_Green_LED_Off();
    External_Green_LED_Off();
}

void Blue_LEDs_On(void) {
    Onboard_Blue_LED_On();
    External_Blue_LED_On();
}

void Blue_LEDs_Off(void) {
    Onboard_Blue_LED_Off();
    External_Blue_LED_Off();
}

void Red_LEDs_On(void) {
    Onboard_Red_LED_On();
    External_Red_LED_On();
}

void Red_LEDs_Off(void) {
    Onboard_Red_LED_Off();
    External_Red_LED_Off();
}

void All_LEDs_Off(void) {
    Green_LEDs_Off();
    Blue_LEDs_Off();
    Red_LEDs_Off();
}

void Switch_Mode(void) {
    if (g_system_monitor.current_mode == 1) {
        g_system_monitor.current_mode = 2;
    } else if (g_system_monitor.current_mode == 2) {
        g_system_monitor.current_mode = 3;
    } else {
        g_system_monitor.current_mode = 1;
    }
    g_system_monitor.pg3_press_count++;
    g_system_monitor.last_mode_change = g_system_monitor.loop_counter;
    
    // 新增：设置脉冲标志和计数器
    g_system_monitor.pg3_pressed_flag = 1;      // 产生脉冲上升沿
    g_system_monitor.pg3_pulse_counter = 3;     // 设置脉冲持续时间（3个循环周期）
}

uint32_t Get_PG3_Press_Count(void) {
    return g_system_monitor.pg3_press_count;
}
