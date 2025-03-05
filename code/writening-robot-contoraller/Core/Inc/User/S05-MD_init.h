#ifndef __SERVO_INIT_H
#define __SERVO_INIT_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_uart.h"
#include "stdio.h"

void USART1_IRQHandler (void);
void usart_senddata(char kk);
int fputc(int ch,FILE *f);

#endif 

