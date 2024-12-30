#include "../Inc/init.h"
#include "../Inc/it_handlers.h"

uint16_t DelayTickCount = 0;
uint32_t GlobalTickBut1Wait = 0;

uint16_t LedSetLoad[3] = {530, 900, 2500};

uint8_t LedCurrfreq[6] = {0, 0, 0, 0, 0, 0};
uint16_t LedLoad[6] = {530, 530, 530, 530, 530, 530};
uint16_t LedCount[6] = {0, 0, 0, 0, 0, 0};
uint8_t LedState = 0;
uint8_t Ledflag[2][6] = {{1, 1, 1, 1, 1, 1}, {0, 0, 0, 0, 0, 0}};

uint8_t flagbut1 = 0;
uint8_t flagbut1long = 0;

const uint8_t LedOffset[6] = {0U, 7U, 14U, 12U, 13U, 15U};
uint8_t CurrentState = 0;
uint8_t Numb = 0;
uint8_t LedOnCount = 0; // Глобальная переменная для подсчета количества включенных светодиодов

void LedVal_Init();
void Led_light();

int main(void)
{
    GPIO_Init();
    RCC_Init();
    ITR_Init();
    SysTick_Init();
    LedVal_Init();

    while (1)
    {
        for (uint8_t i = 0; i < 6; i++) {
            if (LedCount[i] >= LedLoad[i]) {
                LedCount[i] = 0;
                if (Ledflag[1][i] == 1) {
                    Ledflag[1][i] = 0;
                }
                else {
                    Ledflag[1][i] = 1;
                }
            }
        }

        Numb = CurrentState % 7;
        if (Numb != 0) {
            for (uint8_t i = 0; i < Numb; i++) {
                Ledflag[0][i] = 1;
            }
        } else {
            for (uint8_t i = 0; i < 6; i++) {
                Ledflag[0][i] = 0;
            }
            CurrentState = 0;
        }

        if (flagbut1long == 1) {
            for (uint8_t i = 0; i < 6; i++) {
                LedCurrfreq[i] = (LedCurrfreq[i] + 1) % 3;
            }

            for (uint8_t i = 0; i < 6; i++) {
                LedLoad[i] = LedSetLoad[LedCurrfreq[i]];
            }

            flagbut1long = 0;
        }

        if (flagbut1 == 1 && READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) == 0 && GlobalTickBut1Wait >= 50) {
            flagbut1 = 0;
        }

        Led_light();
    }
}

void Led_light()
{
    uint32_t ODR_clear = 0x0UL;
    uint32_t ODR_set = 0x0UL;
    LedOnCount = 0; // Сброс счетчика включенных светодиодов

    if (LedState == 1) {
        for (uint8_t i = 0; i < 6; i++) {
            if (Ledflag[0][i] == 1) {
                ODR_set = ODR_set + (0x1UL << LedOffset[i]);
                LedOnCount++; // Увеличение счетчика включенных светодиодов
            } else {
                ODR_clear = ODR_clear + (0x1UL << LedOffset[i]);
            }
        }
    } else {
        for (uint8_t i = 0; i < 6; i++) {
            if ((Ledflag[0][i] == 1) && (Ledflag[1][i] == 1)) {
                ODR_set = ODR_set + (0x1UL << LedOffset[i]);
                LedOnCount++; // Увеличение счетчика включенных светодиодов
            } else {
                ODR_clear = ODR_clear + (0x1UL << LedOffset[i]);
            }
        }
    }
    MODIFY_REG(GPIOB->ODR, ODR_clear, ODR_set);
}

void LedVal_Init()
{
    for (uint8_t i = 0; i < 6; i++) {
        LedLoad[i] = LedSetLoad[LedCurrfreq[i]];
        LedCount[i] = LedLoad[i];
    }
}
