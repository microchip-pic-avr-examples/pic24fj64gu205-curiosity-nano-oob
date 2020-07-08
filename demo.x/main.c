/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.167.0
        Device            :  PIC24FJ64GU205
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.50
        MPLAB 	          :  MPLAB X v5.35
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/system.h"
#include "button.h"
#include "led.h"
#include "console.h"
#include "mcc_generated_files/usb/usb_device.h"
#include "usb_status_indicator.h"

extern void MCC_USB_CDC_DemoTasks(void);

static bool welcomePrinted = false;
static bool buttonPressedPrinted = false;

static bool IsWelcomeMessageNeeded(void);
static void PrintWelcomeMessage(void);
static bool IsButtonPressedMessageNeeded(void);
static void PrintButtonPressedMessage(void);
            
int main(void)
{    
    SYSTEM_Initialize();
        
    while (1)
    { 
        if(IsWelcomeMessageNeeded() == true)
        {
            PrintWelcomeMessage();
        }
        
        if(IsButtonPressedMessageNeeded() == true)
        {
            PrintButtonPressedMessage();
        }
        
        CONSOLE_Tasks();
        MCC_USB_CDC_DemoTasks();
        USB_STATUS_INDICATOR_Tasks();
    }

    return 1;
}

static bool IsWelcomeMessageNeeded(void)
{
    if(USBGetDeviceState() != CONFIGURED_STATE)
    {
        welcomePrinted = false;
        return false;
    }
          
    if(BUTTON_IsPressed() == true)
    {
        if(welcomePrinted == false)
        {
            return true;
        }
    }
    
    return false;
}

static void PrintWelcomeMessage(void)
{
    welcomePrinted = true;
    CONSOLE_Print("\r\n\r\n");
    CONSOLE_Print("*******************************************************\r\n");
    CONSOLE_Print("PIC24FJ64GU205 Curiosity Nano Demo\r\n");
    CONSOLE_Print("*******************************************************\r\n");
}

static bool IsButtonPressedMessageNeeded(void)
{
    if(USBGetDeviceState() != CONFIGURED_STATE)
    {
        return false;
    }
          
    if(BUTTON_IsPressed() == true)
    {
        if(buttonPressedPrinted == false)
        {
            return true;
        }
    }
    else
    {
        buttonPressedPrinted = false;
    }
    
    return false;
}

static void PrintButtonPressedMessage(void)
{
    buttonPressedPrinted = true;
    CONSOLE_Print("Button Pressed\r\n");
}

