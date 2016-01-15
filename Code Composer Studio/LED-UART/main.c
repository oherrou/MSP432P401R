/*!
 * \file    main.c
 * \brief   Control RGB led color with Hyperterminal via RS232 liaison
 * \author  Olivier Herrou
 * \version 1.0
 * \date    22 Juin 2015
 */
#include "msp.h"
#include <stdio.h>
#include "RGB.h"
#include "MSP432_UART.h"

void interface(unsigned char szData);

void main(void)
{
	
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
	
	/* ~~~> Clock <~~~ */
	CSKEY  = CSKEY_VAL;                 // Unlock CS module for register access
	CSCTL0 = 0;                         // Reset tuning parameters
	CSCTL0 = DCORSEL_3;                 // Set DCO to 12MHz (nominal, center of 8-16MHz range)
	CSCTL1 = SELA_2 | SELS_3 | SELM_3;  // Select ACLK = REFO, SMCLK = MCLK = DCO
	CSKEY  = 0;                         // Lock CS module from unintended accesses

	/* ~~~> Interrupt <~~~ */
	__enable_interrupt();
	NVIC_ISER0  = 1 << ((INT_EUSCIA0 - 16) & 31); // Enable eUSCIA0 interrupt in NVIC module
	NVIC_ISER0 |= 1 << ((INT_TA0_0 - 16) & 31); // TimerA0_0
	//NVIC_ISER1  = 1 << ((INT_PORT1 - 16) & 31); // Port1


/* ~~~> I/O <~~~ */
	/* ~~~> PORT1 <~~~ */
	P1DIR |= BIT0;  // LED1 output
	P1REN  = BIT1 + BIT4;  // activate pullup/down on SW1 and SW2
	P1OUT  = BIT1 + BIT4;  // pullup
	P1IE   = BIT1 + BIT4;  // interrupt enable SW1 and SW2
	P1IES  = BIT1 + BIT4;  // Low to high, must write because no defaut value ??
	P1IFG  = 0x00;	// no interruption pending

	RGB_Init();
	UART0_Init();

	RGB_SetPulse(2000,50);
	RGB_SetBrightness(100);
	RGB_SetColor(255,0,255);
}

void interface(unsigned char szData)
{
	if(szData >= 0x61 && szData <= 0x7A) // 'a' to 'z'
	{
		if(szData  == 'r')
		{
			RGB_SetColor(255,0,0);
			Puts("Color : RED\r");
		}
		if(szData  == 'g')
		{
			RGB_SetColor(0,255,0);
			Puts("Color : GREEN\r");
		}
		if(szData  == 'b')
		{
			RGB_SetColor(0,0,255);
			Puts("Color : BLUE\r");
		}
		if(szData  == 'a')
		{
			Puts("Bright : 100\r");
			RGB_SetBrightness(100);
		}
		if(szData  == 'z')
		{
			Puts("Bright : 35\r");
			RGB_SetBrightness(35);
		}
		if(szData  == 'e')
		{
			Puts("Bright : 5\r");
			RGB_SetBrightness(5);
		}
		if(szData  == 's')
		{
			Puts("RGB : Stop\r");
			RGB_SetBrightness(0);
		}
	}
}


/*!
 * \brief   Interrupt routine of PORT1
 *
 * \return None.
 */
void Port1Handler(void)
{
	uint16_t i = 0;
	if(P1IFG & BIT1) //SW1
	{
		RGB_ColorMod++;
		if(RGB_ColorMod > 2)
		{
			RGB_ColorMod = 0;
		}
		if(RGB_ColorMod == 0)
		{
		    RGB_SetColor(255,0,0);
		}
		if(RGB_ColorMod == 1)
		{
			RGB_SetColor(0,255,0);
		}
		if(RGB_ColorMod == 2)
		{
			RGB_SetColor(0,0,255);
		}
		P1IFG &= ~BIT1;
		for(i=0;i<30000;i++);  // Delay for bounce
	}
}

/*!
 * \brief   Interrupt routine of TimerA0_0
 *
 * \return None.
 */
void TimerA0_0IsrHandler(void)
{
	RGB_HandleColor();
	TA0CCTL0 &= ~CCIFG;
}

/*!
 * \brief   Interrupt routine of eUSCIA0
 *
 * \return None.
 */
 void eUSCIA0IsrHandler(void)
 {
	 if (UCA0IFG & UCRXIFG)
	 {
	   while(!(UCA0IFG&UCTXIFG));
	   interface(UCA0RXBUF);
	   //UCA0TXBUF = UCA0RXBUF;
	   __no_operation();
	 }
 }
