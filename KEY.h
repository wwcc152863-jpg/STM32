#ifndef __KEY_H 
#define __KEY_H
#include "stm32f1xx_hal.h"
#define KEY_MAX_NUMBER    5
struct keys{
	uint8_t key_state;
	uint8_t key_flag;
	uint8_t key_value;
};



void Key_Scan(void);

#endif  /* __KEY_H */


