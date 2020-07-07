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

#include "mcc_generated_files/usb/usb_device_cdc.h"

#include <stdint.h>
#include <stdbool.h>

#define FIFO_SIZE 1000
#define MAX_PACKET 64

static uint8_t fifo[FIFO_SIZE];
static uint8_t txBuffer[MAX_PACKET];
static uint16_t head = 0;
static uint16_t tail = 0;

static void FlushFIFO(void);
static bool FIFOPut(uint8_t data);
static uint16_t GetFIFODepth(void);
static uint8_t FIFOGet(void);

void CONSOLE_Initialize(void)
{
    FlushFIFO();
}

void CONSOLE_Print(char* inputString)
{
    while(*inputString)
    {
        FIFOPut((uint8_t)*inputString++);
    }
}

void CONSOLE_Tasks(void)
{
    uint16_t transmitSize = GetFIFODepth();
    uint8_t i;
    
    if( (USBUSARTIsTxTrfReady() == true) && (transmitSize != 0) )
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

static uint16_t GetFIFODepth(void)
{
    if(tail >= head)
    {
        return (tail - head);
    }
    
    return ((FIFO_SIZE - head) + tail);
}

static void FlushFIFO(void)
{
    head = 0;
    tail = 0;
}

static bool FIFOPut(uint8_t data)
{
    if(GetFIFODepth() < (FIFO_SIZE - 1))
    {
        fifo[tail++] = data;
        if(tail == FIFO_SIZE)
        {
            tail = 0;
        }
        return true;
    }
    
    return false;
}

static uint8_t FIFOGet(void)
{
    uint8_t data;
    
    if(GetFIFODepth() == 0)
    {
        return 0;
    }
    
    data = fifo[head++];
    
    if(head == FIFO_SIZE)
    {
        head = 0;
    }
    
    return data;
}

