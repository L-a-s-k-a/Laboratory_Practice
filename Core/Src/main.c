#include "../Inc/init.h"
#include "init.h"
#include <stdint.h>

uint8_t flag3 = 0;

/* Прямой доступ к нужным регистрам ввода/BSRR */
#define GPIOA_IDR (*(volatile uint32_t *)(0x40020000UL + 0x10UL))  /* PA0: бит0  */
#define GPIOC_IDR (*(volatile uint32_t *)(0x40020800UL + 0x10UL))  /* PC13: бит13 */
#define GPIOA_BSRR (*(volatile uint32_t *)(0x40020000UL + 0x18UL)) /* BSRR A      */
#define GPIOE_BSRR (*(volatile uint32_t *)(0x40021000UL + 0x18UL)) /* BSRR E      */
                                                                   /* PD12 — через CMSIS: GPIOD->BSRR */

/* === Вспомогательные функции управления светодиодами === */
static inline void LED_Off_All(void)
{
    /* OFF: PA5, PE0, PD12 */
    GPIOA_BSRR = 0x00200000UL;    /* BR5  */
    GPIOE_BSRR = 0x00010000UL;    /* BR0  */
    GPIOD->BSRR = GPIO_BSRR_BR12; /* BR12 */
}

static inline void LED_On(uint8_t idx)
{
    if (idx == 0U)
    {
        GPIOA_BSRR = 0x00000020UL; /* BS5  */
    }
    else if (idx == 1U)
    {
        GPIOE_BSRR = 0x00000001UL; /* BS0  */
    }
    else /* idx == 2 */
    {
        GPIOD->BSRR = GPIO_BSRR_BS12; /* BS12 */
    }
}

static inline void RefreshLEDs(uint8_t head, uint8_t cnt)
{
    LED_Off_All();
    if (cnt == 0U)
        return;

    /* 1-й (head) */
    LED_On(head);

    /* 2-й, если нужно */
    if (cnt >= 2U)
    {
        LED_On((uint8_t)((head + 1U) % 3U));
    }

    /* 3-й, если нужно */
    if (cnt >= 3U)

    {
        if (flag3 == 0)
        {
            LED_On((uint8_t)((head + 2U) % 3U));
            flag3 = 1;
        }
        else
        {
            LED_Off_All();
            flag3 = 0;
        }
    }
}

int main(void)
{
    /* Инициализации: A — вручную; C/E — макросы; D — CMSIS */
    GPIO_Init_PortA_Manual(); /* PA0 кнопка, PA5 LED (BSRR) */
    GPIO_Init_PortC_Macros(); /* PC13 кнопка (input, no pull) */
    GPIO_Init_PortE_Macros(); /* PE0 LED (BSRR) */
    GPIO_Init_PortD_CMSIS();  /* PD12 LED (BSRR) */

    /* Состояние: head — «первый LED»: 0=PA5, 1=PE0, 2=PD12; cnt — сколько горит (0..3) */
    uint8_t head = 0U;
    uint8_t cnt = 0U; /* стартуем с 0 — ничего не горит */

    /* Предыдущее состояние кнопок */
    uint8_t pc13_prev = 0U; /* PC13: внешний pull-down → отпущена=0, нажата=1 */
    uint8_t pa0_prev = 1U;  /* PA0 : внутренний pull-up → отпущена=1, нажата=0 */

    /* Старт: всё погасить, ничего не включаем (cnt=0) */
    LED_Off_All();
    /* Можно и так (эффект тот же): RefreshLEDs(head, cnt); */

    /* Состояние: head — «первый LED»: 0=PA5, 1=PE0, 2=PD12; cnt — сколько горит (0..3) */

    while (1)
    {

        /* Сырые значения входов */
        uint8_t pc13_now = ((GPIOC_IDR & 0x00002000U) ? 1U : 0U); /* PC13=бит13 (нажата=1) */
        uint8_t pa0_now = ((GPIOA_IDR & 0x00000001U) ? 1U : 0U);  /* PA0 =бит0  (нажата=0) */

        /* === PA0: фронт 1->0 — меняем ТОЛЬКО количество (0→1→2→3→0) === */
        if (pa0_prev == 1U && pa0_now == 0U)
        {
            /* debounce + подтверждение */
            volatile unsigned int d2 = 200000U;
            while (d2--)
            { /* nop */
            }
            if (((GPIOA_IDR & 0x00000001U) ? 1U : 0U) == 0U)
            {
                cnt++;
                if (cnt > 3U)
                    cnt = 0U; /* 0..3 по кругу */

                /* ждать отпускания */
                while (((GPIOA_IDR & 0x00000001U) ? 1U : 0U) == 0U)
                { /* hold */
                }

                /* Перерисовать по новым head/cnt */
                RefreshLEDs(head, cnt);
            }
        }
        pa0_prev = pa0_now;

        /* === PC13: фронт 0->1 — СДВИГ (head=0→1→2→0), cnt не трогаем === */
        if (pc13_prev == 0U && pc13_now == 1U)
        {
            volatile unsigned int d1 = 200000U;
            while (d1--)
            { /* ждём */
            }
            if ((GPIOC_IDR & 0x00002000U) != 0U)
            {
                head++;
                if (head > 2U)
                    head = 0U;

                /* ждать отпускания */
                while (((GPIOC_IDR & 0x00002000U) ? 1U : 0U) == 1U)
                { /* hold */
                }

                /* Перерисовать по новому head (если cnt=0 — всё останется погашено) */
                RefreshLEDs(head, cnt);
            }
        }
        pc13_prev = pc13_now;
    }
}
