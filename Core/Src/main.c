
// // код на CMSIS
// #include "stm32f4xx.h"

// int main(void) {
    
//     SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    
//     MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER0 | GPIO_MODER_MODER7 | GPIO_MODER_MODER14, 
//                              GPIO_MODER_MODER0_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER14_0);

//     CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODER0 | GPIO_MODER_MODER2 | GPIO_MODER_MODER3);

//     MODIFY_REG(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR2 | GPIO_PUPDR_PUPDR3, 
//                              GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR3_1);

//     uint8_t port_mode = 0;

//     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS_7);  
//     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR_14); 

//     while (1) {
       
//         if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0)) {
//             port_mode = 0;
            
//             CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODER3); 
             
//             SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR_3); 
            
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS_7);   
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR_14);  
//         }
//         else{
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR_7);
//         }

//         if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_2)) {
//             port_mode = 1;
            
//             MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER3, GPIO_MODER_MODER3_0); 
            
//             SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS_3); 
            
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS_14);  
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR_7);   
//         }
//         else{
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR_14);
//         }
        
//         if (port_mode == 0) { 
            
//             if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_3)) {
//                 SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS_0);  
//             } else {
//                 SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR_0); 
//             }
//         } else { 
           
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR_0); 
//         }
//     }
// }


// код с прямым доступом к памяти
#include <stdint.h>

int main(void) {

    *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x06UL;


   
    *(uint32_t*)(0x40020400UL + 0x00UL) &= ~0x3000C003UL; 
    *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x10004001UL;  

    
    *(uint32_t*)(0x40020800UL + 0x00UL) &= ~0x000000F3UL; 

    
    *(uint32_t*)(0x40020800UL + 0x0CUL) &= ~0x000000F3UL; 
    *(uint32_t*)(0x40020800UL + 0x0CUL) |= 0x000000A2UL;  

    
    uint8_t port_mode = 0; 

    
    *(uint32_t*)(0x40020400UL + 0x18UL) = 0x00800000UL;  
    *(uint32_t*)(0x40020400UL + 0x18UL) = 0x40000000UL;  
    *(uint32_t*)(0x40020400UL + 0x18UL) = 0x00010000UL;   

 
    while(1) {
        
      
        if((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x01UL) != 0) {
            port_mode = 0; 
            
            // Переводим PC3 в режим Входа: сбрасываем биты 7:6 в GPIOC_MODER в 00
            *(uint32_t*)(0x40020800UL + 0x00UL) &= ~0x000000C0UL;
            
            // Сбрасываем внешний выходной уровень PC3 в 0 через BR_3 (1 << 19)
            *(uint32_t*)(0x40020800UL + 0x18UL) = 0x00080000UL; 
            
            // ИСПРАВЛЕНО: Включаем КРАСНЫЙ светодиод через BS_7 (1 << 7)
            *(uint32_t*)(0x40020400UL + 0x18UL) = 0x80UL; 
            // ИСПРАВЛЕНО: Гасим СИНИЙ светодиод через BR_14 (1 << 30)
            *(uint32_t*)(0x40020400UL + 0x18UL) = 0x40000000UL; 
        } else {
            // ИСПРАВЛЕНО: Если Кнопка 1 отпущена — гасим КРАСНЫЙ через BR_7 (1 << 23)
            *(uint32_t*)(0x40020400UL + 0x18UL) = 0x00800000UL; 
        }

       
        if((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x04UL) != 0) {
            port_mode = 1; 
            
            
            *(uint32_t*)(0x40020800UL + 0x00UL) &= ~0x000000C0UL; 
            *(uint32_t*)(0x40020800UL + 0x00UL) |= 0x00000040UL; 
            
            
            *(uint32_t*)(0x40020800UL + 0x18UL) = 0x00000008UL; 
            
            
            *(uint32_t*)(0x40020400UL + 0x18UL) = 0x4000UL; 
           
            *(uint32_t*)(0x40020400UL + 0x18UL) = 0x00800000UL; 
        } else {
           
            *(uint32_t*)(0x40020400UL + 0x18UL) = 0x40000000UL; 
        }

       
        if (port_mode == 0) { 
            // Режим ВХОДА: Опрашиваем состояние пина PC3 (Кнопка 3) в регистре IDR
            if((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x08UL) != 0) {
                // ИСПРАВЛЕНО: Если кнопка зажата — включаем ЖЕЛТЫЙ через BS_0 (1 << 0)
                *(uint32_t*)(0x40020400UL + 0x18UL) = 0x01UL; 
            } else {
                // ИСПРАВЛЕНО: Если отпущена — гасим ЖЕЛТЫЙ через BR_0 (1 << 16)
                *(uint32_t*)(0x40020400UL + 0x18UL) = 0x00010000UL; 
            }
        } else {
            // Режим ВЫХОДА: Внешний светодиод на PC3 уже горит.
            // ИСПРАВЛЕНО: Гасим ЖЕЛТЫЙ светодиод платы через BR_0 (1 << 16)
            *(uint32_t*)(0x40020400UL + 0x18UL) = 0x00010000UL; 
        }
    }
}





// ДОП ЗАДАНИЕ 

// #include "stm32f4xx.h"

// void delay_cycles(volatile uint32_t cycles) {
//     while (cycles--) {
//         __NOP();
//     }
// }

// typedef struct {
//     uint32_t pin_bs;
//     uint32_t pin_br;
//     uint32_t chastota_val; 
//     uint32_t timer_counter; 
//     uint8_t state;         
// } LED_t;

// int main(void) {
   
//     SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    
   
//     MODIFY_REG(GPIOB->MODER, 
//                GPIO_MODER_MODER0 | GPIO_MODER_MODER7 | GPIO_MODER_MODER14, 
//                GPIO_MODER_MODER0_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER14_0);

    
//     CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODER0 | GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
//     MODIFY_REG(GPIOC->PUPDR,
//                GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR2 | GPIO_PUPDR_PUPDR3,
//                GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR3_1);

   
//     LED_t leds[3] = {
//         {GPIO_BSRR_BS_0,  GPIO_BSRR_BR_0,  400000, 0, 0}, 
//         {GPIO_BSRR_BS_7,  GPIO_BSRR_BR_7,  400000, 0, 0}, 
//         {GPIO_BSRR_BS_14, GPIO_BSRR_BR_14, 400000, 0, 0}  
//     };

//     uint8_t active_led_idx = 0; 

//     while (1) {
        
//         if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0)) {
//             active_led_idx = (active_led_idx + 1) % 3;
//             delay_cycles(200000); 
//             while(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0));
//         }

        
//         if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_2)) {
//             if (leds[active_led_idx].chastota_val > 150000) {
//                 leds[active_led_idx].chastota_val -= 150000;
//             }
//             delay_cycles(200000);
//             while(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_2));
//         }

        
//         if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_3)) {
//             if (leds[active_led_idx].chastota_val < 800000) {
//                 leds[active_led_idx].chastota_val += 150000;
//             }
//             delay_cycles(200000);
//             while(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_3));
//         }

//         //  Независимое мигание ВСЕХ светодиодов 
//         for (uint8_t i = 0; i < 3; i++) {
//             leds[i].timer_counter++; 

//             if (leds[i].timer_counter >= leds[i].chastota_val) {
//                 leds[i].timer_counter = 0; 
                
//                 if (leds[i].state == 0) {
//                     leds[i].state = 1;
//                     WRITE_REG(GPIOB->BSRR, leds[i].pin_bs); 
//                 } else {
//                     leds[i].state = 0;
//                     WRITE_REG(GPIOB->BSRR, leds[i].pin_br); 
//                 }
//             }
//         }
//     }
// }




 
