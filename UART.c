#include "UART.h"


extern UART_HandleTypeDef huart1;

uint8_t uart1_Buff[RXBUFF_MAX_SIZEOF];
uint16_t uart1_i;
uint8_t rxdata1;



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    uart1_Buff[uart1_i++] = rxdata1;
    HAL_UART_Receive_IT(&huart1, &rxdata1, 1);
    if (uart1_i >= RXBUFF_MAX_SIZEOF)
    {
      uart1_i = 0;
    }
  }
}





