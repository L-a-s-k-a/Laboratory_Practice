#include <stdint.h>

#define RCC_AHB1ENR     (*(volatile uint32_t*)(0x40023800UL + 0x30UL))
// Макрос для регистра тактирования RCC AHB1ENR
// Адрес 0x40023800 + смещение 0x30 = 0x40023830
// volatile uint32_t* - указатель на 32-битную ячейку памяти

#define GPIOA_MODER     (*(volatile uint32_t*)(0x40020000UL + 0x00UL))
// Макрос для регистра MODER порта A 
// Адрес 0x40020000 + смещение 0x00

#define GPIOA_BSRR      (*(volatile uint32_t*)(0x40020000UL + 0x18UL))
// Макрос для регистра BSRR порта A (установка/сброс битов)
// Адрес 0x40020000 + смещение 0x18 = 0x40020018

#define GPIOB_MODER     (*(volatile uint32_t*)(0x40020400UL + 0x00UL))
// Макрос для регистра MODER порта B
// Адрес 0x40020400 + смещение 0x00

#define GPIOB_BSRR      (*(volatile uint32_t*)(0x40020400UL + 0x18UL))
// Макрос для регистра BSRR порта B
// Адрес 0x40020400 + смещение 0x18 = 0x40020418

#define GPIOC_IDR       (*(volatile uint32_t*)(0x40020800UL + 0x10UL))
// Макрос для регистра IDR порта C (входные данные)
// Адрес 0x40020800 + смещение 0x10 = 0x40020810

#define CLOCK_GPIOABC       0x00000007UL
// Значение для включения тактирования GPIOA, GPIOB, GPIOC
// 0x07 = 0b00000111 = биты 0, 1, 2

#define GPIOB_MODE_OUT      0x10004001UL
// Значение для настройки PB0, PB7, PB14 на выход
// Биты 0-1 (PB0): 01, биты 14-15 (PB7): 01, биты 28-29 (PB14): 01

#define GPIOA_MODE_OUT      0x00001000UL
// Значение для настройки PA6 на выход
// Биты 12-13 (PA6): 01

#define LED1_SET            0x00000001UL
// Значение для установки PB0 (бит 0 в BSRR)

#define LED1_RESET          0x00010000UL
// Значение для сброса PB0 (бит 16 в BSRR, 0+16)

#define LED2_SET            0x00000080UL
// Значение для установки PB7 (бит 7 в BSRR)
// 0x80 = 0b10000000

#define LED2_RESET          0x00800000UL
// Значение для сброса PB7 (бит 23 в BSRR, 7+16)

#define LED3_SET            0x00004000UL
// Значение для установки PB14 (бит 14 в BSRR)

#define LED3_RESET          0x40000000UL
// Значение для сброса PB14 (бит 30 в BSRR, 14+16)

#define LED4_SET            0x00000040UL
// Значение для установки PA6 (бит 6 в BSRR порта A)

#define LED4_RESET          0x00400000UL
// Значение для сброса PA6 (бит 22 в BSRR порта A, 6+16)

#define BTN_PIN13           0x00002000UL
// Маска для чтения 13-го бита (PC13)
// 0x2000 = 0b0010 0000 0000 0000 = бит 13

#define DEBOUNCE_TICKS      100000UL
// Количество циклов задержки для антидребезга

#define LEDS_COUNT          4U
// Общее количество светодиодов в схеме (4)

#define CYCLE_PRESSES       6U
// Каждое 6-е нажатие - все включены)


#define SET_BITS(REG, MASK)     ((REG) |= (MASK))
// Макрос для установки битов: REG = REG | MASK
// Устанавливает биты из MASK в REG, не трогая остальные

#define WRITE_BITS(REG, VAL)    ((REG) = (VAL))
// Макрос для полной записи значения в регистр
// Заменяет всё содержимое REG на VAL

#define READ_BITS(REG, MASK)    ((REG) & (MASK))
// Макрос для чтения битов: возвращает REG & MASK
// Изолирует биты из MASK

#define BTN_PRESSED()           (READ_BITS(GPIOC_IDR, BTN_PIN13) != 0)
// Макрос для проверки нажатия кнопки
// Читает регистр IDR порта C, применяет маску для бита 13
// Если результат не равен 0 (бит 13 = 1), кнопка считается нажатой

volatile uint8_t flag = 0;

volatile uint8_t leds_on = 0;

volatile uint8_t press_cnt = 0;

static void delay(volatile uint32_t t)
{
    while (t--) {}
    // Пустой цикл, выполняется пока t не станет 0
}

static void GPIO_Ini(void)
// Функция инициализации портов GPIO
{
    SET_BITS(RCC_AHB1ENR, CLOCK_GPIOABC);
    // Включаем тактирование портов A, B, C
    // Вызов макроса: RCC_AHB1ENR |= 0x00000007UL

    SET_BITS(GPIOB_MODER, GPIOB_MODE_OUT);
    // Настраиваем PB0, PB7, PB14 на режим выхода
    // Вызов макроса: GPIOB_MODER |= 0x10004001UL

    SET_BITS(GPIOA_MODER, GPIOA_MODE_OUT);
    // Настраиваем PA6 на режим выхода
    // Вызов макроса: GPIOA_MODER |= 0x00001000UL
    // Пин PC13 остается в режиме входа (состояние после сброса)
}

// Включает первые n, выключает остальные
static void Leds_Set(uint8_t n)
{
    WRITE_BITS(GPIOB_BSRR, (n >= 1) ? LED1_SET : LED1_RESET);
    // Если n >= 1, записываем LED1_SET (установка PB0), иначе LED1_RESET (сброс PB0)
    // Тернарный оператор: условие ? значение_если_да : значение_если_нет

    WRITE_BITS(GPIOB_BSRR, (n >= 2) ? LED2_SET : LED2_RESET);

    WRITE_BITS(GPIOB_BSRR, (n >= 3) ? LED3_SET : LED3_RESET);

    WRITE_BITS(GPIOA_BSRR, (n >= 4) ? LED4_SET : LED4_RESET);
}

int main(void)
{
    GPIO_Ini();
    // Вызываем функцию инициализации портов

    Leds_Set(0);
    // Вызываем функцию управления LED с аргументом 0 (все выключены)

    while (1)
    {
        if (BTN_PRESSED())
            // Проверяем нажатие кнопки через макрос
        {
            flag = 1;
            // Устанавливаем флаг для STMViewer

            delay(DEBOUNCE_TICKS);
            // Задержка для антидребезга

            if (BTN_PRESSED())
            {
                press_cnt++;
                // Увеличиваем счетчик нажатий

                if (press_cnt >= CYCLE_PRESSES)
                    // Если счетчик >= 6
                {
                    press_cnt = 0;
                    leds_on = LEDS_COUNT;
                    // Включаем все 4 светодиода
                }
                else if (press_cnt > LEDS_COUNT)
                    // Если счетчик > 4 (равен 5)
                {
                    leds_on = 0;
                }
                else
                    // Если счетчик от 1 до 4
                {
                    leds_on = press_cnt;
                    // Количество LED равно номеру нажатия
                }

                Leds_Set(leds_on);
                // Применяем состояние к пинам

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