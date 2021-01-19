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

#include <xc.h>

#include <stdbool.h>
#include "button.h"

#define PIN_STATE  PORTAbits.RA12
#define PIN_DIRECTION  TRISAbits.TRISA12

#define BUTTON_PRESSED      0
#define BUTTON_NOT_PRESSED  1

#define PIN_INPUT           1
#define PIN_OUTPUT          0

/*********************************************************************
 * Function: bool BUTTON_IsPressed( void );
 *
 * Overview: Returns the current state of the requested button
 *
 * PreCondition: None.
 *
 * Input: None.
 *
 * Output: true if pressed; false if not pressed.
 *
 ********************************************************************/
bool BUTTON_IsPressed ( void )
{
    IOCPUAbits.CNPUA12 = 1;
    PIN_DIRECTION = PIN_INPUT;
    return ( (PIN_STATE == BUTTON_PRESSED) ? true : false);
}
