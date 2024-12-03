#include "../Inc/init.h"

uint8_t flag = 1;
uint8_t count = 0;
uint8_t reverse = 1;

uint8_t flag1, flag2, flag3, flag4, flag5, flag6, LedState;
uint8_t flag1ON, flag2ON, flag3ON, flag4ON, flag5ON, flag6ON;
uint8_t BtnCount1, LongBtnCount1, VeryLongBtnCount;
uint16_t ledTime1, ledTime2, ledTime3, ledTime4, ledTime5, ledTime6;
uint16_t freq1, freq2, freq3, freq4, freq5, freq6;
uint16_t GlobalTickCount;
uint16_t dtime, cycleTime;
int freq[3] = {2200, 1500, 300};

int main(void)
{

	GPIO_Ini();
	while (1)
	{
		if ((READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_6) != 0))
		{
			reverse = !reverse;
			for (int i = 0; i < 500000; i++)
				;
		}
		if (reverse == 0)
		{
			if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_7) != 0)
			{
				count++;
				for (int i = 0; i < 500000; i++)
					;
				if (count > 6)
				{
					count = 0;
				}
			}
			ITR_init();		// инициализация прерываний
			RCC_Init();		// инициализация таймеров
			GPIO_Ini();		// инициализация портов
			SysTick_Init(); // инициализация системного таймера

			dtime = GlobalTickCount - cycleTime;
			cycleTime = GlobalTickCount;

			if (LongBtnCount1 >= 4 || LongBtnCount1 == 0)
			{
				LongBtnCount1 = 1;
			}

			ledTime1 = ledTime1 + dtime;
			if (ledTime1 >= freq[LongBtnCount1 - 1])
			{
				flag1ON = !flag1ON;
				ledTime1 = 0;
			}
			if (flag1ON)
			{
				if (BtnCount1 == 1)
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);

				if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) != 0)
				{
					count--;
					for (int i = 0; i < 500000; i++)
						;
					if (count > 6)
					{
						count = 0;
					}
				}
			}
			if (reverse == 1)
			{
				if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) != 0)
				{
					count++;
					for (int i = 0; i < 500000; i++)
						;
					if (count > 6)
					{
						count = 0;
					}
				}
				else
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);

				if (BtnCount1 == 2)
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);

				if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_7) != 0)
				{
					count--;
					for (int i = 0; i < 500000; i++)
						;
					if (count > 6)
					{
						count = 0;
					}
				}
			}
			else
				SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);

			if (flag == 1)
			{
				switch (count)
				{
				case 0:
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR5 | GPIO_BSRR_BR13 | GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
					SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR7);
					SET_BIT(GPIOС->BSRR, GPIOC_PIN6_R);
					break;
				case 1:
					SET_BIT(GPIOС->BSRR, GPIOC_PIN6_S);
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR5 | GPIO_BSRR_BR13 | GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
					SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR7);
					break;
				case 2:
					SET_BIT(GPIOС->BSRR, GPIOC_PIN6_S);
					SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS7);
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR5 | GPIO_BSRR_BR13 | GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
					break;
				case 3:
					SET_BIT(GPIOС->BSRR, GPIOC_PIN6_S);
					SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS7);
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS5);
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13 | GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
					break;
				case 4:
					SET_BIT(GPIOС->BSRR, GPIOC_PIN6_S);
					SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS7);
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS5 | GPIO_BSRR_BS13);
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
					break;
				case 5:
					SET_BIT(GPIOС->BSRR, GPIOC_PIN6_S);
					SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS7);
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS5 | GPIO_BSRR_BS13 | GPIO_BSRR_BS0);
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR1);
					break;
				case 6:
					SET_BIT(GPIOС->BSRR, GPIOC_PIN6_S);
					SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS7);
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS5 | GPIO_BSRR_BS13 | GPIO_BSRR_BS0 | GPIO_BSRR_BS1);
					break;
				}
				if (BtnCount1 == 3)
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);

				else
					SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
			}
		}
	}