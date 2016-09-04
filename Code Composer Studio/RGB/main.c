/*!
 * \file    main.c
 * \brief   Serial communication - Packet framing
 * \author  Olivier Herrou
 * \version 1.0
 * \date    01 Sept 2016
 */

/* TODO
 Il faut supprimer la fonction "interface" et faire des commandes dans lesquelles on envoie les parametres de la LED
 Actuellement on peut quasiment faire la communication PC => MSP
 Il faudra faire en sorte de coder sur le PC le mécanisme de réception des trames provenant du MSP
 	 Cela nécessitera de modifier les fonctions Putc et Puts du MSP car on envoie la string mais on l'encapsule pas.
 	 En gros dans Puts, la data c'est le payload, il faut mettre header length et footer à côté
 	 Et faire une jolie fonction, send_trame(data,length). Et avec ça on est bon je pense
 Error handling:
  - size of the payload and data[] length

*/
#include "msp432.h"
#include "RGB.h"
#include "uart.h"


void fsmUart(unsigned char);
void readTrame(void);

#define UART_HEADER 0x01
#define UART_FOOTER 0x02
#define OPCODE_RGB 0x48
#define OPCODE_BRIGHTNESS 0x49

enum State {
  STATE_START = 48, // Offset so that it can be printable
  STATE_SOF,
  STATE_LOF,
  STATE_DATA,
  STATE_CRC,
  STATE_EOF,
  STATE_VERIF,
  STATE_END
};


static unsigned char szLength = 0x00;
static unsigned char szData[20] = {0x00}; // Need to handle the size somewhere else
static unsigned char szState = STATE_SOF;
static int iNbTrame = 0;

void main(void)
{

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

	/* ~~~> Clock <~~~ */
	CS->KEY = CS_KEY_VAL;
	CS->CTL0 = 0;
	CS->CTL0 = CS_CTL0_DCORSEL_3;
	CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
	CS->KEY = 0;

	/* ~~~> Interrupt <~~~ */
	__enable_interrupt();
	NVIC->ISER[0] = TA0_0_IRQn; // TimerA0_0
	NVIC->ISER[0] = 1 << (( EUSCIA0_IRQn) & 31); //EUSCIA0
	NVIC->ISER[1] = PORT1_IRQn; // Port1

/* ~~~> I/O <~~~ */
	/* ~~~> PORT1 <~~~ */
	P1DIR |= BIT0;  // LED1 output
	P1REN  = BIT1 + BIT4;  // activate pullup/down on SW1 and SW2
	P1OUT  = BIT1 + BIT4;  // pullup
	P1IFG  = 0x00;	// no interruption pending
	P1IE   = BIT1 + BIT4;  // interrupt enable SW1 and SW2
	P1IES  = BIT1 + BIT4;  // Low to high, must write because no defaut value ??

	RGB_Init();
	UART0_Init();

	int iPulse = 7;
	RGB_SetPulse(2000,iPulse);
	RGB_SetBrightness(1);
	RGB_SetColor(255,255,255);
	for(;;)
	{
		RGB_HandleColor();
		readTrame();
	}
}

void readTrame(void)
{
	if(iNbTrame>0)
	{
		if(szData[0] == OPCODE_RGB)// Expect 3 data, r component, g component, b component
		{
			RGB_SetColor(szData[1],szData[2],szData[3]);
			Puts("Colog rgb\n");
		}
		else if(szData[0] == OPCODE_BRIGHTNESS) // expect 1 data, percentage 0-100
		{
			RGB_SetBrightness(szData[1]);
			Puts("Brigthness\n");
		}
		iNbTrame--;
	}
}

void fsmUart(unsigned char szIncByte)
{
	char szNextState = szState;
	static int indexData = 0;
	switch(szState)
	{
		case STATE_START:
			if(szIncByte == UART_FOOTER)
			{
				Puts("Resynch\n");
				szNextState = STATE_SOF;
			}
			break;

		case STATE_SOF:
			if(szIncByte == UART_HEADER)
			{
				Puts("Header detected\n");
				szNextState = STATE_LOF;
			}
			else szNextState = STATE_START; // Error, received an unexpected byte
			break;

		case STATE_LOF:
			szLength = szIncByte;
			Putc(szLength);
			Puts("\n");
			szNextState = STATE_DATA;
			break;

		case STATE_DATA:
			szData[indexData] = szIncByte;
			indexData++;
			if(indexData >= szLength)
			{
				indexData = 0;
				szNextState = STATE_EOF; // CRC next time
			}
			break;

		case STATE_CRC:
			//TODO Retrieve CRC
			szNextState = STATE_EOF;
			break;

		case STATE_EOF:
			if(szIncByte == UART_FOOTER)
			{
				P1OUT ^= BIT0;
				Puts("End of trame\n");
				iNbTrame++;
				szNextState = STATE_SOF;
			}
			else szNextState = STATE_START;
			break;

		default :
			break;
	}

	if(szState != szNextState)
	{
		szState = szNextState;
	}
}


/*!
 * \brief   Interrupt routine of PORT1
 *
 * \return None.
 */
void PORT1_IRQHandler(void)
{
	uint16_t i = 0;
	if(P1IFG & BIT1) //SW1
	{
		P1IFG &= ~BIT1;
		for(i=0;i<30000;i++);  // Delay for bounce
	}
}

/*!
 * \brief   Interrupt routine of TimerA0_0
 *
 * \return None.
 */
void TA0_0_IRQHandler(void)
{
	RGB_HandleColor();
	TA0CCTL0 &= ~CCIFG;
}

/*!
 * \brief   Interrupt routine of EUSCIA0
 *
 * \return None.
 */
void EUSCIA0_IRQHandler(void)
{
	 if (UCA0IFG & UCRXIFG)
	 {
	   while(!(UCA0IFG&UCTXIFG));
	   fsmUart(UCA0RXBUF);
	   __no_operation();
	 }
}
