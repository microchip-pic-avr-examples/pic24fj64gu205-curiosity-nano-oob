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

#include "mcc_generated_files/usb/usb_device.h"
#include "led.h"
#include "timer_1ms.h"

#define FADE_RATE 10

enum DETACHED_LED_PATTERN{
    DETACHED_LED_PATTERN_FADE_ON_START,
    DETACHED_LED_PATTERN_FADE_ON_WAIT,
    DETACHED_LED_PATTERN_FADE_OFF_START,
    DETACHED_LED_PATTERN_FADE_OFF_WAIT,
    DETACHED_LED_PATTERN_OFF_START,
    DETACHED_LED_PATTERN_OFF_WAIT
};

static enum DETACHED_LED_PATTERN detachedLEDPattern;
static volatile bool timerExpired = false;

static void ResetDetachedStateMachine(void);
static void RunDetachedStateMachine(void);
static void Request1SecondTimeout(void);
static void TimerHandler(void);

void USB_STATUS_INDICATOR_Tasks(void)
{
    if(USBGetDeviceState() == CONFIGURED_STATE)
    {
        ResetDetachedStateMachine();
        LED_SetFadeRate(0);
        LED_On();
    }
    else
    {
        RunDetachedStateMachine();
    }
}

static void ResetDetachedStateMachine(void)
{
    detachedLEDPattern = DETACHED_LED_PATTERN_FADE_ON_START;
    timerExpired = false;
}

static void RunDetachedStateMachine(void)
{
    switch(detachedLEDPattern)
    {
        case DETACHED_LED_PATTERN_FADE_ON_START:
            Request1SecondTimeout();
            LED_SetFadeRate(FADE_RATE);
            LED_On();
            detachedLEDPattern = DETACHED_LED_PATTERN_FADE_ON_WAIT;
            break;
            
        case DETACHED_LED_PATTERN_FADE_ON_WAIT:
            if(timerExpired == true)
            {
                detachedLEDPattern = DETACHED_LED_PATTERN_FADE_OFF_START;
            }
            break;
            
        case DETACHED_LED_PATTERN_FADE_OFF_START:
            Request1SecondTimeout();
            LED_Off();
            detachedLEDPattern = DETACHED_LED_PATTERN_FADE_OFF_WAIT;
            break;
            
        case DETACHED_LED_PATTERN_FADE_OFF_WAIT:
            if(timerExpired == true)
            {
                detachedLEDPattern = DETACHED_LED_PATTERN_OFF_START;
            }
            break;
            
        case DETACHED_LED_PATTERN_OFF_START:
            Request1SecondTimeout();
            detachedLEDPattern = DETACHED_LED_PATTERN_OFF_WAIT;
            break;
            
        case DETACHED_LED_PATTERN_OFF_WAIT:
            if(timerExpired == true)
            {
                detachedLEDPattern = DETACHED_LED_PATTERN_FADE_ON_START;
            }
            break;
            
        default:
            break;
    }
}

static void Request1SecondTimeout(void)
{
    timerExpired = false;
    (void)TIMER_RequestTick(&TimerHandler, 1000);
}

static void TimerHandler(void)
{
    TIMER_CancelTick(&TimerHandler);
    timerExpired = true;
}
