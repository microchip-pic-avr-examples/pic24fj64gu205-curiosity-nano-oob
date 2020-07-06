/*******************************************************************************
Copyright 2018 Microchip Technology Inc. (www.microchip.com)

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

#include <stdint.h>
#include <stdbool.h>
#include <xc.h>
#include "lcd.h"

#ifndef FCY
    #if defined(XTAL_FREQ)
        #define FCY (XTAL_FREQ/2)
    #elif defined(SYSTEM_PERIPHERAL_CLOCK)
        #define FCY SYSTEM_PERIPHERAL_CLOCK
    #else
        #error This module requires the clock frequency to be defined.
    #endif
#endif

#include <libpic30.h>

/* Private Definitions ***********************************************/
#define FAST_INSTRUCTION_TIME_US    50
#define SLOW_INSTRUCTION_TIME_US    1650
#define STARTUP_TIME_MS             30
#define SIGNAL_TIMING_US            1

#define MAX_COLUMN                  16

enum COMMAND
{
    COMMAND_CLEAR_SCREEN        = 0x01,
    COMMAND_RETURN_HOME         = 0x02,
    COMMAND_ENTER_DATA_MODE     = 0x06,
    COMMAND_CURSOR_OFF          = 0x0C,
    COMMAND_CURSOR_ON           = 0x0F,
    COMMAND_MOVE_CURSOR_LEFT    = 0x10,
    COMMAND_MOVE_CURSOR_RIGHT   = 0x14,
    COMMAND_SET_MODE_4_BIT      = 0x28,
    COMMAND_SET_MODE_8_BIT      = 0x38,
    COMMAND_ROW_0_HOME          = 0x80,
    COMMAND_ROW_1_HOME          = 0xC0,
    COMMAND_START_UP_1          = 0x33,   
    COMMAND_START_UP_2          = 0x32
};

struct PIN_STATE{
   uint8_t D4 :1;
   uint8_t D5 :1;
   uint8_t D6 :1;
   uint8_t D7 :1;
};
   
/* Private Functions *************************************************/
static void CarriageReturn ( void ) ;
static void ShiftCursorLeft ( void ) ;
static void ShiftCursorRight ( void ) ;
static void ShiftCursorUp ( void ) ;
static void ShiftCursorDown ( void ) ;
static void SendData ( char ) ;
static void SendCommand ( unsigned int ) ;
static void DataNibbleWrite(uint8_t value);
static void PinStateSave(void);
static void PinStateRestore(void);

#define LCD_RW_SetHigh()        
#define LCD_RW_SetLow()        

static inline void LCD_D4_SetHigh(void)
{
    TRISBbits.TRISB0 = 0; 
    LATBbits.LATB0 = 1;
}

static inline void LCD_D4_SetLow(void)
{
    TRISBbits.TRISB0 = 0; 
    LATBbits.LATB0 = 0;
}

static inline void LCD_D5_SetHigh(void)
{
    TRISBbits.TRISB1 = 0; 
    LATBbits.LATB1 = 1;
}

static inline void LCD_D5_SetLow(void)
{
    TRISBbits.TRISB1 = 0; 
    LATBbits.LATB1 = 0;
}

static inline void LCD_D6_SetHigh(void)
{
    TRISBbits.TRISB2 = 0; 
    LATBbits.LATB2 = 1;
}

static inline void LCD_D6_SetLow(void)
{
    TRISBbits.TRISB2 = 0; 
    LATBbits.LATB2 = 0;
}

static inline void LCD_D7_SetHigh(void)
{
    TRISBbits.TRISB3 = 0; 
    LATBbits.LATB3 = 1;
}

static inline void LCD_D7_SetLow(void)
{
    TRISBbits.TRISB3 = 0; 
    LATBbits.LATB3 = 0;
}

static inline void LCD_RS_SetHigh(void)
{
    TRISAbits.TRISA7 = 0; 
    LATAbits.LATA7 = 1;
}

static inline void LCD_RS_SetLow(void)
{
    TRISAbits.TRISA7 = 0; 
    LATAbits.LATA7 = 0;
}

static inline void LCD_Enable_SetHigh(void)
{
    TRISCbits.TRISC1 = 0; 
    LATCbits.LATC1 = 1;
}

static inline void LCD_Enable_SetLow(void)
{
    TRISCbits.TRISC1 = 0; 
    LATCbits.LATC1 = 0;
}

/* Private variables ************************************************/
static uint8_t row ;
static uint8_t column ;
static struct PIN_STATE pin_state;

/*********************************************************************
 * Function: bool LCD_Initialize(void);
 *
 * Overview: Initializes the LCD screen.  Can take several hundred
 *           milliseconds.
 *
 * PreCondition: none
 *
 * Input: None
 *
 * Output: true if initialized, false otherwise
 *
 ********************************************************************/
bool LCD_Initialize ( void )
{             
    DataNibbleWrite(0);
    LCD_RW_SetLow(); 
    LCD_RS_SetLow(); 
    LCD_Enable_SetLow ( ) ;  
    LCD_Enable_SetHigh ( )  ;
    
    __delay_ms(STARTUP_TIME_MS);

    SendCommand ( COMMAND_START_UP_1 ) ;
    SendCommand ( COMMAND_START_UP_2 ) ;
    SendCommand ( COMMAND_SET_MODE_4_BIT ) ;
    SendCommand ( COMMAND_CURSOR_OFF ) ;
    SendCommand ( COMMAND_ENTER_DATA_MODE ) ;

    LCD_ClearScreen ( ) ;   
    
    return true ;
}

/*********************************************************************
 * Function: void LCD_PutString(char* inputString, uint16_t length);
 *
 * Overview: Puts a string on the LCD screen.  Unsupported characters will be
 *           discarded.  May block or throw away characters is LCD is not ready
 *           or buffer space is not available.  Will terminate when either a
 *           null terminator character (0x00) is reached or the length number
 *           of characters is printed, which ever comes first.
 *
 * PreCondition: already initialized via LCD_Initialize()
 *
 * Input: char* - string to print
 *        uint16_t - length of string to print
 *
 * Output: None
 *
 ********************************************************************/
void LCD_PutString ( char* inputString , uint16_t length )
{
    while (length--)
    {
        switch (*inputString)
        {
            case 0x00:
                return ;

            default:
                LCD_PutChar ( *inputString++ ) ;
                break ;
        }
    }
}
/*********************************************************************
 * Function: void LCD_PutChar(char);
 *
 * Overview: Puts a character on the LCD screen.  Unsupported characters will be
 *           discarded.  May block or throw away characters is LCD is not ready
 *           or buffer space is not available.
 *
 * PreCondition: already initialized via LCD_Initialize()
 *
 * Input: char - character to print
 *
 * Output: None
 *
 ********************************************************************/
void LCD_PutChar ( char inputCharacter )
{
    static char lastCharacter = 0;
        
    switch (inputCharacter)
    {
        case '\r':
            if(lastCharacter != '\n')
            {
                CarriageReturn ( ) ;
            }
            break ;

        case '\n': 
            if(lastCharacter != '\r')
            {
                CarriageReturn ( ) ;
            }
            
            if (row == 0)
            {
                ShiftCursorDown ( ) ;
            }
            else
            {
                ShiftCursorUp ( ) ;
            }
            break ;

        case '\b':
            ShiftCursorLeft ( ) ;
            LCD_PutChar ( ' ' ) ;
            ShiftCursorLeft ( ) ;
            break ;
            
        case '\f':
            LCD_ClearScreen();
            break;

        default:
            if (column == MAX_COLUMN)
            {
                column = 0 ;
                if (row == 0)
                {
                    SendCommand ( COMMAND_ROW_1_HOME ) ;
                    row = 1 ;
                }
                else
                {
                    SendCommand ( COMMAND_ROW_0_HOME ) ;
                    row = 0 ;
                }
            }
            
            SendData ( inputCharacter ) ;
            column++ ;
            break ;
    }
    
    lastCharacter = inputCharacter;
}
/*********************************************************************
 * Function: void LCD_ClearScreen(void);
 *
 * Overview: Clears the screen, if possible.
 *
 * PreCondition: already initialized via LCD_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
void LCD_ClearScreen ( void )
{
    SendCommand ( COMMAND_CLEAR_SCREEN ) ;
    SendCommand ( COMMAND_RETURN_HOME ) ;

    row = 0 ;
    column = 0 ;
}

/*********************************************************************
 * Function: void LCD_CursorEnable(bool enable)
 *
 * Overview: Enables/disables the cursor
 *
 * PreCondition: None
 *
 * Input: bool - specifies if the cursor should be on or off
 *
 * Output: None
 *
 ********************************************************************/
void LCD_CursorEnable ( bool enable )
{
    if (enable == true)
    {
        SendCommand ( COMMAND_CURSOR_ON ) ;
    }
    else
    {
        SendCommand ( COMMAND_CURSOR_OFF ) ;
    }
}

/*******************************************************************/
/*******************************************************************/
/* Private Functions ***********************************************/
/*******************************************************************/
/*******************************************************************/
/*********************************************************************
 * Function: static void CarriageReturn(void)
 *
 * Overview: Handles a carriage return
 *
 * PreCondition: already initialized via LCD_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
static void CarriageReturn ( void )
{
    if (row == 0)
    {
        SendCommand ( COMMAND_ROW_0_HOME ) ;
    }
    else
    {
        SendCommand ( COMMAND_ROW_1_HOME ) ;
    }
    column = 0 ;
}
/*********************************************************************
 * Function: static void ShiftCursorLeft(void)
 *
 * Overview: Shifts cursor left one spot (wrapping if required)
 *
 * PreCondition: already initialized via LCD_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
static void ShiftCursorLeft ( void )
{
    uint8_t i ;

    if (column == 0)
    {
        if (row == 0)
        {
            SendCommand ( COMMAND_ROW_1_HOME ) ;
            row = 1 ;
        }
        else
        {
            SendCommand ( COMMAND_ROW_0_HOME ) ;
            row = 0 ;
        }

        //Now shift to the end of the row
        for (i = 0 ; i < ( MAX_COLUMN - 1 ) ; i++)
        {
            ShiftCursorRight ( ) ;
        }
    }
    else
    {
        column-- ;
        SendCommand ( COMMAND_MOVE_CURSOR_LEFT ) ;
    }
}
/*********************************************************************
 * Function: static void ShiftCursorRight(void)
 *
 * Overview: Shifts cursor right one spot (wrapping if required)
 *
 * PreCondition: already initialized via LCD_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
static void ShiftCursorRight ( void )
{
    SendCommand ( COMMAND_MOVE_CURSOR_RIGHT ) ;
    column++ ;

    if (column == MAX_COLUMN)
    {
        column = 0 ;
        if (row == 0)
        {
            SendCommand ( COMMAND_ROW_1_HOME ) ;
            row = 1 ;
        }
        else
        {
            SendCommand ( COMMAND_ROW_0_HOME ) ;
            row = 0 ;
        }
    }
}
/*********************************************************************
 * Function: static void ShiftCursorUp(void)
 *
 * Overview: Shifts cursor up one spot (wrapping if required)
 *
 * PreCondition: already initialized via LCD_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
static void ShiftCursorUp ( void )
{
    uint8_t i ;

    for (i = 0 ; i < MAX_COLUMN ; i++)
    {
        ShiftCursorLeft ( ) ;
    }
}
/*********************************************************************
 * Function: static void ShiftCursorDown(void)
 *
 * Overview: Shifts cursor down one spot (wrapping if required)
 *
 * PreCondition: already initialized via LCD_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
static void ShiftCursorDown ( void )
{
    uint8_t i ;

    for (i = 0 ; i < MAX_COLUMN ; i++)
    {
        ShiftCursorRight ( ) ;
    }
}

/*********************************************************************
 * Function: static void SendData(char data)
 *
 * Overview: Sends data to LCD
 *
 * PreCondition: None
 *
 * Input: char - contains the data to be sent to the LCD
 *
 * Output: None
 *
 ********************************************************************/
static void SendData ( char data )
{
    PinStateSave();
    
    LCD_RW_SetLow ( ) ;
    LCD_RS_SetHigh ( ) ;
    
    DataNibbleWrite(data>>4);
    __delay_us(SIGNAL_TIMING_US);
    LCD_Enable_SetHigh ( ) ;
    __delay_us(SIGNAL_TIMING_US);
    
    LCD_Enable_SetLow ( ) ;
    
    DataNibbleWrite(data);
    __delay_us(SIGNAL_TIMING_US);
    LCD_Enable_SetHigh ( ) ;
    __delay_us(SIGNAL_TIMING_US);
    LCD_Enable_SetLow ( ) ;
    
    LCD_RS_SetLow ( ) ;
 
    __delay_us(FAST_INSTRUCTION_TIME_US);
    
    PinStateRestore();
}

/*********************************************************************
 * Function: static void SendCommand(char data)
 *
 * Overview: Sends command to LCD
 *
 * PreCondition: None
 *
 * Input: char - contains the command to be sent to the LCD
 *        unsigned int - has the specific delay for the command
 *
 * Output: None
 *
 ********************************************************************/
static void SendCommand ( enum COMMAND command )
{       
    PinStateSave();
    
    LCD_RW_SetLow ( ) ;
    LCD_RS_SetLow ( ) ;
	
    DataNibbleWrite(command >> 4);
    LCD_Enable_SetHigh ( ) ;
    __delay_us(SIGNAL_TIMING_US);
    LCD_Enable_SetLow ( ) ;
    
    LCD_RW_SetLow ( ) ;
    LCD_RS_SetLow ( ) ;
    DataNibbleWrite(command);
    LCD_Enable_SetHigh ( ) ;
    __delay_us(SIGNAL_TIMING_US);
    LCD_Enable_SetLow ( ) ;  
        
    switch(command)
    {
        case COMMAND_MOVE_CURSOR_LEFT:
        case COMMAND_MOVE_CURSOR_RIGHT:
        case COMMAND_SET_MODE_8_BIT:
		case COMMAND_SET_MODE_4_BIT:
        case COMMAND_CURSOR_OFF:
            __delay_us(FAST_INSTRUCTION_TIME_US);
            break;
            
        case COMMAND_ENTER_DATA_MODE:
        case COMMAND_CLEAR_SCREEN:
        case COMMAND_RETURN_HOME:
        case COMMAND_CURSOR_ON:
        case COMMAND_ROW_0_HOME:
        case COMMAND_ROW_1_HOME:
        default:
            __delay_us(SLOW_INSTRUCTION_TIME_US);
            break;
    }
    
    PinStateRestore();
    
}

static void DataNibbleWrite(uint8_t value)
{   
    if(value & 0x01)
    {
        LCD_D4_SetHigh();
    }
    else
    {
        LCD_D4_SetLow();
    }
    
    
    if(value & 0x02)
    {
        LCD_D5_SetHigh();
    }
    else
    {
        LCD_D5_SetLow();
    }
    
  
    if(value & 0x04)
    {
        LCD_D6_SetHigh();
    }
    else
    {
        LCD_D6_SetLow();
    }
    

    if(value & 0x08)
    {
        LCD_D7_SetHigh();
    }
    else
    {
        LCD_D7_SetLow();
    }
}

static void PinStateSave(void)
{
    pin_state.D4 = LATBbits.LATB0;
    pin_state.D5 = LATBbits.LATB1;
    pin_state.D6 = LATBbits.LATB2;
    pin_state.D7 = LATBbits.LATB3;
}

static void PinStateRestore(void)
{
    LATBbits.LATB0 = pin_state.D4;
    LATBbits.LATB1 = pin_state.D5;
    LATBbits.LATB2 = pin_state.D6;
    LATBbits.LATB3 = pin_state.D7;
}