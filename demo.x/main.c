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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "timer_1ms.h"
#include "button_user.h"
#include "leds.h"
 
#include "io_mapping.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables and Functions
// *****************************************************************************
// *****************************************************************************
extern void SYS_Initialize ( void ) ;
static void TimerEventHandler( void );

static bool toggleBlinkAlive = false;

#define MEMCMP_VALUES_IDENTICAL 0

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main ( void )
{    
    /* Call the System Initialize routine*/
    SYS_Initialize ( );
    
    /* To determine how the LED and Buttons are mapped to the actual board
     * features, please see io_mapping.h. */
    LED_Enable ( LED_BLINK_ALIVE );
    LED_Enable ( LED_BUTTON_PRESSED );
    
    /* Get a timer event once every 100ms for the blink alive. */
    TIMER_SetConfiguration ( TIMER_CONFIGURATION_1MS );
    TIMER_RequestTick( &TimerEventHandler, 100 );
    
    /* Clear the screen */
    printf( "\f" );   
    
    while ( 1 )
    {
        if(toggleBlinkAlive == true)
        {
            LED_Toggle( LED_BLINK_ALIVE );
            toggleBlinkAlive = false;
        }
        
        /* To determine how the LED and Buttons are mapped to the actual board
         * features, please see io_mapping.h. */
        if(button.isPressed() == true)
        {
            LED_On( LED_BUTTON_PRESSED );
        }
        else
        {
            LED_Off( LED_BUTTON_PRESSED );
        }
    }
}

static void TimerEventHandler(void)
{    
    toggleBlinkAlive = true;
}