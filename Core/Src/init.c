#include "init.h"
#define CONST_UGOL 0.036

void DWT_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; 
    DWT->CYCCNT = 0;                               
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            
}


void delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < ticks) {
        __NOP(); 
    }
}

// 3. Задержка в миллисекундах
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        delay_us(1000); 
    }
}

void INIT_SVETODIOD(){ 
     /*Синий*/
     PORT_SV->MODER &= ~(0x3U << (NUM_BLUE*2));
     PORT_SV->MODER |= (1U <<(NUM_BLUE*2));
     PORT_SV->OTYPER &=~(1U <<(NUM_BLUE));
     PORT_SV->OSPEEDR &= ~(0x3U << (NUM_BLUE*2));
     PORT_SV->OSPEEDR |=(2U <<(NUM_BLUE*2));
     PORT_SV->PUPDR &=~(0x3U<<(NUM_BLUE*2));
     //Зеленный
     PORT_SV->MODER &= ~(0x3U << (NUM_GREEN*2));
     PORT_SV->MODER |= (1U <<(NUM_GREEN*2));
     PORT_SV->OTYPER &=~(1U <<(NUM_GREEN));
     PORT_SV->OSPEEDR &= ~(0x3U << (NUM_GREEN*2));
     PORT_SV->OSPEEDR |=(2U <<(NUM_GREEN*2));
     PORT_SV->PUPDR &=~(0x3U<<(NUM_GREEN*2));
     //Красный
     PORT_SV->MODER &= ~(0x3U << (NUM_RED*2));
     PORT_SV->MODER |= (1U <<(NUM_RED*2));
     PORT_SV->OTYPER &=~(1U <<(NUM_RED));
     PORT_SV->OSPEEDR &= ~(0x3U << (NUM_RED*2));
     PORT_SV->OSPEEDR |=(2U <<(NUM_RED*2));
     PORT_SV->PUPDR &=~(0x3U<<(NUM_RED*2));
}
void INIT_UPRAVLENI_CCW(GPIO_TypeDef *port_ccw, uint8_t n2){ 
     // CCW
     port_ccw->MODER &= ~(0x3U << n2*2);
     port_ccw->MODER |= (1U <<n2*2);
     port_ccw->OTYPER &=~(1U <<n2);
     port_ccw->OSPEEDR &= ~(0x3U << n2*2);
     port_ccw->OSPEEDR |=(2U <<n2*2);
     port_ccw->PUPDR &=~(0x3U<<n2*2);  

}
void INIT_UPRAVLENI_CW( GPIO_TypeDef *port_cw ,uint8_t n1){
     // CW
     port_cw->MODER &= ~(0x3U << n1*2);
     port_cw->MODER |= (1U <<n1*2);
     port_cw->OTYPER &=~(1U <<n1);
     port_cw->OSPEEDR &= ~(0x3U << n1*2);
     port_cw->OSPEEDR |=(2U <<n1*2);
     port_cw->PUPDR &=~(0x3U<<n1*2);  
}
 //Для запуска светодиода необходимо записать его цвет G,B или R 
 void SET_SVETODIOD(char svet){
    if(svet=='G'){
          PORT_SV->BSRR=(1U <<NUM_GREEN);
     }
     else if (svet=='B'){
          PORT_SV->BSRR=(1U <<NUM_BLUE);
     }
     else if(svet=='R'){
          PORT_SV->BSRR=(1U <<NUM_RED);
     }
 }
 void RESET_SVETODIOD(char svet){
     if(svet=='G'){
          PORT_SV->BSRR=(1U <<(NUM_GREEN+16));
     }
     else if (svet=='B'){
          PORT_SV->BSRR=(1U <<(NUM_BLUE+16));
     }
     else if(svet=='R'){
          PORT_SV->BSRR=(1U <<(NUM_RED+16));
     }
     
 }
void MoveCW(GPIO_TypeDef *port_cw, uint8_t n, uint32_t steps, uint32_t min_time){
      uint32_t start_time=2000;
     uint16_t raznosti=100;
    
     for(uint32_t i=0;i<steps;i++){
          port_cw->BSRR =(1U <<n);
          delay_us(2);
          port_cw->BSRR =(1U <<(n+16));
          if(start_time >= min_time){
               start_time=start_time-raznosti;
          }
          delay_us(start_time);

     }
  }

void MoveCCW(GPIO_TypeDef *port_ccw, uint8_t n, uint32_t steps, uint32_t min_time ){
     uint32_t start_time=2000;
     uint16_t raznosti=100;
    
     for(uint32_t i=0;i<steps;i++){
          port_ccw->BSRR =(1U <<n);
          delay_us(2);
          port_ccw->BSRR =(1U <<(n+16));
          if(start_time >= min_time){
               start_time=start_time-raznosti;
          }
          delay_us(start_time);

     }
  }
void POVOROT_UGL(GPIO_TypeDef *port_ccw, uint8_t n, uint16_t ugol){
     uint32_t steps= (ugol/CONST_UGOL);
     uint16_t start_time=2000;
     uint8_t raznosti=100;
     for(uint32_t i=0;i<steps;i++){
          port_ccw->BSRR =(1U <<n);
          delay_us(2);
          port_ccw->BSRR =(1U <<(n+16));
          if(start_time >= 700){
               start_time=start_time-raznosti;
          }
          delay_us(start_time);

     }
}

void OSTANOVKA(GPIO_TypeDef *port_ccw, uint8_t n){
     port_ccw->BSRR =(1U <<(n+16));
}

