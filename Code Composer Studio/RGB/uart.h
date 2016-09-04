/*!
 * \file    uart.h
 * \author  Olivier Herrou
 * \version 1.0
 * \date    01 Sept 2016
 */
#include "msp432.h"

void UART0_Init(void);
void Puts(unsigned char *s);
void Putc(unsigned char byte);
