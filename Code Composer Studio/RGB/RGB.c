/*!
 * \file    RGB.c
 * \brief   RGB led control
 * \author  Olivier Herrou
 * \version 1.0
 * \date    22 June 2015
 */
#include "RGB.h"


/*!
 * \brief   Function to initialize the RGB led
 *
 * \remarks	the led are :
 *			* PORT2.0 : RED
 *			* PORT2.1 : GREEN
 *			* PORT2.2 : BLUE
 *			TimerA0 is used, SMCLK must be equal to 12MHz
 *			You must enable interrupt for INT_TA0_0
 * \return  None.
 */
void RGB_Init()
{

	/* ~~~> Global var <~~~ */
	RGB_SatRed 		= 0;
	RGB_SatGreen   	= 0;
	RGB_SatBlue		= 0;
	RGB_BrightHigh 	= 0;
	RGB_PulseWidth 	= 0;
	RGB_PulseHigh  	= 0;
	RGB_State   	= 1;

	/* ~~~> PORT2 <~~~ */
	P2DIR |= BIT0 + BIT1 + BIT2; // RGB led output
	P2OUT  = 0x00;

    /* ~~~> TIMERA <~~~ */
    TA0CTL   = TASSEL_2 + MC_1 + TACLR; // SMCLK, UP, clear
    TA0CCR0  = 120; // 100us
    TA0CCTL0 = CCIE;  // Interrupt enable on CCIE
}



/*!
 * \brief  Function to set the pulse of the rgb led
 *
 * \param  iPulseWidth  width of the pulse
 * \param  iCycleHigh   percentage at high level
 * \return None.
 */
void RGB_SetPulse(uint16_t iPulseWidth, uint16_t iCycleHigh)
{
	RGB_PulseWidth = iPulseWidth;
	RGB_PulseHigh  = iCycleHigh;
}

/*!
 * \brief  Function to set the brightness parameter of the rgb led
 *
 * \param  iCycleHigh   percentage at high level
 * \return None.
 */
void RGB_SetBrightness(uint16_t iCycleHigh)
{
	RGB_BrightHigh  = iCycleHigh;
}

/*!
 * \brief   Function to set the color of the RGB led
 *
 * \param   iRed   			the red saturation   (0 - 255)
 * \param   iGreen 			the green saturation (0 - 255)
 * \param   iBlue  			the blue saturation  (0 - 255)
 * \return None.
 */
void RGB_SetColor(uint16_t iRed, uint16_t iGreen, uint16_t iBlue)
{
	/* ~~~> Saturation <~~~ */
	if(iRed   > 255) iRed   = 255;
	if(iGreen > 255) iGreen = 255;
	if(iBlue  > 255) iBlue  = 255;

	/* ~~~> Assignment <~~~ */
	RGB_SatRed     = ((iRed   * RGB_COLORSCALE) / 255);
	RGB_SatGreen   = ((iGreen * RGB_COLORSCALE) / 255);
	RGB_SatBlue    = ((iBlue  * RGB_COLORSCALE) / 255);
}

/*!
 * \brief   Function to handle the rgb led
 *
 * \return None.
 */
void RGB_HandleColor(void)
{
	static uint16_t iModLED    = 0;
	static uint16_t iModBright = 0;
	static uint16_t iPulse 	   = 0;

	if(RGB_State)
	{
		/* ~~~> Pulse <~~~ */
		if(iPulse == RGB_PulseWidth) iPulse = 0;
		if(iPulse < (RGB_PulseWidth * (RGB_PulseHigh/ 100.0)) ) // LED is on
		{
			/* ~~~> BRIGHTNESS <~~~ */
			if(iModBright == RGB_BRIGHTWIDTH) iModBright = 0;
			if(iModBright < (RGB_BRIGHTWIDTH * (RGB_BrightHigh/ 100.0)) ) // LED is on
			{
				/* ~~~> Reset Cycle <~~~ */
				if(iModLED == RGB_COLORSCALE) iModLED = 0;

				/* ~~~> Red LED <~~~ */
				if(iModLED < RGB_SatRed) P2OUT |= BIT0;
				else P2OUT &=~ BIT0;

				/* ~~~> Green LED <~~~ */
				if(iModLED < RGB_SatGreen) P2OUT |= BIT1;
				else P2OUT &=~ BIT1;

				/* ~~~> Blue LED <~~~ */
				if(iModLED <  RGB_SatBlue) P2OUT |= BIT2;
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
