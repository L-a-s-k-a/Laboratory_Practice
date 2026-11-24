#ifndef LAB2_IT_HANDLERS_H
#define LAB2_IT_HANDLERS_H

#include <stdint.h>

#define BUTTON_MODE   (1U << 0U)
#define BUTTON_SPEED  (1U << 1U)
#define BUTTON_SELECT (1U << 2U)

extern volatile uint32_t system_ms;


uint32_t Buttons_Pressed(void); // возвращает маску новых нажатий после подавления дребезга

#endif
