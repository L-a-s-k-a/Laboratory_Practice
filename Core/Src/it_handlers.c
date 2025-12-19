#include "it_handlers.h"// Подключение файла с прототипами функций

extern volatile uint8_t led;//Объявление внешней переменной горящего светодиода, которая объявлена в главном файле       
extern volatile uint32_t GlobalTickCount;//Объявление внешней переменной счётчика милисекунд, которая объявлена в главном файле  
extern volatile uint8_t LedMode;//Объявление внешней переменной режима работы светодиода, которая объявлена в главном файле
extern volatile uint8_t LedChanged;//Объявление внешней переменной маркера смены горящего светодиода, которая объявлена в главном файле
extern volatile uint8_t FreqIndex;//Объявление внешней переменной индекса частоты мигания, которая объявлена в главном файле
extern volatile uint8_t FreqChanged;//Объявление внешней переменной маркера изменения частоты, которая объявлена в главном файле

#define DEBOUNCE_MS 100u  //Объявление периода антидребезга кнопки 
#define SHORT_PRESS_MS 2000u   // Объявление времени короткого нажатия, которое меньше 2 секунд
#define LONG_PRESS_MS 4000u    // Объявление времени длинного нажатия, которое больше 4 секунд

static volatile uint16_t ExternInterruptTickCount = 0; //Инициализация счетчика времени с последнего прерывания кнопки для устранения дребезга, видимая только в этом файле
static volatile uint8_t ButtonPressed = 0; //Инициализация переменной состояния кнопки, видимая только в этом файле
static volatile uint32_t ButtonPressTime = 0; //Инициализация счетчика, который считает сколько времени была нажата кнопка, видимая только в этом файле

void SysTick_Handler(void)//Обработчик прерывания сиситемного таймера SysTick
{
    GlobalTickCount++;//Увеличение глобального счётчика для отсчета времени 
    ExternInterruptTickCount++;//Увеличение счётчика для устранения дребезга кнопки
    
    if (ButtonPressed) { //Если кнопка нажата
        ButtonPressTime++; //Увеличение счетчика времени удержания на 1 мс
    }
}

void EXTI15_10_IRQHandler(void)//Обработчик внешнего прерывания для кнопки
{
    SET_BIT(EXTI->PR, EXTI_PR_PR13);//Сброс флага для кнопки на пине PC13

    if (ExternInterruptTickCount < DEBOUNCE_MS) return;//Если с последнего прерывания прошло менее 100 мс, то игнорируем нажатие
    ExternInterruptTickCount = 0;// Сброс счётчика времени с последнего нажатия на кнопку

    uint8_t CurrentButtonState = (READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) != 0);// Чтение текущего состояния кнопки, если нажата, то 1, иначе 0
    
    if (CurrentButtonState == 1 && ButtonPressed == 0)  //Если сейчас кнопка нажата, а до этого не была нажата
    {
        ButtonPressed = 1;//Установка маркера, что кнопка нажата
        ButtonPressTime = 0;  // Сброс счетчика времени
    }
    else if (CurrentButtonState == 0 && ButtonPressed == 1)  // Если сейчас кнопка не нажата, а до этого была, то есть отпущена
    {
        ButtonPressed = 0; // Сброс маркера нажатия кнопки
        
        if (ButtonPressTime < SHORT_PRESS_MS)  //Если кнопка нажата менее 2 с
        {
            led++;//Увеличение счетчика для смены горящего светодиода
            if (led > 6) led = 1;//Если счётчик вышел за рамки сузествующих светодиодов, то приравнять 1, то есть закольцевать
            LedChanged = 1;  //Изменение значения маркера, свидетельствующего, что светодиод поменялся
        }
        else if (ButtonPressTime < LONG_PRESS_MS)  // Если кнопка нажата меньше 4 секунд и больше 2
        {
            if (LedMode == 1)//Если режим светодиода мигание, то 
            {
                FreqIndex++;//Увеличение индекс частоты
                if (FreqIndex > 2) FreqIndex = 0;  // Если индекс выходит за массив периода, то присвоить знчение начала массива
                FreqChanged = 1;  // Устанавка маркера смены частоты мерцания
            }
        }
        else  //Иначе если нажатие длится более 4 секунд
        {
            LedMode = !LedMode;  //Переключениие режима горения на мерцани яи наоборот
            
            if (LedMode == 0 && led >= 1 && led <= 6)// Если стоит режим постоянного горения и счетчик светодиодов отображает значение входящее в диапазон
            {
                extern void SetLED(uint8_t led_num, uint8_t state);//Использование внешней функции для включения и выключения светодиодов
                SetLED(led, 1);//Включение текщего светодиода
            }
        }
        
        ButtonPressTime = 0;//Сброс счётчика времени удержания кнопки
    }
}

void EXTI_ITR_Init(void)// Функция инициализации внешних прерываний 
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);//Включение тактирования модуля SYSCFG на шине APB2 для использования регистров EXTI

    MODIFY_REG(SYSCFG->EXTICR[3], 
               SYSCFG_EXTICR4_EXTI13_Msk,
               SYSCFG_EXTICR4_EXTI13_PC);//Привязка линии EXTI13 к порту PC13 используя регистр SYSCFG_EXTICR4

    SET_BIT(EXTI->IMR, EXTI_IMR_MR13); // Разрешение прерывания от линии EXTI13 через регистр маски прерываний 
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13);    // Включение детектора прерываний по нарастанию
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR13);    // Включение детектора прерываний по спаду

    NVIC_SetPriority(EXTI15_10_IRQn,
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));//Установка максимального приоритета для прерывания EXTI15_10
    NVIC_EnableIRQ(EXTI15_10_IRQn);//Глобальное разрешение прерываний EXTI15_10
}

void SysTick_Init(void)//Функция инициализации системного таймера SysTick
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); //Отключение таймера для его конфигурации
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk); // Включение генерации прерывания при достижении нуля счётчиком SysTick
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);//Выбор источника тактирования 

    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 179999u << SysTick_LOAD_RELOAD_Pos);//Установка значения перезагрузки 179999, то есть таймер будт считать от 179999 до 0, то есть будет совершать один такт раз в милисекунду
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 0u);//Очистка текущего значения счётчика SysTick и сброс маркера COUNTFLAG

    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);//Запуск таймера 
}