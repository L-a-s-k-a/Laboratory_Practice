#include "Interrupt.h"

/* Глобальные переменные */
volatile uint32_t g_msTicks = 0U;
volatile uint8_t g_currentLed = 0U; // 0..3
volatile uint8_t g_speedIndex = 0U; // 0..3
volatile uint8_t g_running = 0U;    // 0 - не запущено, 1 - запущено

volatile uint32_t g_debugCounter = 0;

/* Для антидребезга */
static volatile uint8_t g_btnDebounceActive = 0U;
static volatile uint32_t g_btnDebounceDeadline = 0U; // время (мс), когда надо проверить кнопку

/* ===== SysTick: таймер 1 мс ===== */
void SysTick_Handler(void)
{
    g_msTicks++;

    /* Обработка антидребезга кнопки */
    if (g_btnDebounceActive)
    {
        /* Используем арифметику со знаком для корректной работы при переполнении */
        if ((int32_t)(g_msTicks - g_btnDebounceDeadline) >= 0)
        {
            g_btnDebounceActive = 0U;

            /* Читаем состояние PA0 ещё раз:
               pull-up => "0" означает "кнопка всё ещё нажата" */
            if ((GPIOA->IDR & GPIO_IDR_ID0) == 0U)
            {
                /* Подтверждённое нажатие */

                if (g_running == 0U)
                {
                    /* Первый запуск гирлянды */
                    g_running = 1U;
                    g_currentLed = 0U;
                    g_speedIndex = 0U;

                    TIM2_UpdateFrequency(g_speedIndex);
                    LEDs_AllOff();
                    LED_OnIndex(g_currentLed);

                    /* Запускаем TIM2 */
                    SET_BIT(TIM2->CR1, TIM_CR1_CEN);
                }
                else
                {
                    /* Гирлянда уже работает: переключаем скорость */
                    g_speedIndex = (g_speedIndex + 1U) & 0x03U;
                    TIM2_UpdateFrequency(g_speedIndex);
                }
            }

            /* После завершения дебаунса снова разрешаем EXTI0 */
            SET_BIT(EXTI->IMR, EXTI_IMR_MR0);
        }
    }
}

/* ===== TIM2: бегущие огни ===== */
void TIM2_IRQHandler(void)
{
    if (READ_BIT(TIM2->SR, TIM_SR_UIF))
    {
        /* Сброс флага обновления */
        CLEAR_BIT(TIM2->SR, TIM_SR_UIF);

        if (g_running)
        {
            /* Гасим все светодиоды */
            LEDs_AllOff();
            g_debugCounter++; // <<<< ВОТ ЭТА ПЕРЕМЕННАЯ ДЛЯ MCUViewer

            /* Следующий светодиод по кругу 0..3 */
            g_currentLed = (g_currentLed + 1U) & 0x03U;

            /* Включаем нужный */
            LED_OnIndex(g_currentLed);
        }
    }
}

/* ===== EXTI0: кнопка PA0, запуск антидребезга ===== */
void EXTI0_IRQHandler(void)
{
    if (READ_BIT(EXTI->PR, EXTI_PR_PR0))
    {
        /* Сброс флага прерывания */
        WRITE_REG(EXTI->PR, EXTI_PR_PR0);

        /* Если уже идёт дебаунс - игнорируем новые фронты */
        if (g_btnDebounceActive == 0U)
        {
            g_btnDebounceActive = 1U;
            g_btnDebounceDeadline = g_msTicks + 20U; // 20 мс

            /* На время дебаунса запрещаем новые прерывания по EXTI0 */
            CLEAR_BIT(EXTI->IMR, EXTI_IMR_MR0);
        }
    }
}
