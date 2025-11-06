#include "task1.h"

void Delay(int t){
    for (int i = 0; i < t; i++){
        for (int j = 0; j < t; j++){
        }
    }
}
int DelayVal = 100;
void GenRow(void){
    void (*ledSet[3])(int) = { peSet, pcSet, pdSet };
    void (*ledReset[3])(int) = { peReset, pcReset, pdReset };
    int ledPins[3] = { PE5, PC1, PD1 };
    
    int cursor = 0;          // текущий светодиод
    int ledsOn = 1;          // количество одновременно включенных светодиодов
    const int LED_length = 3;


    while(1) {
        // перемещение курсора
        if(!readPC(PC11)) {
            Delay(DelayVal);
            cursor = (cursor + 1) % LED_length;
	    while(!readPC(PC11)){
		Delay(DelayVal);
		}
            Delay(DelayVal);
        }

        // увеличение числа включаемых светодиодов
        if(!readPC(PC12)) {
            Delay(DelayVal);
            if(ledsOn < LED_length) ledsOn++;
            else ledsOn = 0;// анти-дребезг
		 while(!readPC(PC12)){
		Delay(DelayVal);
		}
            Delay(DelayVal);
        }

        // Сбрасываем все светодиоды
        for(int i=0;i<LED_length;i++) {
            ledReset[i](ledPins[i]);
        }

        // Включаем нужные светодиоды с учётом курсора
        for(int i=0;i<ledsOn;i++){
            int index = (cursor + i) % LED_length;
            ledSet[index](ledPins[index]);
        }
    }
}

void GenFreq(void){
    void (*ledSet[3])(int) = { peSet, pcSet, pdSet };
    void (*ledReset[3])(int) = { peReset, pcReset, pdReset };
    int ledPins[3] = { PE5, PC1, PD1 };
    
    int cursor = 0;          // текущий светодиод
    int frequencyState[3] = {0, 0, 0};  // состояния частоты для каждого светодиода: 0, 1, 2
    int periodValues[3] = {10000000, 5000000, 2000000};  // периоды мерцания для разных частот (мс)
    
    const int LED_length = 3;
    int time = 0;           // глобальное время
    int ledTimers[3] = {0, 0, 0}; // таймеры для каждого светодиода
    int ledStates[3] = {0, 0, 0}; // состояния каждого светодиода

    while(1) {
        Delay(DelayVal);
        time += DelayVal*DelayVal;
        
        // перемещение курсора
        if(!readPC(PC11)) {
            Delay(DelayVal);
            cursor = (cursor + 1) % LED_length;
            while(!readPC(PC11)) {
                Delay(DelayVal);
                time += DelayVal*DelayVal;
            }
            Delay(DelayVal);
            time += DelayVal*DelayVal;
        }

        // изменение частоты мерцания для выбранного светодиода
        if(!readPC(PC12)) {
            Delay(DelayVal);
            time += DelayVal*DelayVal;
            frequencyState[cursor] = (frequencyState[cursor] + 1) % 3;
            while(!readPC(PC12)) {
                Delay(DelayVal);
                time += DelayVal*DelayVal;
            }
            Delay(DelayVal);
            time += DelayVal*DelayVal;
        }

        // Обновление таймеров и мерцание для каждого светодиода
        for(int i = 0; i < LED_length; i++) {
            ledTimers[i] += DelayVal;
            
            // Если прошло полпериода - переключаем состояние
            // Используем отдельную частоту для каждого светодиода
            if(ledTimers[i] >= periodValues[frequencyState[i]] / 2) {
                ledTimers[i] = 0; // сбрасываем таймер
                ledStates[i] = !ledStates[i]; // переключаем состояние
                
                if(ledStates[i]) {
                    ledSet[i](ledPins[i]);
                } else {
                    ledReset[i](ledPins[i]); 
                }
            }
        }
    }
}