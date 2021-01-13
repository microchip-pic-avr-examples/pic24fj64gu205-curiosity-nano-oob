//Copyright 2016 Microchip Technology Inc. (www.microchip.com)
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

#ifndef LED_H
#define LED_H

#include <stdbool.h>

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
void LED_On(void);

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
void LED_Off(void);

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
void LED_Toggle(void);

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
bool LED_Get(void);

/*********************************************************************
* Function: bool LED_Enable(void);
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
void LED_Enable(void);

/*********************************************************************
* Function: bool LED_SetFadeRate(uint16_t rate);
*
* Overview: Configures the LED fade rate.  Units are arbitrary based
* on the system clock.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
********************************************************************/
void LED_SetFadeRate(uint16_t rate);

/*********************************************************************
* Function: bool LED_Enable(uint16_t new_intensity);
*
* Overview: Configures the maximum LED intensity when the LED is on.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
********************************************************************/
void LED_SetIntensity(uint16_t new_intensity);


#endif //LED_H
