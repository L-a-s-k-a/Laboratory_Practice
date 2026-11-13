#include "Driver.h"
#include <stddef.h>

/*Для начала работы с драйвером сперва необходимо сделать следующие действия:
    1) Выбрать,с помощью чего будут реализована функция задержки.
По умолчанию в драйвере используеться функция задержки на основе аппаратного счетчика ядра Cortex (Функции DWT_Init(),delay_us(),delay_ms()).
Если в ядре,которое используеться в вашем микроконтроллере,отсутствует такой аппаратный счетчик (например Cortex M0 и M0+),то необходимо реализовать свои функции задержки.

    2) Вызвать функцию инициализации для параметров мотора Motor_Init(uint32_t steps, float c_ugol, uint32_t rpm)
steps-количество шагов на один оборот,c_ugol-угол шага (в градусах),rpm-максимальная скорость вращения (в об/мин) 
(Например:Motor_Init(10000, 0.036, 3000);10000-шагов на оборот,0.036-за один шаг вал поворачивается на угол 0.036 градуса,3000-максимальное количество оборотов)

    3) Вызвать функцию  Sync_Timer_Params(TIM_TypeDef *TIMX, uint32_t base_freq)
где TIMX-указатель на таймер,base_freq-частота тактирования таймера,на которое пользователь настроил (В Гц)
(Например:Sync_Timer_Params(TIM2, 90000000U))
   
___Управление двигателем___
Вращение по часовой и против часовой стрелки осуществляется через функции:

    1) Rotation_OB(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t ob, uint32_t f_steps, uint32_t delay_time);
Скорость двигателя задается через об/мин (переменная ob).
TIMX-указатель на таймер,
CCR_reg-указатель на регистр сравнения таймера (например &TIM1->CCR4),
f_steps-количество шагов для плавного разгона (чем больше,тем плавнее разгон),
delay_time-задержка в миллисекундах между шагами разгона.
(Например:Rotation_OB(TIM1, &TIM1->CCR4, 2500, 500, 5);) (2500 об/мин)
ВАЖНО: Функция блокирует процессор на время разгона и оставляет мотор крутиться бесконечно на заданной скорости.

    2) Rotation_GC(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t f, uint32_t f_steps, uint32_t delay_time);
Аналог Rotation_OB,но скорость задается напрямую в Герцах (f).
(Например:Rotation_GC(TIM1, &TIM1->CCR4, 10000, 500, 5)-разгон до 10000 Гц за 500 шагов)

___Плавная остановка двигателя___
    1) Motor_Smooth_Stop_OB(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t ob, uint32_t f_steps, uint32_t delay_time);
Плавное торможение до заданной скорости (ob-об/мин).
Если ob=0,мотор плавно остановится полностью.
f_steps-количество шагов торможения,
delay_time-задержка между шагами торможения.

    2) Motor_Smooth_Stop_GC(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t target_f, uint32_t f_steps, uint32_t delay_time);
Аналог Motor_Smooth_Stop_OB,но целевая частота задается в Герцах.

___Управление по углу___
Эти функции позволяют мотору повернуться на заданный угол и автоматически остановиться.
Работают через прерывания таймера,не блокируя процессор!

    1) Rotation_UGL_OB(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t ob, float angle, uint32_t steps)
ob-скорость вращения (об/мин),
angle-целевой угол поворота (в градусах),
steps-количество шагов для плавного разгона/торможения.
Возвращает: 1-успешный запуск, 0-ошибка параметров.
(Например:Rotation_UGL_OB(TIM1, &TIM1->CCR4, 60, 180.0f, 500)-поворот на 180 градусов со скоростью 60 об/мин)

    2) Rotation_UGL_GC(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t f_target, float angle, uint32_t steps)
Аналог Rotation_UGL_OB,но скорость задается в Герцах.

    3) Rotation_UGL_IRQHandler(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg)
ОБЯЗАТЕЛЬНО вызывать в обработчике прерывания таймера!
Эта функция считает шаги,меняет частоту (разгон/торможение) и останавливает мотор по достижении угла.
(Например в main.c:
 void TIM2_IRQHandler(void) {
     Rotation_UGL_IRQHandler(TIM2, &TIM2->CCR4);
 })

ВАЖНО: После вызова Rotation_UGL_OB/GC необходимо ждать завершения движения:
while (param_ugl.sost_ugl_motor == 1) { }
или просто сделать паузу delay_ms(),но это менее точно.

___Вспомогательные функции___
    - Motor_Start(TIM_TypeDef *TIMX)-запустить таймер
    - Motor_Stop(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg)-мгновенная остановка мотора
    - Calculation_Freq(uint32_t freq)-расчет значения ARR для заданной частоты
    - DWT_Init()-инициализация счетчика циклов для задержек
    - delay_us(uint32_t us)-задержка в микросекундах
    - delay_ms(uint32_t ms)-задержка в миллисекундах

*/



 __attribute__((weak)) void delay_us(uint32_t us) {
   // Это версия по умолчанию (для Cortex-M3/M4/M7 с DWT)
     uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks) { __NOP(); }
}

 __attribute__((weak)) void delay_ms(uint32_t ms) {
     // Это версия по умолчанию
     for (uint32_t i = 0; i < ms; i++) { delay_us(1000U); }
 }
// Глобальные структуры для хранения состояния системы и параметров
Parametrs_Motor  param_m;
Parametrs_Driver param_d;
Parametrs_Now    param_n;
Parametrs_TIM    param_t;
Parametrs_UGL    param_ugl;
  
void DWT_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Разрешаем доступ к отладочным регистрам
    DWT->CYCCNT = 0;                                // Обнуляем счетчик циклов
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // Запускаем счетчик циклов DWT
}

void delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U); // Расчет количества тиков процессора для заданного времени
    while ((DWT->CYCCNT - start) < ticks) { __NOP(); }  // Ожидаем, пока счетчик не наберет нужное значение
}

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) { delay_us(1000U); } // Вызываем микросекундную задержку нужное количество раз
}

void Motor_Init(uint32_t steps, float c_ugol, uint32_t rpm) {
    // в этой функции пользователь вводит параметры двигателя
    param_m.const_steps = steps; // количество шагов на 1 оборот
    param_m.const_ugol = c_ugol; // угол микрошага
    param_m.max_rpm = rpm;       // максимальная скорость в оборотах в минуту
    
    // в блоке ниже приведены параметры, необходимые для корректной работы двигателя
    param_d.frequency = 0U;
    param_d.rpm = 0U;
    param_n.sost_mot = 0;        // Сброс статуса: мотор остановлен
    param_n.freq_now = 0;
}

uint32_t Calculation_Freq(uint32_t freq) {
    if (freq == 0U) return 0xFFFFU; // защита от частоты, равной нулю
    uint32_t tick_hz = param_t.freq_PROC / (param_t.psc + 1U); // расчет реальной частоты тиков таймера
    uint32_t period = (tick_hz + freq / 2U) / freq; // расчет периода с математическим округлением до ближайшего целого
    if (period < 2U) period = 2U; // защита от ошибки, если период arr=0 или 1 (таймер может зависнуть)
    uint32_t arr = period - 1U;   // расчет значения для регистра arr (счет идет с 0)
    
    // Аппаратная защита: обрезаем значение, если оно не влезает в 16-битный регистр таймера (TIM1, TIM3 и т.д.)
    if (arr > 0xFFFFU) {
        arr = 0xFFFFU;
    }
    return arr;
}

void Sync_Timer_Params(TIM_TypeDef *TIMX, uint32_t base_freq) {
    param_t.freq_PROC = base_freq; // значения частоты шины
    param_t.psc = TIMX->PSC;       // значения PSC, которое установил пользователь
}

void Rotation_GC(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t f, uint32_t f_steps, uint32_t delay_time) {
    uint64_t f_max = ((uint64_t)param_m.max_rpm * param_m.const_steps) / 60U;  // расчет максимальной частоты
    if (f == 0U || f > f_max) return; // защита от ошибки, если частота, заданная пользователем, превысит максимальную или будет равна 0
    
    uint32_t f_start;
    if (param_n.sost_mot == 1) {
        // этот алгоритм выполняется, если мотор уже запущен (плавное изменение скорости)
        f_start = param_d.frequency;
        Acceleration(TIMX, CCR_reg, f, f_start, f_steps, delay_time);
    } else {
        // начальный разгон мотора
        param_n.sost_mot = 1; // инициализация флага, что мотор запущен
        f_start = 250U;
        if (f < f_start) f_start = f; // защита от ошибки, если стартовая скорость меньше безопасной f_start
        
        uint32_t arr = Calculation_Freq(f_start); // расчет значения ARR для стартовой частоты 
        if (arr == 0U) arr = 1U; // защита от ошибки, если arr=0
        
        // применение настроек CCR и ARR
        TIMX->ARR = arr;
        *CCR_reg = (arr + 1U) / 2U;
        
        // Запуск двигателя и последующий разгон
        Motor_Start(TIMX);
        Acceleration(TIMX, CCR_reg, f, f_start, f_steps, delay_time);
    } 
}

void Rotation_OB(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t ob, uint32_t f_steps, uint32_t delay_time) {
    if ((ob == 0) || ob > param_m.max_rpm) return; // защита от ошибки, если скорость в об/мин больше максимальной
    uint32_t f = (uint32_t)(((uint64_t)ob * param_m.const_steps + 30U) / 60U); // расчет частоты через количество оборотов в минуту 
    Rotation_GC(TIMX, CCR_reg, f, f_steps, delay_time); 
}

void Acceleration(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t f, uint32_t f_st, uint32_t f_steps, uint32_t delay_time) {
    uint32_t arr;
    if (f_steps == 0U) f_steps = 1U; // защита от ошибки, если шаг разгона будет равен 0 (защита от деления на ноль)
    int64_t delta = (int64_t)f - (int64_t)f_st; // расчет разницы нужной частоты и стартовой
    
    for (uint32_t i = 1; i <= f_steps; i++) {
        uint32_t f_current = (uint32_t)((int64_t)f_st + (delta * (int64_t)i) / (int64_t)f_steps); // плавный разгон (линейная интерполяция)
        arr = Calculation_Freq(f_current); // расчет для ARR текущего значения частоты
        
        // изменение регистров ARR и CCR
        TIMX->ARR = arr;
        *CCR_reg = (arr + 1U) / 2U;
        
        // Не делаем задержку на последнем шаге, чтобы сразу выйти на целевую частоту без лишнего ожидания
        if (i < f_steps) delay_ms(delay_time);
    }
    param_n.freq_now = f;
    param_d.frequency = f; 
}

void Motor_Start(TIM_TypeDef *TIMX) {
    // функция для запуска таймера и применение текущих настроек 
    TIMX->EGR = (1U << 0);       // Принудительно обновляем регистры предзагрузки (бит UG)
    TIMX->CR1 |= (1U << 0);      // Включаем счетчик таймера (бит CEN)
}

void Motor_Stop(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg) {
    // остановка мотора
    TIMX->CR1 &= ~(1U << 0);     // Выключаем счетчик таймера (бит CEN)
    *CCR_reg = 0U;               // Обнуляем скважность ШИМ
    
    // Сброс глобальных параметров состояния
    param_d.frequency = 0U;
    param_d.rpm = 0U;
    param_n.sost_mot = 0;
    param_n.freq_now = 0;
}

void Motor_Smooth_Stop_GC(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t target_f, uint32_t f_steps, uint32_t delay_time) {
    if (f_steps == 0U) f_steps = 1U;
    
    // Плавно снижаем частоту от ТЕКУЩЕЙ (param_n.freq_now) до ЦЕЛЕВОЙ (target_f)
    // Если target_f = 0, мотор плавно остановится.
    Acceleration(TIMX, CCR_reg, target_f, param_n.freq_now, f_steps, delay_time);
    
    // Полная остановка таймера
    Motor_Stop(TIMX, CCR_reg);
}
void Motor_Smooth_Stop_OB(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t ob, uint32_t f_steps, uint32_t delay_time) {
    // 1. Защита: если об/мин равен 0, нет смысла тормозить
    if (ob == 0U) return;
    // 2. Защита: если мотор уже стоит, тормозить нечего
    if (param_n.sost_mot == 0) return;
    
    // 3. Переводим об/мин в Гц 
    uint32_t f_target_stop = (uint32_t)(((uint64_t)ob * param_m.const_steps + 30U) / 60U);
    
    // 4. Защита: если целевая частота остановки выше текущей — это не торможение, а разгон!
    if (f_target_stop >= param_n.freq_now) return;
    
    // 5. Вызываем существующую функцию плавного торможения
    // Она плавно снизит частоту от param_n.freq_now до f_target_stop, а потом выключит мотор
    Motor_Smooth_Stop_GC(TIMX, CCR_reg, f_target_stop, f_steps, delay_time);
}

uint8_t Rotation_UGL_GC(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t f_target, float angle, uint32_t steps) {
    if (f_target == 0 || steps == 0 || angle <= 0.0f) { // защита от ошибки, если переданные параметры некорректны
        return 0;
    }
    
    // Расчет целевого количества шагов на основе угла и угла микрошага 
    param_ugl.steps_target = (uint32_t)(angle / param_m.const_ugol + 0.5f);
    if (param_ugl.steps_target == 0U) {
        return 0;
    }
    
    // Инициализация параметров для работы через прерывания
    param_ugl.steps = steps;
    param_ugl.f_target = f_target;
    param_ugl.steps_current = 0;
    param_ugl.sost_ugl_motor = 1;
    
    int32_t f_start = 1000U;         
    param_ugl.f_start = f_start;    
    
    uint32_t arr = Calculation_Freq(f_start);  // Расчет ARR для старта
    TIMX->ARR = arr;
    *CCR_reg = (arr + 1U) / 2U;
    
    TIMX->DIER |= TIM_DIER_UIE;      // Разрешаем прерывание по обновлению таймера (UIE)
    TIMX->SR &= ~TIM_SR_UIF;         // Очищаем флаг прерывания перед запуском, чтобы не было ложного срабатывания
    
    Motor_Start(TIMX);
    return 1;
}

uint8_t Rotation_UGL_OB(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t ob, float angle, uint32_t steps) {
    if (ob == 0U || ob > param_m.max_rpm) return 0; // Защита от некорректных оборотов
    uint32_t f = (uint32_t)(((uint64_t)ob * param_m.const_steps + 30U) / 60U); // Расчет частоты через об/мин
    return Rotation_UGL_GC(TIMX, CCR_reg, f, angle, steps);
}  

void Rotation_UGL_IRQHandler(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg) {
    // Проверяем, что прерывание вызвано именно событием обновления 
    if (TIMX->SR & TIM_SR_UIF) {
        TIMX->SR &= ~TIM_SR_UIF;         // Сбрасываем флаг прерывания 
        param_ugl.steps_current++;       // Увеличиваем счетчик сделанных шагов
        
        if (param_ugl.steps_current >= param_ugl.steps_target) {
            // Мотор достиг нужного угла — останавливаем
            Motor_Stop(TIMX, CCR_reg);       // Выключаем таймер
            TIMX->DIER &= ~TIM_DIER_UIE;     // Запрещаем прерывание, чтобы таймер больше не дергал процессор
            param_ugl.sost_ugl_motor = 0;    // Мотор свободен
            return;                          // Выходим из функции
        }
        
        uint32_t f_current;
        int64_t delta = (int64_t)param_ugl.f_target - (int64_t)param_ugl.f_start; // Разница между целевой и стартовой частотой
        
        if (param_ugl.steps_current <= param_ugl.steps) {
            // Расчет текущей частоты 
            f_current = (uint32_t)((int64_t)param_ugl.f_start + (delta * (int64_t)param_ugl.steps_current) / (int64_t)param_ugl.steps);
        } else {
            // Если фаза разгона закончена, держим целевую частоту до самого конца
            f_current = param_ugl.f_target;
        }
        
        uint32_t arr = Calculation_Freq(f_current); // Расчет ARR для текущей частоты
        TIMX->ARR = arr;                            // Применяем новую частоту
        *CCR_reg = (arr + 1U) / 2U;                 // Поддерживаем скважность 50%
    }
}