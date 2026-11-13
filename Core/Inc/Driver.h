#ifndef DRIVER_H
#define DRIVER_H

#include "init.h"
#include <stdint.h>

typedef struct {
    uint32_t freq_now;
    uint8_t  sost_mot;
} Parametrs_Now;

typedef struct {
    uint32_t const_steps;
    float    const_ugol;
    uint32_t max_rpm;
} Parametrs_Motor;

typedef struct {
    uint32_t frequency;
    uint32_t rpm;
} Parametrs_Driver;

typedef struct {
    uint32_t freq_PROC; 
    uint32_t psc;       
} Parametrs_TIM;
typedef struct{
    volatile uint8_t sost_ugl_motor;
    uint32_t steps_current;
    uint32_t steps_target;
    uint32_t f_target;
    uint32_t steps;
    uint32_t f_start; 

} Parametrs_UGL;
extern Parametrs_Motor  param_m;
extern Parametrs_Driver param_d;
extern Parametrs_Now    param_n;
extern Parametrs_TIM    param_t; 
extern Parametrs_UGL    param_ugl;

void Motor_Init(uint32_t steps, float c_ugol, uint32_t rpm);

uint32_t Calculation_Freq(uint32_t freq);
void Rotation_OB(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t ob, uint32_t f_steps, uint32_t delay_time);
void Rotation_GC(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t f, uint32_t f_steps, uint32_t delay_time);
void Acceleration(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t f, uint32_t f_st, uint32_t f_steps, uint32_t delay_time);
void Motor_Start(TIM_TypeDef *TIMX);
void Motor_Stop(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg);
void Motor_Smooth_Stop_GC(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t current_f, uint32_t f_steps, uint32_t delay_time);
void Motor_Smooth_Stop_OB(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t ob, uint32_t f_steps, uint32_t delay_time);
void Sync_Timer_Params(TIM_TypeDef *TIMX, uint32_t base_freq);
uint8_t Rotation_UGL_GC(TIM_TypeDef *TIMX,volatile uint32_t *CCR_reg,uint32_t f_target, float angle,uint32_t steps);
void Rotation_UGL_IRQHandler(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg);
uint8_t Rotation_UGL_OB(TIM_TypeDef *TIMX, volatile uint32_t *CCR_reg, uint32_t ob, float angle, uint32_t steps);

#endif /* DRIVER_H */