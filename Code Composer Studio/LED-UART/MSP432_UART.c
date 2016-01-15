/*!
 * \file    MSP432_UART.c
 * \brief   Contains the functions to handle the uart on the MSP432
 * \author  Olivier Herrou
 * \version 1.0
 * \date    22 Juin 2015
 */

#include "MSP432_UART.h"

/*!
 * \brief   Function to initialize the uart communication with eUSCI_A0
 *
 * \remarks	* BAUD rate : 9600
 *			* No parity
 *			* 8 data bits
 *			* 1 stop bit
 * \return  None.
 */
void UART0_Init()
{
	// Configure UART pins
	P1DIR  |= BIT0;
	P1SEL0 |= BIT2 | BIT3;                  // set 2-UART pin as second function

	// Configure UART
	UCA0CTLW0 |= UCSWRST;  // Put eUSCI in reset
	UCA0CTLW0 |= UCSSEL__SMCLK;
	// Baud Rate calculation
	// 12000000/(16*9600) = 78.125
	// Fractional portion = 0.125
	// User's Guide Table 21-4: UCBRSx = 0x10
	// UCBRFx = int ( (78.125-78)*16) = 2
	UCA0BR0 = 78;                           // 12000000/16/9600
	UCA0BR1 = 0x00;
	UCA0MCTLW = 0x1000 | UCOS16 | 0x0020;

	UCA0CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
	UCA0IE |= UCRXIE;                       // Enable USCI_A0 RX interrupt
	Puts("-> UART LIAISON : INITIALIZE\r");
	//sendByte('\n');
}

void Puts(unsigned char *s)
{
	while(*s)
	{
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = *(s++);
	}
}


/*!
 * \brief   Function to send a single byte via uart
 *
 * \param   byte  the byte to send
 * \return  None.
 */
void Putc(unsigned char byte)
{
	while (!(UCA0IFG & UCTXIFG));			// USCI_A0 TX buffer ready?
	UCA0TXBUF = byte;
	__no_operation();
}
