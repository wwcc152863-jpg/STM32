#include "Motor.h"
#include "delay.h"
extern TIM_HandleTypeDef htim2;

uint8_t dri;
uint16_t count;
uint8_t flag;
void Motor_Stop(void)
{
  
}
void Motor_SetSpeed(uint8_t speed, uint8_t dri)
{
  
  if (dri == 0)
  {
    switch(speed)
    {
      case 0: 
        B1(0);
        B2(0);
        B3(1);
        B4(1);
      break;
      case 1:
        B1(0);
        B2(1);
        B3(1);
        B4(0);
      break;
      case 2:
        B1(1);
        B2(1);
        B3(0);
        B4(0);
      break;
      case 3:
        B1(1);
        B2(0);
        B3(0);
        B4(1);
      break;
    }
  }
  else
  {
    switch(speed)
    {
      case 0:
        B1(1);
        B2(0);
        B3(0);
        B4(1);
      break;
      case 1:
        B1(1);
        B2(1);
        B3(0);
        B4(0);
      break;
      case 2:
        B1(0);
        B2(1);
        B3(1);
        B4(0);
      break;
      case 3:
        B1(0);
        B2(0);
        B3(1);
        B4(1);
      break;
    }
  }
}
void MOTOR_Run(void)
{
    static uint8_t speed = 0;
    flag = 1;
    
    if (count >= 1000)
    {
      MOTOR_Stop();
      count = 0;
    }
    else
    {
      count++;
      speed++;
      if (speed > 3)
        speed = 0;
       Motor_SetSpeed(speed, dri);
    }
    
   
}

void MOTOR_Start(uint8_t direction)
{
  dri = direction;
  HAL_TIM_Base_Start_IT(&htim2);
}


void MOTOR_Stop(void)
{
  flag = 0;
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11 |GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0, GPIO_PIN_RESET);
  HAL_TIM_Base_Stop_IT(&htim2);
}





