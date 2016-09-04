/*!
 * \file    RGB.h
 * \author  Olivier Herrou
 * \version 1.0
 * \date    22 June 2015
 */
#include "msp.h"

#define RGB_COLORSCALE 30
#define RGB_BRIGHTWIDTH 50
volatile uint16_t RGB_PulseWidth;   //in ms
volatile uint16_t RGB_PulseHigh;	// 0 - 100
volatile uint16_t RGB_BrightHigh;	// 0 - 100
volatile uint16_t RGB_SatRed; 		// 0 - RGB_COLORSCALE different level of red
volatile uint16_t RGB_SatGreen; 	// 0 - RGB_COLORSCALE different level of green
volatile uint16_t RGB_SatBlue; 	    // 0 - RGB_COLORSCALE different level of blue
volatile uint16_t RGB_State;		// 0 Off, 1 On

void RGB_Init(void);
void RGB_SetPulse(uint16_t iPulseWidth, uint16_t iCycleHigh);
void RGB_SetBrightness(uint16_t iCycleHigh);
void RGB_SetColor(uint16_t iRed, uint16_t iGreen, uint16_t iBlue);
void RGB_HandleColor(void);
