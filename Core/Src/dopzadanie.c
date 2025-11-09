// Доп. задание

// #include "../Inc/init.h"
// #include "init.h"
// #include <stdint.h>

// /* ===== GPIO прямой доступ ===== */
// #define GPIOA_IDR (*(volatile uint32_t *)(0x40020000UL + 0x10UL))  /* PA0: бит0  */
// #define GPIOC_IDR (*(volatile uint32_t *)(0x40020800UL + 0x10UL))  /* PC13: бит13 */
// #define GPIOA_BSRR (*(volatile uint32_t *)(0x40020000UL + 0x18UL)) /* BSRR A      */
// #define GPIOE_BSRR (*(volatile uint32_t *)(0x40021000UL + 0x18UL)) /* BSRR E      */
// /* PD12 — через CMSIS: GPIOD->BSRR */

// /* ===== Вспомогательные функции LED ===== */
// static inline void LED_Off_All(void)
// {
//     GPIOA_BSRR = 0x00200000UL;    /* PA5 off (BR5) */
//     GPIOE_BSRR = 0x00010000UL;    /* PE0 off (BR0) */
//     GPIOD->BSRR = GPIO_BSRR_BR12; /* PD12 off      */
// }

// static inline void LED_On(uint8_t idx)
// {
//     if (idx == 0U)
//     {
//         GPIOA_BSRR = 0x00000020UL;
//     } /* PA5 on (BS5)  */
//     else if (idx == 1U)
//     {
//         GPIOE_BSRR = 0x00000001UL;
//     } /* PE0 on (BS0)  */
//     else
//     {
//         GPIOD->BSRR = GPIO_BSRR_BS12;
//     } /* PD12 on  */
// }

// /* Рисуем "статическую" картинку по head/cnt */
// static inline void RefreshLEDs(uint8_t head, uint8_t cnt)
// {
//     LED_Off_All();
//     if (cnt == 0U)
//         return;

//     LED_On(head); /* 1-й */
//     if (cnt >= 2U)
//         LED_On((uint8_t)((head + 1U) % 3U)); /* 2-й */
//     if (cnt >= 3U)
//         LED_On((uint8_t)((head + 2U) % 3U)); /* 3-й */
// }

// static inline void DrawAccordingToState(uint8_t head, uint8_t cnt,
//                                         uint8_t blink_idx, uint8_t blink_on)
// {
//     if (blink_idx == 0U)
//     { /* мигание выкл */
//         RefreshLEDs(head, cnt);
//     }
//     else
//     {
//         if (blink_on)
//             RefreshLEDs(head, cnt);
//         else
//             LED_Off_All();
//     }
// }

// int main(void)
// {
//     /* Инициализация портов (как у вас) */
//     GPIO_Init_PortA_Manual(); /* PA0 кнопка, PA5 LED */
//     GPIO_Init_PortC_Macros(); /* PC13 кнопка         */
//     GPIO_Init_PortE_Macros(); /* PE0 LED             */
//     GPIO_Init_PortD_CMSIS();  /* PD12 LED            */

//     /* Состояние */
//     uint8_t head = 0U; /* 0=PA5, 1=PE0, 2=PD12 */
//     uint8_t cnt = 0U;  /* 0..3, стартуем с 0 — всё погашено */

//     /* Кнопки состояний */
//     uint8_t pc13_prev = 0U; /* PC13: pull-down отпущена=0, нажата=1 */
//     uint8_t pa0_prev = 1U;  /* PA0 : pull-up отпущена=1, нажата=0 */

//     /* ==== Мигание без таймеров ==== */
//     /* blink_idx: 0=выкл, 1..3 = три частоты */
//     uint8_t blink_idx = 0U;
//     uint8_t blink_on = 1U;       /* текущая фаза 1=свет */
//     uint32_t blink_counter = 0U; /* счётчик до переключения фазы */

//     /* Периоды */
//     const uint32_t BLINK_PERIOD_TICKS[3] = {
//         40000U, /* F1 — медленно  */
//         20000U, /* F2 — средне    */
//         10000U  /* F3 — быстро    */
//     };

//     /* Порог удержания PA0 в «итерациях ожидания отпуска» (без таймеров) */
//     const uint32_t HOLD_THRESHOLD_TICKS = 4000000U;

//     /* Старт: всё погасить */
//     LED_Off_All();

//     while (1)
//     {
//         /* --- Часть 1. Драйвер мигания (простой счётчик) --- */
//         if (blink_idx != 0U && cnt != 0U)
//         {
//             blink_counter++;
//             if (blink_counter >= BLINK_PERIOD_TICKS[blink_idx - 1U])
//             {
//                 blink_counter = 0U;
//                 blink_on ^= 1U;
//                 DrawAccordingToState(head, cnt, blink_idx, blink_on);
//             }
//         }

//         /* --- Часть 2. Опрос кнопок --- */
//         uint8_t pc13_now = ((GPIOC_IDR & 0x00002000U) ? 1U : 0U); /* PC13: нажата=1 */
//         uint8_t pa0_now = ((GPIOA_IDR & 0x00000001U) ? 1U : 0U);  /* PA0 : нажата=0 */

//         /* === PA0: анализ короткого/длинного === */
//         if (pa0_prev == 1U && pa0_now == 0U) /* фронт 1->0 */
//         {
//             /* Небольшой дебаунс */
//             volatile unsigned int d2 = 200000U;
//             while (d2--)
//             { /* nop */
//             }

//             if (((GPIOA_IDR & 0x00000001U) ? 1U : 0U) == 0U)
//             {
//                 /* Измерим «длительность» удержания в тиках,
//                    просто считая, пока держат кнопку */
//                 uint32_t hold_ticks = 0U;

//                 /* Ждём отпускания, параллельно считаем hold_ticks
//                    (в это время мигание может "подвиснуть" — это допустимо в условии) */
//                 while (((GPIOA_IDR & 0x00000001U) ? 1U : 0U) == 0U)
//                 {
//                     hold_ticks++;
//                     /* Небольшая «нагрузка», чтобы тик был ощутимее */
//                     __asm volatile("nop");
//                 }

//                 if (hold_ticks >= HOLD_THRESHOLD_TICKS)
//                 {
//                     /* ДЛИННОЕ НАЖАТИЕ: переключаем режим мигания Off->F1->F2->F3->Off */
//                     blink_idx = (uint8_t)((blink_idx + 1U) % 4U);
//                     blink_on = 1U;      /* начнём новую частоту со "светлой" фазы */
//                     blink_counter = 0U; /* сброс счётчика периода */
//                     DrawAccordingToState(head, cnt, blink_idx, blink_on);
//                 }
//                 else
//                 {
//                     /* КОРОТКОЕ НАЖАТИЕ: меняем количество (0→1→2→3→0) */
//                     cnt++;
//                     if (cnt > 3U)
//                         cnt = 0U;
//                     DrawAccordingToState(head, cnt, blink_idx, blink_on);
//                 }
//             }
//         }
//         pa0_prev = pa0_now;

//         /* === PC13: фронт 0->1 — сдвиг головы (head=0→1→2→0) === */
//         if (pc13_prev == 0U && pc13_now == 1U)
//         {
//             volatile unsigned int d1 = 200000U;
//             while (d1--)
//             { /* nop */
//             }

//             if ((GPIOC_IDR & 0x00002000U) != 0U)
//             {
//                 /* ждать отпускания (простая блокировка) */
//                 while (((GPIOC_IDR & 0x00002000U) ? 1U : 0U) == 1U)
//                 { /* hold */
//                 }

//                 head++;
//                 if (head > 2U)
//                     head = 0U;

//                 DrawAccordingToState(head, cnt, blink_idx, blink_on);
//             }
//         }
//         pc13_prev = pc13_now;
//     }
// }