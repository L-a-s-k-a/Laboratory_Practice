#include "stm32f4xx.h"

/*
 *   LED1 - PB0  (зеленая)
 *   LED2 - PB7  (синяя)
 *   LED3 - PB14 (красная)
 *   LED4 - PA6  (D12, доп)
 *   Button - PC13
 */


#define BTN_PRESSED()       (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0) // Макрос для проверки нажатия кнопки
 // GPIOC->IDR - регистр входных данных порта C
 // GPIO_IDR_IDR_13 - маска для 13-го бита 
 // READ_BIT - макрос CMSIS для чтения битов (делает побитовое И)

#define DEBOUNCE_TICKS      100000UL // Количество циклов задержки для антидребезга

#define LEDS_COUNT          4U // Общее количество светодиодов (4)

#define CYCLE_PRESSES       6U // 6е нажатие

volatile uint8_t flag = 0; // Переменная - флаг нажатия

volatile uint8_t leds_on = 0; // Количество включенных LED

volatile uint8_t press_cnt = 0; // Счетчик нажатий

static void delay(volatile uint32_t ticks) // Статическая функция задержки
{
    while (ticks--) {}
    // Пустой цикл, выполняется пока ticks не станет 0
}

static void GPIO_Ini(void)
// Функция инициализации портов GPIO
{

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); // Включаем тактирование портов A, B, C
    // RCC->AHB1ENR - регистр тактирования 
    // RCC_AHB1ENR_GPIOAEN - бит включения тактирования GPIOA и тд
    // SET_BIT - макрос CMSIS для установки битов

    //LED1: PB0
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0); // Настраиваем PB0 на режим выхода
    // GPIOB->MODER - регистр режима порта B
    // GPIO_MODER_MODE0_0 - макрос CMSIS для бита 0 (режим 01 = Output)

    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0); // Настраиваем PB0 на тип выхода
    // GPIOB->OTYPER - регистр типа выхода
    // GPIO_OTYPER_OT_0 - макрос для бита 0
    // CLEAR_BIT - макрос CMSIS для сброса бита

    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0); // Устанавливаем среднюю скорость переключения для PB0
    // GPIOB->OSPEEDR - регистр скорости
    // GPIO_OSPEEDER_OSPEEDR0_0 - макрос для битов скорости пина 0

    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0); // Отключаем подтяжку для PB0, как оно было в начале 
    // GPIOB->PUPDR - регистр подтяжки
    // GPIO_PUPDR_PUPDR0 - макрос для битов подтяжки пина 0
    // CLEAR_BIT сбрасывает биты в 00 (без подтяжки)

    // LED2: PB7
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0); // Настраиваем PB7 на режим выхода
    // GPIO_MODER_MODE7_0 - макрос для битов 14-15 (пин 7)

    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7); // Push-Pull для PB7

    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0); // Средняя скорость для PB7

    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7); // Без подтяжки для PB7

    // LED3: PB14
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0); // Настраиваем PB14 на режим выхода
    // GPIO_MODER_MODE14_0 - макрос для битов 28-29 (пин 14)

    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14); // Push-Pull для PB14

    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0); // Средняя скорость для PB14

    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14); // Без подтяжки для PB14

    // LED4: PA6 
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODE6_0); // Настраиваем PA6 на режим выхода
    // GPIO_MODER_MODE6_0 - макрос для битов 12-13 (пин 6)

    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_6); // Push-Pull для PA6

    SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEEDR6_0); // Средняя скорость для PA6

    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR6); // Без подтяжки для PA6

}

// Функция управления светодиодами: включает первые n, выключает остальные
static void Leds_Set(uint8_t n)
{
    if (n >= 1) { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS0); } // Если n >= 1, записываем GPIO_BSRR_BS0 в регистр BSRR порта B
    // WRITE_REG - макрос CMSIS для полной записи в регистр
    // GPIO_BSRR_BS0 - макрос для установки бита 0
    else { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR0); } // Иначе записываем GPIO_BSRR_BR0 (сброс бита 0)
    // GPIO_BSRR_BR0 - макрос для бита 16

    if (n >= 2) { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS7); } // Если n >= 2, включаем PB7
    // GPIO_BSRR_BS7 - макрос для бита 7
    else { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR7); } // Иначе выключаем PB7
    // GPIO_BSRR_BR7 - макрос для бита 23

    if (n >= 3) { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS14); } // Если n >= 3, включаем PB14
    // GPIO_BSRR_BS14 - макрос для бита 14
    else { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR14); } // Иначе выключаем PB14
    // GPIO_BSRR_BR14 - макрос для бита 30 (14+16)

    if (n >= 4) { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS6); } // Если n >= 4, включаем PA6
    // GPIOA->BSRR - регистр BSRR порта A
    // GPIO_BSRR_BS6 - макрос для бита 6
    else { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR6); } // Иначе выключаем PA6
    // GPIO_BSRR_BR6 - макрос для бита 22 (6+16)
}

int main(void)
{
    GPIO_Ini(); // Вызываем функцию инициализации портов

    Leds_Set(0); // Вызываем функцию управления LED с аргументом 0 (все выкл)

    while (1)
    {
        if (BTN_PRESSED()) // Проверяем нажатие кнопки через макрос
        {
            flag = 1;

            delay(DEBOUNCE_TICKS);

            if (BTN_PRESSED())
            {
                press_cnt++;
                // Увеличиваем счетчик нажатий на 1

                if (press_cnt >= CYCLE_PRESSES)
                    // Если счетчик >= 6
                {
                    press_cnt = 0;
                    // Сбрасываем счетчик в 0
                    leds_on = LEDS_COUNT;
                    // Все LED вкл
                }
                else if (press_cnt > LEDS_COUNT)
                    // Если счетчик > 4 (равен 5)
                {
                    leds_on = 0;
                    // Выключаем все LED
                }
                else
                    // Если счетчик от 1 до 4
                {
                    leds_on = press_cnt;
                    // Количество включенных LED равно номеру нажатия
                }

                Leds_Set(leds_on);
                // Применяем новое состояние к физическим пинам

                while (BTN_PRESSED()) {}
                delay(DEBOUNCE_TICKS);
            }
            flag = 0;
        }
        else
            // Если кнопка не нажата
        {
            flag = 0;
        }
    }
}