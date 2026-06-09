#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f1xx_hal.h"

#define B1(x)       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, (GPIO_PinState)x);
#define B2(x)       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, (GPIO_PinState)x);
#define B3(x)       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, (GPIO_PinState)x);
#define B4(x)       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, (GPIO_PinState)x);

void MOTOR_Start(uint8_t direction);
void MOTOR_Stop(void);
void MOTOR_Run(void);

#endif  /* __MOTOR_H */

