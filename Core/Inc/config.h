
#ifndef HW_CONFIG_H
#define HW_CONFIG_H

#define BOARD_Nucleo144_F429ZI
// #define BOARD_OTHER
#ifdef BOARD_Nucleo144_F429ZI
  #define PORT_SV       GPIOB
  #define NUM_GREEN      0
  #define NUM_RED       14
  #define NUM_BLUE      7
  // Порты для управления двигателем
  #define GPIO_1        GPIOA 
  #define Number1_GPIO  4
  #define GPIO_2        GPIOE
  #define Number2_GPIO  0
  #define CONST_UGOL    0.036

#elif defined(BOARD_OTHER)
  #define PORT_SV      GPIO
  #define NUM_GREEN    NUMs
  #define NUM_RED      NUM
  #define NUM_BLUE     NUM
   // Порты для управления двигателем
  #define GPIO_1        GPIO_ 
  #define Number1_GPIO  num
  #define GPIO_2        GPIO_ 
  #define Number2_GPIO  num
  #define CONST_UGOL    num
   
#else 
     #error "Ошибка компиляции: Не выбрана плата! Раскомментируйте один из макросов BOARD_..."
#endif


#endif