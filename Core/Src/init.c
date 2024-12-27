#include "init.h"
void RCC_Ini(void){
    /* Предварительная очистка регистров RCC *///将所有rcc时钟清除
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U); //调整内部高速时钟频率
    CLEAR_REG(RCC->CFGR);
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
    /* Настройка главного регистра RCC */
    SET_BIT(RCC->CR, RCC_CR_HSEON); //启用外部时钟
    while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET); //用hserdy检测时钟是否启用
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); //Сбросим бит байпаса в 0, если вдруг там что-то лежит
    SET_BIT(RCC->CR, RCC_CR_CSSON); //打开时钟安全系统
/* Натсройка регистров PLL下面的代码设置寄存器位以将系统时钟 SYSCLK 设置为 180 MHz（微控制器的最大可能时钟频率）并启动 PLL 模块。相锁环
* Предварительная очистка регистра
* В качестве источника тактирования для PLL выбирается HSE
* Мы сначала делим входную частоту (HSE) на 4 (получаем 2 МГц),
затем умножем на 180 и снова делим на 2, таким образом получем 180МГц
* Включаем работу PLL
*/
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);//使用外部时钟作输入源
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2);//输入分频器先除于4得到输入pll频率为2mhz
    //Выставляем предделитель входной частоты PLL на 4
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 |
    RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7);//通过设置 PLLN 位来调整 VCO 乘法器,结果为180
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); //通过清零 PLLP 位来调整 VCO 输出频率预分频器,如果清零，则预分频器将设置为 2。即经过乘法后输出时除于2,我们调整乘法后所得频率的预分频器。换句话说，我们得到了最终的PLL频率
    SET_BIT(RCC->CR, RCC_CR_PLLON); //启动pll
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY)); //Ждём запуска PLL
/* Настройка основных конфигураций RCC
* В качетсве системных часов выбираем выход PLL
* Настраиваем предделители шин AHB и APB
* Настраиваем выходы MCO1 и MCO2 для внешней оценки настроенной
системы тактирования
*/
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL); //将 PLL 模块的输出设置为控制器时钟频率。
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); //设置AHB总线分频器。RCC_CFGR_HPRE_DIV1关闭分频器.AHB 总线的时钟频率为 180 MHz
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);//APB1分频器，文档说该总线的时钟不应该超过 45 MHz。由于分频后获得的频率AHB，即180 MHz预分频器需要设置为4
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);//Предделитель APВ2, делим на 2.APB2 总线时钟不超过 90 MHz。
    
    // //设置mco时钟输出用作检测,可不写
    // MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_Msk);//Предделитель на выходе MCO2 (PC9) = 5设置MCO2 输出的预分频器。该输出对于检查接收频率是必要的。由于普通示波器无法读取50 MHz以上的频率，因此在该块的输出处安装了预分频器5，因此所得频率将为36 MHz。
    // CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2); //Настраиваем на выход MCO2 -System clock选择系统时钟输出
/* Настройка задержки внутренней памяти
* Выставление битов LATENCY регистра FLASH_ACR в позицию 5SW (6 CPU cycles).将 FLASH_ACR 寄存器的 LATENCY 位设置为位置 5SW（6 个 CPU 周期）。
* Данная настройка необходима при увеличении системной частоты
тактирования свыше 20 МГц. Таблица 12 RM0090当系统时钟频率增加到 20 MHz 以上时，需要进行此设置。
*/
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
}
void GPIO_Ini(void){
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN |RCC_AHB1ENR_GPIOCEN);//Включение тактирования портов GPIOB и GPIOC
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE2_0|GPIO_MODER_MODE6_0|GPIO_MODER_MODE1_0|GPIO_MODER_MODE14_0|GPIO_MODER_MODE7_0|GPIO_MODER_MODE0_0); //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_2|GPIO_OTYPER_OT_6|GPIO_OTYPER_OT_1|GPIO_OTYPER_OT_14|GPIO_OTYPER_OT_7|GPIO_OTYPER_OT_0); //Настройка на Push- Pull работу 7-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR2_0|GPIO_OSPEEDER_OSPEEDR6_0|GPIO_OSPEEDER_OSPEEDR1_0|GPIO_OSPEEDER_OSPEEDR14_0|GPIO_OSPEEDER_OSPEEDR7_0|GPIO_OSPEEDER_OSPEEDR0_0); //Настройка скорости работы 7-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR2_0|GPIO_PUPDR_PUPDR6_0|GPIO_PUPDR_PUPDR1_0|GPIO_PUPDR_PUPDR14_0|GPIO_PUPDR_PUPDR7_0|GPIO_PUPDR_PUPDR0_0); //Отключение PU/PD резисторов для 7-го пина GPIOB
    // SET_BIT(GPIOC->MODER, GPIO_MODER_MODE13|GPIO_MODER_MODE6);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD13_1|GPIO_PUPDR_PUPD6_1); // Настройка порта РС13 на вход со стяжкой (Pull-down)
}
void EXTI_ITR_Ini(void){
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); 
    // MODIFY_REG(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI6_Msk,SYSCFG_EXTICR2_EXTI6_PC);//EXTICR1控制EXTI0-3;EXTICR2控制EXTI4-7;
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk,SYSCFG_EXTICR4_EXTI13_PC); //Настройка мультиплексора на вывод линии прерывания EXTI13 на PC13
  SET_BIT(EXTI->IMR, EXTI_IMR_MR13); //imr打开13线上的中断请求|EXTI_IMR_MR6
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13); //设置上升沿触发|EXTI_RTSR_TR6
    CLEAR_BIT(EXTI->FTSR, EXTI_FTSR_TR13); //Настройка детектирования спадающего фронта 13 линии|EXTI_FTSR_TR6

    NVIC_SetPriority(EXTI15_10_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 2));
    // NVIC_SetPriority(EXTI9_5_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 2));
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    // NVIC_EnableIRQ(EXTI9_5_IRQn);
    
}
void SysTick_Init(void){
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); //关闭计数器
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk); //TICKINT=1,计数器到0后使能systick中断
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk); //选择时钟为AHB.置位0为AHB/8,置位1为AHB
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk,179999 << SysTick_LOAD_RELOAD_Pos); //load寄存器记录从开始计数到 0 的值。AHB总线180Mhz设置180000-1为1毫秒 180000000为1秒
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk,179999 << SysTick_VAL_CURRENT_Pos); //val寄存器允许您跟踪当前计数器值,向这些位写入任何值都会将该字段清除为 0，并且还将清除 STK_CTRL 寄存器中的 COUNTFLAG 位。
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); //Включим счётчик
    }