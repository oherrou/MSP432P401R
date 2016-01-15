/*!
 * \file    blink.c
 * \brief   Control your RGB led brigthness, color and pulsation
 Use SW1 and SW2 to change color/brightness 

 - Interruption timerA0
 - Interruption timerAN 
 - Interruption Port1
Remark :
 Brightness is not perfect since some color components are repeated during phases
 
 * \author  Olivier Herrou
 * \version 1.0
 * \date    22 Juin 2015
 */
//****************************************************************************

#include "msp.h"
#include <stdio.h>

#define RGB1_COLORSCALE 30
#define RGB1_BRIGHTWIDTH 50
volatile uint16_t RGB1_PulseWidth;  //in ms
volatile uint16_t RGB1_PulseHigh;	// 0 - 100
volatile uint16_t RGB1_BrightHigh;	// 0 - 100
volatile uint16_t RGB1_SatRed; 		// 0 - 20 different level of red
volatile uint16_t RGB1_SatGreen; 	// 0 - 20 different level of green
volatile uint16_t RGB1_SatBlue; 	// 0 - 20 different level of blue
volatile uint16_t RGB1_State;		// 0 Off, 1 On
volatile uint16_t RGB1_BrightMod;	//
volatile uint16_t RGB1_ColorMod;	//
void myDelayMs(void);

void RGB_SetColor(uint16_t iRed, uint16_t iGreen, uint16_t iBlue, uint16_t iBrightnessLvl); // OK
void RGB_SetPulse(uint16_t iDelayMs); // WIP
void RGB_HandleColor(void);

void RGB1_SetPulse(uint16_t iPulseWidth, uint16_t iCycleHigh);
void RGB1_SetBrightness(uint16_t iCycleHigh);
void RGB1_SetColor(uint16_t iRed, uint16_t iGreen, uint16_t iBlue);
void RGB1_HandleColor(void);

void main(void)
{
	/* ~~~> Global var <~~~ */
	RGB1_SatRed 	= 0;
	RGB1_SatGreen   = 0;
	RGB1_SatBlue	= 0;
	RGB1_BrightHigh = 0;
	RGB1_PulseWidth = 0;
	RGB1_PulseHigh  = 0;
	RGB1_State   	= 1;
	RGB1_BrightMod  = 0;
	RGB1_ColorMod	= 0;

	/* ~~~> Watchdog <~~~ */
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    /* ~~~> Clock <~~~ */
    CSKEY  = CSKEY_VAL;                 // Unlock CS module for register access
    CSCTL0 = 0;                         // Reset tuning parameters
    CSCTL0 = DCORSEL_3;                 // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CSCTL1 = SELA_2 | SELS_3 | SELM_3;  // Select ACLK = REFO, SMCLK = MCLK = DCO
    CSKEY  = 0;                         // Lock CS module from unintended accesses

    /* ~~~> Interrupt <~~~ */
    __enable_interrupt();
    NVIC_ISER0  = 1 << ((INT_TA0_0 - 16) & 31); // TimerA0_0
    NVIC_ISER0 |= 1 << ((INT_TA0_N - 16) & 31); // TimerA0_N
    NVIC_ISER1  = 1 << ((INT_PORT1 - 16) & 31); // Port1

/* ~~~> I/O <~~~ */
    /* ~~~> PORT1 <~~~ */
    P1DIR |= BIT0;  // LED1 output
    P1REN  = BIT1 + BIT4;  // activate pullup/down on SW1 and SW2
    P1OUT  = BIT1 + BIT4;  // pullup
    P1IE   = BIT1 + BIT4;  // interrupt enable SW1 and SW2
    P1IES  = BIT1 + BIT4;  // Low to high, must write because no defaut value ??
    P1IFG  = 0x00;	// no interruption pending

    /* ~~~> PORT2 <~~~ */
    P2DIR |= BIT0 + BIT1 + BIT2; // RGB led output
    P2OUT  = 0x00;

    /* ~~~> TIMERA <~~~ */
    TA0CTL   = TASSEL_2 + MC_1 + TACLR; // SMCLK, UP, clear
    TA0CCR0  = 120; // Pour SMCLK à 12MhZ, on aura 100us de bip
    TA0CCTL0 = CCIE;  // Interrupt enable on CCIE

    /* ~~~> Main <~~~ */

    RGB1_SetPulse(2000,50);
    RGB1_SetBrightness(100);
    RGB1_SetColor(255,0,255);
    while(1);
}

/*!
 * \brief  Function to set the pulse of the rgb led
 *
 * \param  iPulseWidth  width of the pulse
 * \param  iCycleHigh   percentage at high level
 * \return None.
 */
void RGB1_SetPulse(uint16_t iPulseWidth, uint16_t iCycleHigh)
{
	RGB1_PulseWidth = iPulseWidth;
	RGB1_PulseHigh  = iCycleHigh;
}

/*!
 * \brief  Function to set the brightness parameter of the rgb led
 *
 * \param  iCycleHigh   percentage at high level
 * \return None.
 */
void RGB1_SetBrightness(uint16_t iCycleHigh)
{
	RGB1_BrightHigh  = iCycleHigh;
}

/*!
 * \brief   Function to set the color of the RGB led
 *
 * \param   iRed   			the red saturation   (0 - 255)
 * \param   iGreen 			the green saturation (0 - 255)
 * \param   iBlue  			the blue saturation  (0 - 255)
 * \return None.
 */
void RGB1_SetColor(uint16_t iRed, uint16_t iGreen, uint16_t iBlue)
{
	/* ~~~> Saturation <~~~ */
	if(iRed   > 255) iRed   = 255;
	if(iGreen > 255) iGreen = 255;
	if(iBlue  > 255) iBlue  = 255;

	/* ~~~> Assignment <~~~ */
	RGB1_SatRed     = ((iRed   * RGB1_COLORSCALE) / 255);
	RGB1_SatGreen   = ((iGreen * RGB1_COLORSCALE) / 255);
	RGB1_SatBlue    = ((iBlue  * RGB1_COLORSCALE) / 255);
}

/*!
 * \brief   Function to handle the rgb led
 *
 * \return None.
 */
void RGB1_HandleColor(void)
{
	static uint16_t iModLED    = 0;
	static uint16_t iModBright = 0;
	static uint16_t iPulse 	   = 0;

	if(RGB1_State)
	{
		/* ~~~> Pulse <~~~ */
		if(iPulse == RGB1_PulseWidth) iPulse = 0;
		if(iPulse < (RGB1_PulseWidth * (RGB1_PulseHigh/ 100.0)) ) // LED is on
		{
			/* ~~~> BRIGHTNESS <~~~ */
			if(iModBright == RGB1_BRIGHTWIDTH) iModBright = 0;
			if(iModBright < (RGB1_BRIGHTWIDTH * (RGB1_BrightHigh/ 100.0)) ) // LED is on
			{
				/* ~~~> Reset Cycle <~~~ */
				if(iModLED == RGB1_COLORSCALE) iModLED = 0;

				/* ~~~> Red LED <~~~ */
				if(iModLED < RGB1_SatRed) P2OUT |= BIT0;
				else P2OUT &=~ BIT0;

				/* ~~~> Green LED <~~~ */
				if(iModLED < RGB1_SatGreen) P2OUT |= BIT1;
				else P2OUT &=~ BIT1;

				/* ~~~> Blue LED <~~~ */
				if(iModLED <  RGB1_SatBlue) P2OUT |= BIT2;
				else P2OUT &=~ BIT2;

				iModLED++;
			}
			else
			{
				P2OUT = 0x00;
				iModLED = 0;
			}

			iModBright++;
		}
		else
		{
			P2OUT = 0x00;
			iModLED    = 0;
			iModBright = 0;
		}

		iPulse++;
	}
	else
	{
		iModLED    = 0;
		iModBright = 0;
		//iPulse 	   = 0;
	}
}

/*!
 * \brief   Interrupt routine of PORT1
 *
 * \return None.
 */
void Port1Handler(void)
{
	static uint16_t i = 0;
	if(P1IFG & BIT1) //SW1
	{
		RGB1_ColorMod++;
		if(RGB1_ColorMod > 2)
		{
			RGB1_ColorMod = 0;
		}
		if(RGB1_ColorMod == 0)
		{
		    RGB1_SetColor(255,0,0);
		}
		if(RGB1_ColorMod == 1)
		{
			RGB1_SetColor(0,255,0);
		}
		if(RGB1_ColorMod == 2)
		{
			RGB1_SetColor(0,0,255);
		}
		P1IFG &= ~BIT1;
		for(i=0;i<30000;i++);  // Delay for bounce
	}

	if(P1IFG & BIT4) //SW2
	{
		RGB1_BrightMod++;
		if(RGB1_BrightMod > 2)
		{
			RGB1_BrightMod = 0;
		}
		if(RGB1_BrightMod == 0)
		{
			RGB1_SetBrightness(100);
		}
		if(RGB1_BrightMod == 1)
		{
			RGB1_SetBrightness(50);
		}
		if(RGB1_BrightMod == 2)
		{
			RGB1_SetBrightness(10);
		}
		P1IFG &= ~BIT4;
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
	RGB1_HandleColor();
	TA0CCTL0 &= ~CCIFG;
}

/*!
 * \brief   Interrupt routine of TimerA0_N
 *
 * \return None.
 */
void TimerA0_NIsrHandler(void)
{
	if(TA0IV & BIT1) // TA0CCR1 is reached
	{
		TA0CCTL1 &= ~CCIFG;
	}
}


