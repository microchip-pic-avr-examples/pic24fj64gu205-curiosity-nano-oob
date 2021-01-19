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

#include "mcc_generated_files/usb/usb_device_cdc.h"

#include <stdint.h>
#include <stdbool.h>

#define FIFO_SIZE 1000u
#define MAX_PACKET 64u

static uint8_t fifo[FIFO_SIZE];
static uint8_t txBuffer[MAX_PACKET];
static uint16_t head = 0;
static uint16_t tail = 0;

static void FlushFIFO(void);
static void FIFOPut(uint8_t data);
static uint16_t GetFIFODepth(void);
static uint8_t FIFOGet(void);

void CONSOLE_Initialize(void)
{
    FlushFIFO();
}

void CONSOLE_Print(char* inputString)
{
    char* data = inputString;
    
    while(*data != 0u)
    {
        FIFOPut((uint8_t)*data++);
    }
}

void CONSOLE_Tasks(void)
{
    uint16_t transmitSize = GetFIFODepth();
    uint8_t i;
    
    if(USBGetDeviceState() != CONFIGURED_STATE)
    {
        CONSOLE_Initialize();
    }
    else
    {
        if( (USBUSARTIsTxTrfReady() == true) && (transmitSize != 0u) )
        {
            if(transmitSize > MAX_PACKET)
            {
                transmitSize = MAX_PACKET;
            }

            for(i=0; i<transmitSize; i++)
            {
                txBuffer[i] = FIFOGet();
            }

            putUSBUSART(txBuffer,transmitSize);
        }

        CDCTxService();
    }
}

static uint16_t GetFIFODepth(void)
{
    uint16_t depth = (tail - head);
            
    if(tail < head)
    {
        depth = ((FIFO_SIZE - head) + tail);
    }
    
    return depth;
}

static void FlushFIFO(void)
{
    head = 0;
    tail = 0;
}

static void FIFOPut(uint8_t data)
{
    if(GetFIFODepth() < (FIFO_SIZE - 1u))
    {
        fifo[tail] = data;
        tail++;
        
        if(tail == FIFO_SIZE)
        {
            tail = 0;
        }
    }
}

static uint8_t FIFOGet(void)
{
    uint8_t data = 0u;
    
    if(GetFIFODepth() > 0u)
    {
        data = fifo[head];
        head++;

        if(head == FIFO_SIZE)
        {
            head = 0;
        }
    }
    
    return data;
}

