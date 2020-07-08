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

#ifndef SYSTEM_PERIPHERAL_CLOCK
#define SYSTEM_PERIPHERAL_CLOCK 16000000
#pragma message "This module requires a definition for the peripheral clock frequency.  Assuming 16MHz Fcy (32MHz Fosc).  Define value if this is not correct."
#endif

#include <xc.h>
#include <stdbool.h>

#define LED_STATE               LATCbits.LATC1
#define LED_PIN_DIRECTION       TRISCbits.TRISC1

#define LED_ON  0
#define LED_OFF 1

#define INPUT  1
#define OUTPUT 0

#define MAXIMUM_INTENSITY 0x03FF
#define DEFAULT_INTENSITY (MAXIMUM_INTENSITY)

static uint16_t current_intensity = DEFAULT_INTENSITY;
static bool ledOn = false;
static int fadeRate = 20;

static volatile uint16_t fadeCount = 0;
#define FADE_COUNT 200

void LED_Enable(void)
{
    LED_PIN_DIRECTION = OUTPUT;
    LED_STATE = LED_OFF;
    
    _CCT4IF = 0;
    _CCT4IE = 1;
    
    CCP4CON1Lbits.CCPON = 0;        //Disable   
                
    ledOn = false;
    
    LED_PIN_DIRECTION = OUTPUT;

    _RP17R = 26; //26 = OCM4A -> RC1[RP17]

    //uses CCP4-PWM
    CCP4RAL = 0;
    CCP4RBL = MAXIMUM_INTENSITY;
    CCP4PRL = MAXIMUM_INTENSITY;
    CCP4PRH = 0;
    CCP4TMRL = 0;
    CCP4CON1Lbits.MOD = 0b0100;     //dual-compare
    CCP4CON1Lbits.T32 = 0;          //16-bit mode
    CCP4CON1Lbits.CLKSEL = 0b000;   //System clock
    CCP4CON1Lbits.TMRPS = 0b00;     //1:1 Pre-scaler 
    CCP4CON1Lbits.CCSEL = 0;        //Output compare
}

void LED_On(void)
{   
    if(ledOn == false)
    {
        ledOn = true;
        
        if( CCP4CON1Lbits.CCPON == 0 )
        {
            fadeCount = 0;

            CCP4CON1Lbits.CCPON = 1;        //Enable   
        }
    }
}

void LED_Off(void)
{
    if(ledOn == true)
    {
        ledOn = false;
        
        if( CCP4CON1Lbits.CCPON == 0 )
        {
            ledOn = false;
            fadeCount = 0;

            CCP4CON1Lbits.CCPON = 1;        //Enable   
        }
    }
}

void LED_Toggle(void)
{
	if(CCP4CON1Lbits.CCPON)
    {
        LED_Off();
    }
    else
    {
        LED_On();
    }
}

void LED_SetIntensity(uint16_t new_intensity)
{  
    //Convert 16-bit to 10-bit to reduce flicker/jitter
    new_intensity >>= 6;
    
    CCP4RBL = MAXIMUM_INTENSITY-new_intensity; //active low so invert
    
    current_intensity = new_intensity;
    
    if(CCP4TMRL > new_intensity)
    {
        CCP4TMRL = 0;
    }
}

void __attribute__((interrupt,auto_psv)) _CCT4Interrupt(void)
{
    _CCT4IF = 0;
    
    if(fadeCount++ == FADE_COUNT)
    {
        fadeCount = 0;
        
        if(ledOn == true)
        {
            if(CCP4RBL < fadeRate)
            {
                LED_PIN_DIRECTION = OUTPUT;
                LED_STATE = LED_ON;
                CCP4CON1Lbits.CCPON = 0;
                CCP4RBL = 0;
            }
            else
            {
                CCP4RBL -= fadeRate;
            }
        }
        else
        {
            if(CCP4RBL + fadeRate > MAXIMUM_INTENSITY)
            {
                LED_PIN_DIRECTION = OUTPUT;
                LED_STATE = LED_OFF;
                CCP4CON1Lbits.CCPON = 0;
                CCP4RBL = MAXIMUM_INTENSITY;
            }
            else
            {
                CCP4RBL += fadeRate;
            }
        }
    }
}
