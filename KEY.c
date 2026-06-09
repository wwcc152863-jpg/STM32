#include "KEY.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "OLED.h"
#include "tim.h"
extern TIM_HandleTypeDef htim2;
extern uint8_t mode;
struct keys key[KEY_MAX_NUMBER]; 
extern uint8_t water_level_min, temp_min, light_min, wtl_i, time_i;
extern struct timer residue_time;
extern uint8_t dev_i, water_flag, led_flag, oxy_flag, warm_flag, weishi_flag, time_flag, motor_flag;
uint8_t time_dev_switch;
void Key_Scan(void)
{
  if (key[0].key_flag == 1)
  {
		if (mode >= 3)
		 mode = 0;
		else
		 mode++;
		wtl_i = 0;
		dev_i = 0;
		time_i = 0;
		OLED_Clear();
    key[0].key_flag = 0;
  }
  else if (key[1].key_flag == 1)
  {
    if (mode == 1)
		{
			if (wtl_i == 2)
				wtl_i = 0;
			else
				wtl_i++;
		}
		else if (mode == 2)
		{
			if (dev_i == 2)
				dev_i = 0;
			else
				dev_i++;
		}
		else if (mode == 3)
		{
			if (time_i == 2)
				time_i = 0;
			else
				time_i++;
		}
    key[1].key_flag = 0;
  }
  else if (key[2].key_flag == 1)
  {
    if (mode == 1)
		{
			(wtl_i == 0) ? water_level_min++ : (wtl_i == 1) ? temp_min++ : light_min++;
		}
		else if (mode == 2)
		{
			 if (dev_i == 0)
				 water_flag = !water_flag;
			 else if (dev_i == 1)
				 warm_flag = !warm_flag;
			 else if (dev_i == 2)
				 led_flag = !led_flag;
		}
		else if (mode == 3)
		{
			if (time_dev_switch == 0)
			{
				if (time_i == 0)
					(residue_time.hour < 23) ? (residue_time.hour++) : (residue_time.hour = 0);
				else if (time_i == 1)
					(residue_time.minute < 59) ? (residue_time.minute++) : (residue_time.minute = 0);
				else if (time_i == 2)
					(residue_time.sec < 59) ? (residue_time.sec++) : (residue_time.sec = 0);
			}
			else
			{
				oxy_flag = !oxy_flag;
			}
		}
    key[2].key_flag = 0;
  }
  else if (key[3].key_flag == 1)
  {
   if (mode == 1)
		{
			(wtl_i == 0) ? ((water_level_min > 0) ?( water_level_min--) : (water_level_min = 0)) : (wtl_i == 1) ? ((temp_min > 0) ? temp_min-- : (temp_min = 0)) : ((light_min < 0) ? light_min-- : (light_min = 0));
		}
		else if (mode == 3)
		{
			if (time_dev_switch == 0)
			{
				if (time_i == 0)
					(residue_time.hour == 0) ? (residue_time.hour = 23) : (residue_time.hour--);
				else if (time_i == 1)
					(residue_time.minute == 0) ? (residue_time.minute = 59) : (residue_time.minute--);
				else if (time_i == 2)
					(residue_time.sec == 0) ? (residue_time.sec = 59) : (residue_time.sec--);
			}
			else
				motor_flag = !motor_flag;
		}
    key[3].key_flag = 0;
  }
	else if ((key[4].key_flag == 1) && (mode == 3))
	{
		if (time_dev_switch == 0)
			time_dev_switch = 1;
		else if(time_dev_switch == 1)
		{
			HAL_TIM_Base_Start_IT(&htim3);
			time_flag = 1;
			time_dev_switch = 0;
		}
		key[4].key_flag = 0;
	}
  else
  {
    key[0].key_flag = 0;
    key[1].key_flag = 0;
    key[2].key_flag = 0;
    key[3].key_flag = 0;
		key[4].key_flag = 0;
  }
}




