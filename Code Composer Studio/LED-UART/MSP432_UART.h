/*!
 * \file    MSP432_UART.h
 * \brief   Header to handle the uart on the MSP432
 * \author  Olivier Herrou
 * \version 1.0
 * \date    22 Juin 2015
 */
#include "msp.h"

void UART0_Init(void);
void Puts(unsigned char *s);
void Putc(unsigned char byte);
