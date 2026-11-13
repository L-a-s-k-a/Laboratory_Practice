#ifndef LAB1_INIT_H
#define LAB1_INIT_H

#include <stdbool.h>
#include <stdint.h>

void GPIO_Ini(void);
void GPIO_SetSharedOutput(bool output);
bool GPIO_ReadInputButton(void);
bool GPIO_ReadOutputButton(void);
bool GPIO_ReadSharedButton(void);
void GPIO_SetIndicators(bool red, bool blue, bool orange);

#endif
