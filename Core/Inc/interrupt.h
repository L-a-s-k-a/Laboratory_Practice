#include <stdint.h>
#include <stdbool.h>

extern uint32_t msTicks;                    //счётчик времени в мс
extern uint8_t ledMode; // текущий режим работы светодиодов

extern uint8_t mode0_group_sel;         //выбранная группа
extern uint8_t mode0_group_freq[3];//частота мигания группы
extern uint32_t mode0_pair_tick;    // счётчик времени для рабочей пары
extern uint8_t  mode0_pair_index;   // индекс активной паары

extern uint8_t mode1_led_sel;       //выбранный светодиод
extern uint8_t mode1_led_freq[6];       //частоты мигания этих светодиодов
extern bool    mode1_led_active[6];     // активность светодиода
extern uint32_t mode1_led_ticks[6];     //счётчик времени светодиода
extern uint32_t staging_tick;       //таймер задержки включения
extern uint8_t  staging_index;      // индекс светодиода который врубаетсся

void SysTick_Handler(void);     //обработчик системного таймера
void EXTI4_IRQHandler(void);        //обработчик прервыний кнопки PC4
void EXTI15_10_IRQHandler(void);    // обработчик прерываний на двух оставшихся бутонах pc13 и pb12