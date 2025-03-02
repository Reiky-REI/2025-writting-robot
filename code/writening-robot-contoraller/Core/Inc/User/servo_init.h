#ifndef __SERVO_INIT_H
#define __SERVO_INIT_H

#include "stm32f10x_conf.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stdio.h"

void USART1_IRQHandler (void);
void usart_senddata(char kk);
int fputc(int ch,FILE *f);

#endif 

