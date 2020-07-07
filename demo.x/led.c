/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/

#include <xc.h>

#include <stdbool.h>

#include "led.h"

#define LED_D3_SETTING      LATBbits.LATB0
#define LED_D3_DIRECTION    TRISBbits.TRISB0

#define LED_ON  1
#define LED_OFF 0

#define PIN_INPUT   1
#define PIN_OUTPUT  0

/*********************************************************************
* Function: void LED_On(void);
*
* Overview: Turns requested LED on
*
* PreCondition: LED configured via LED_Configure()
*
* Input: none
*
* Output: none
*
********************************************************************/
void LED_On(void)
{
    LED_D3_DIRECTION = PIN_OUTPUT;
    LED_D3_SETTING = LED_ON;
}

/*********************************************************************
* Function: void LED_Off(void);
*
* Overview: Turns requested LED off
*
* PreCondition: LED configured via LEDConfigure()
*
* Input: none
*
* Output: none
*
********************************************************************/
void LED_Off(void)
{
    LED_D3_DIRECTION = PIN_OUTPUT;
    LED_D3_SETTING = LED_OFF;
}

/*********************************************************************
* Function: void LED_Toggle(void);
*
* Overview: Toggles the state of the requested LED
*
* PreCondition: LED configured via LEDConfigure()
*
* Input: none
*
* Output: none
*
********************************************************************/
void LED_Toggle(void)
{
    LED_D3_DIRECTION = PIN_OUTPUT;
    LED_D3_SETTING ^= 1;
}

/*********************************************************************
* Function: bool LED_Get(void);
*
* Overview: Returns the current state of the requested LED
*
* PreCondition: LED configured via LEDConfigure()
*
* Input: none
*
* Output: true if on, false if off
*
********************************************************************/
bool LED_Get(void)
{
    return ( ( LED_D3_SETTING == LED_ON ) ? true : false ) ;
}

/*********************************************************************
* Function: void LED_Enable(void);
*
* Overview: Configures the LED for use by the other LED API
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
********************************************************************/
void LED_Enable(void)
{
    LED_D3_DIRECTION = PIN_OUTPUT;
}

