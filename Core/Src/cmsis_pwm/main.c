#include <stdbool.h>
#include "init.h"

enum { DEBOUNCE_MS = 20, LONG_PRESS_MS = 800 };
enum { NO_PRESS, SHORT_PRESS, LONG_PRESS };

typedef struct {
    bool raw; //true - 0, то есть нажато
    bool pressed; //подтверждение нажатия
    bool long_sent; //	было ли уже выдано длинное событие для этого удержания
    uint32_t changed_at; //отсчет таймера 2, когда последний раз изменилось raw
    uint32_t pressed_at;// когда нажатие подтвердилось
} Button;

static int read_button(Button *button, bool raw, uint32_t now)
{
    //если новый raw != текущий raw функция сохраняет новый уровень и запоминает changed_at = now.
    //Если контакт снова переключится из-за дребезга, отсчёт 20 мс начнётся заново.
    if (raw != button->raw) {
        button->raw = raw;
        button->changed_at = now;
    }
// подтверждаем нажатие или отпускание кнопки
    if (raw != button->pressed && now - button->changed_at >= DEBOUNCE_MS) {
        button->pressed = raw;
        if (raw) {
            button->pressed_at = now;
            button->long_sent = false;
        } else if (!button->long_sent) {
            return SHORT_PRESS;
        }
    }
//проверяем удержание
    if (button->pressed && raw && !button->long_sent &&
        now - button->pressed_at >= LONG_PRESS_MS) {
        button->long_sent = true;
        return LONG_PRESS;
    }
    return NO_PRESS;
}

int main(void)
{
    const uint32_t leds[] = {GPIO_BSRR_BS_13, GPIO_BSRR_BS_14, GPIO_BSRR_BS_15};
    const uint32_t periods[] = {1000U, 500U, 250U};
    Button s1 = {0}, s2 = {0}, s3 = {0};
    bool output = false; //изначально кнопка 3 - вход
    bool swapped = false; //cначала S1 зажигает следующий
    unsigned speed = 0; //первый период 1000
    unsigned lit = 0;//сколько диодов горит изначально

    GPIO_Ini();

    while (1) {
        uint32_t now = TIM2->CNT; //текущее время
        uint32_t pins = GPIOE->IDR;//входные уровни порта
        int event1 = read_button(&s1, (pins & GPIO_IDR_ID7) == 0U, now);
        int event2 = read_button(&s2, (pins & GPIO_IDR_ID8) == 0U, now);

        if (event1 == LONG_PRESS) {
            output = !output;
            if (output) {
                SET_BIT(GPIOE->MODER, GPIO_MODER_MODER9_0);
                TIM1->EGR = TIM_EGR_UG;
                SET_BIT(GPIOE->MODER, GPIO_MODER_MODER10_1);
            } else {
                CLEAR_BIT(GPIOE->MODER, GPIO_MODER_MODER10);
                CLEAR_BIT(GPIOE->MODER, GPIO_MODER_MODER9);
            }
            // Начинаем отслеживать S3 с нового состояния после смены режима.
            s3 = (Button){0};
            pins = GPIOE->IDR;
        }

        if (!output && read_button(&s3, (pins & GPIO_IDR_ID9) == 0U, now) == SHORT_PRESS) {
            swapped = !swapped;
        }

        if (event2 == LONG_PRESS) {
            speed = (speed + 1U) % 3U;
            TIM1->ARR = periods[speed] - 1U;
            TIM1->CCR2 = periods[speed] / 2U;
            TIM1->EGR = TIM_EGR_UG;
        }

        bool turn_off = (swapped ? event1 : event2) == SHORT_PRESS;
        bool turn_on = (swapped ? event2 : event1) == SHORT_PRESS;
        if (turn_off && lit > 0U) {
            lit--;
            GPIOD->BSRR = leds[lit] << 16;
        } else if (!turn_off && turn_on && lit < 3U) {
            GPIOD->BSRR = leds[lit];
            lit++;
        }
    }
}
