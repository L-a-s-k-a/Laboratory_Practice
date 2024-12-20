#include <init.h>
#include <it_handlers.h>
//btn port - PC0
uint16_t flik_list[3] = {2500, 1000, 526};
uint8_t led_en[6] = {0, 0, 0, 0, 0, 0};
uint8_t led_state_flik[6] = {0, 0, 0, 0, 0, 0};
uint32_t led_tick[6] = {0, 0, 0, 0, 0, 0};
// uint32_t led1_tick = 0;
// uint32_t led2_tick = 0;
// uint32_t led3_tick = 0;
uint8_t BtnCount, BtnCount2;
uint8_t mode = 0;
uint8_t shortState = 0;

void shine(){
    // if(led_en[0]) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
    // else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
    // if(led_en[1]) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
    // else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
    // if(led_en[2]) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
    // else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
    //PE2,4,5,6,3 PF8
    if(led_en[0]) SET_BIT(GPIOE->BSRR, GPIO_BSRR_BS2);
    else SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR2);
    if(led_en[1]) SET_BIT(GPIOE->BSRR, GPIO_BSRR_BS4);
    else SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR4);
    if(led_en[2]) SET_BIT(GPIOE->BSRR, GPIO_BSRR_BS5);
    else SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR5);
    if(led_en[3]) SET_BIT(GPIOE->BSRR, GPIO_BSRR_BS6);
    else SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR6);
    if(led_en[4]) SET_BIT(GPIOE->BSRR, GPIO_BSRR_BS3);
    else SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR3);
    if(led_en[5]) SET_BIT(GPIOF->BSRR, GPIO_BSRR_BS8);
    else SET_BIT(GPIOF->BSRR, GPIO_BSRR_BR8);    

}

void flickers(){
    // if(led_en[0]){
    //     if (led1_tick >= flik_list[led_state_flik[0]] / 2){// Если прошло 1000 прерываний системного таймера = 1 секунда
    //        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14); // Включаем светодиод
    //        if (led1_tick >= flik_list[led_state_flik[0]]){// Если прошло 2000 прерываний системного таймера = 2 секунды
    //             led1_tick = 0; // Обнуляем переменную счётчика системного таймера
    //         }
    //     }
    //     else{
    //         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14); // Иначе выключаем светодиод
    //     }
    // }
    // else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
    //     //blue(PB7)
    // if(led_en[1]){
    //     if (led2_tick >= flik_list[led_state_flik[1]] / 2){// Если прошло 1000 прерываний системного таймера = 1 секунда
    //        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); // Включаем светодиод
    //        if (led2_tick >= flik_list[led_state_flik[1]]){// Если прошло 2000 прерываний системного таймера = 2 секунды
    //             led2_tick = 0; // Обнуляем переменную счётчика системного таймера
    //         }
    //     }
    //     else{
    //         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); // Иначе выключаем светодиод
    //     }
    // }
    // else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
    // if(led_en[2]){
    //     //green(PB0)
    //     if (led3_tick >= flik_list[led_state_flik[2]] / 2){// Если прошло 1000 прерываний системного таймера = 1 секунда
    //        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0); // Включаем светодиод
    //        if (led3_tick >= flik_list[led_state_flik[2]]){// Если прошло 2000 прерываний системного таймера = 2 секунды
    //             led3_tick = 0; // Обнуляем переменную счётчика системного таймера
    //         }
    //     }
    //     else{
    //         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0); // Иначе выключаем светодиод
    //     }
    // }
    // else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
    //PE2
    if(led_en[0]){
        if (led_tick[0] >= flik_list[led_state_flik[0]] / 2){// Если прошло 1000 прерываний системного таймера = 1 секунда
           SET_BIT(GPIOE->BSRR, GPIO_BSRR_BS2); // Включаем светодиод
           if (led_tick[0] >= flik_list[led_state_flik[0]]){// Если прошло 2000 прерываний системного таймера = 2 секунды
                led_tick[0] = 0; // Обнуляем переменную счётчика системного таймера
            }
        }
        else{
            SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR2); // Иначе выключаем светодиод
        }
    }
    else SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR2);
    //PE4
    if(led_en[1]){
        if (led_tick[1] >= flik_list[led_state_flik[1]] / 2){
           SET_BIT(GPIOE->BSRR, GPIO_BSRR_BS4); 
           if (led_tick[1] >= flik_list[led_state_flik[1]]){
                led_tick[1] = 0;
            }
        }
        else{
            SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR4); 
        }
    }
    else SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR4);
    //PE5    //PE2,4,5,6,3 PF8
    if(led_en[2]){
        if (led_tick[2] >= flik_list[led_state_flik[2]] / 2){
           SET_BIT(GPIOE->BSRR, GPIO_BSRR_BS5); 
           if (led_tick[2] >= flik_list[led_state_flik[2]]){
                led_tick[2] = 0; 
            }
        }
        else{
            SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR5); 
        }
    }
    else SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR5);
    //PE6    //PE2,4,5,6,3 PF8
    if(led_en[3]){
        if (led_tick[3] >= flik_list[led_state_flik[3]] / 2){
           SET_BIT(GPIOE->BSRR, GPIO_BSRR_BS6); 
           if (led_tick[3] >= flik_list[led_state_flik[3]]){
                led_tick[3] = 0; 
            }
        }
        else{
            SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR6); 
        }
    }
    else SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR6);
    //PE5    //PE2,4,5,6,3 PF8
    if(led_en[4]){
        if (led_tick[4] >= flik_list[led_state_flik[4]] / 2){
           SET_BIT(GPIOE->BSRR, GPIO_BSRR_BS3); 
           if (led_tick[4] >= flik_list[led_state_flik[4]]){
                led_tick[4] = 0; 
            }
        }
        else{
            SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR3); 
        }
    }
    else SET_BIT(GPIOE->BSRR, GPIO_BSRR_BR3);
    //PF8    //PE2,4,5,6,3 PF8
    if(led_en[5]){
        if (led_tick[5] >= flik_list[led_state_flik[5]] / 2){
           SET_BIT(GPIOF->BSRR, GPIO_BSRR_BS8); 
           if (led_tick[5] >= flik_list[led_state_flik[5]]){
                led_tick[5] = 0; 
            }
        }
        else{
            SET_BIT(GPIOF->BSRR, GPIO_BSRR_BR8); 
        }
    }
    else SET_BIT(GPIOF->BSRR, GPIO_BSRR_BR8);
    
    }
    

void incFlikState(int i){
    led_state_flik[i]++;
    if(led_state_flik[i] > 2) led_state_flik[i] = 0;
}

int main(void){
    GPIO_Ini();     // Инициализация поротв GPIO
    RCC_Ini();      // Инициализация системы тактирования RCC
    EXTI_ITR_Ini(); // Инициализация контроллера прерываний
    SysTick_Init(); // Инициализация системного таймера
    while (1){
        
       switch (BtnCount2){
            case 1: 
                led_en[0] = 1;
                if(shortState){ 
                    incFlikState(0);
                    shortState = 0; }
            break;    
            case 2: 
                led_en[1] = 1; 
                if(shortState){ 
                    incFlikState(1);
                    shortState = 0; }
            break;        
            case 3: 
                led_en[2] = 1; 
                if(shortState){ 
                    incFlikState(2);
                    shortState = 0; }
            break;
            case 4: 
                led_en[3] = 1; 
                if(shortState){ 
                    incFlikState(3);
                    shortState = 0; }
            break;
            case 5: 
                led_en[4] = 1; 
                if(shortState){ 
                    incFlikState(4);
                    shortState = 0; }
            break;
            case 6: 
                led_en[5] = 1; 
                if(shortState){ 
                    incFlikState(5);
                    shortState = 0; }
            break;
            case 7:
                led_en[0] = 0;
                led_en[1] = 0;
                led_en[2] = 0;
                led_en[3] = 0;
                led_en[4] = 0;
                led_en[5] = 0;
                BtnCount2 = 0;
            break;
            
        }
        if(mode) flickers(); 
        else shine(); 
    }
}

