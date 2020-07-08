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

#define LED_STATE               LATCbits.LATC1
#define LED_PIN_DIRECTION       TRISCbits.TRISC1

#define LED_ON  0
#define LED_OFF 1

#define INPUT  1
#define OUTPUT 0

#define MAXIMUM_INTENSITY 0x03FF
#define DEFAULT_INTENSITY (MAXIMUM_INTENSITY)

static uint16_t current_intensity = DEFAULT_INTENSITY;

void LED_On(void)
{   
    if(CCP4CON1Lbits.CCPON == 0)
    {
        LED_PIN_DIRECTION = OUTPUT;

        _RP17R = 26; //26 = OCM4A -> RC1[RP17]

        //uses CCP4-PWM
        CCP4RAL = 0;
        CCP4RBL = MAXIMUM_INTENSITY-current_intensity;  //0 is on for this LED so need to invert
        CCP4PRL = MAXIMUM_INTENSITY;
        CCP4PRH = 0;
        CCP4TMRL = 0;
        CCP4CON1Lbits.MOD = 0b0100;     //dual-compare
        CCP4CON1Lbits.T32 = 0;          //16-bit mode
        CCP4CON1Lbits.CLKSEL = 0b000;   //System clock
        CCP4CON1Lbits.TMRPS = 0b00;     //1:1 Pre-scaler 
        CCP4CON1Lbits.CCSEL = 0;        //Output compare
        CCP4CON1Lbits.CCPON = 1;        //Enable   
    }
}

void LED_Off(void)
{
    LED_PIN_DIRECTION = OUTPUT;
    LED_STATE = LED_OFF;
    
    CCP4CON1Lbits.CCPON = 0;        //Disable 
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
