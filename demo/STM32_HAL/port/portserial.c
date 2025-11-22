/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "main.h"
#include "mb.h"
#include "mbport.h"
#include "stdio.h"
#include "stm32f0xx_hal.h"

/* ----------------------- Static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);

/* ----------------------- Variables ----------------------------------------*/
extern UART_HandleTypeDef* modbusUart;

uint8_t txByte = 0x00;
uint8_t rxByte = 0x00;

/* ----------------------- Start implementation -----------------------------*/

/*----------------------------------------------------------------------------*/
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    if (xRxEnable == FALSE) {
        HAL_UART_AbortReceive_IT(modbusUart);
    } else {
        SetRS485ReceiveMode();
        HAL_UART_Receive_IT(modbusUart, &rxByte, 1);
    }

    if (xTxEnable == FALSE) {
        HAL_UART_AbortTransmit_IT(modbusUart);
    } else {
        if (modbusUart->gState == HAL_UART_STATE_READY) {
            SetRS485TransmitMode();
            prvvUARTTxReadyISR();
        }
    }
}

/* --------------------------------------------------------------------------*/
BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    UNUSED( ucPORT );

    // Configure UART for Modbus communication
    uart_mb.Instance          = MB_USART;
    uart_mb.Init.BaudRate     = ulBaudRate;
    uart_mb.Init.StopBits     = UART_STOPBITS_1; // Always use 1 stop bit
    uart_mb.Init.Mode         = UART_MODE_TX_RX;
    uart_mb.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    uart_mb.Init.OverSampling = UART_OVERSAMPLING_16;

    // Configure WordLength and Parity based on data bits and parity
    if( ucDataBits == 8 )
    {
        if( eParity == MB_PAR_NONE )
        {
            uart_mb.Init.WordLength = UART_WORDLENGTH_8B;
            uart_mb.Init.Parity     = UART_PARITY_NONE;
        }
        else
        {
            uart_mb.Init.WordLength = UART_WORDLENGTH_9B; // 8 data bits + parity
            uart_mb.Init.Parity     = (eParity == MB_PAR_ODD) ? UART_PARITY_ODD : UART_PARITY_EVEN;
        }
    }
    else if( ucDataBits == 7 )
    {
        if( eParity == MB_PAR_NONE )
        {
            uart_mb.Init.WordLength = UART_WORDLENGTH_7B;
            uart_mb.Init.Parity     = UART_PARITY_NONE;
        }
        else
        {
            uart_mb.Init.WordLength = UART_WORDLENGTH_8B; // 7 data bits + parity
            uart_mb.Init.Parity     = (eParity == MB_PAR_ODD) ? UART_PARITY_ODD : UART_PARITY_EVEN;
        }
    }
    else
    {
        return FALSE; // Unsupported data bits configuration
    }

    if( HAL_UART_Init( &uart_mb ) != HAL_OK )
    {
        return FALSE; // UART initialization failed
    }

    // Disable RX and TX interrupts initially
    __HAL_UART_DISABLE_IT(&uart_mb, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&uart_mb, UART_IT_TXE);
    HAL_UART_RegisterCallback(&uart_mb, HAL_UART_TX_COMPLETE_CB_ID, UART_TxCplt);
    HAL_UART_RegisterCallback(&uart_mb, HAL_UART_RX_COMPLETE_CB_ID, UART_RxCplt);


    return TRUE;
}

/* --------------------------------------------------------------------------*/
BOOL xMBPortSerialPutByte(CHAR ucByte)
{
    txByte = ucByte;
    HAL_UART_Transmit_IT(modbusUart, &txByte, 1);
    return TRUE;
}

/* --------------------------------------------------------------------------*/
BOOL xMBPortSerialGetByte(CHAR* pucByte)
{
    *pucByte = rxByte;
    HAL_UART_Receive_IT(modbusUart, &rxByte, 1);
    return TRUE;
}

/* --------------------------------------------------------------------------*/
static void prvvUARTTxReadyISR(void)
{
    pxMBFrameCBTransmitterEmpty();
}

/* --------------------------------------------------------------------------*/
static void prvvUARTRxISR(void)
{
    pxMBFrameCBByteReceived();
}

/* --------------------------------------------------------------------------*/
void UART_TxCplt(UART_HandleTypeDef* huart)
{
        prvvUARTTxReadyISR();
}

/* --------------------------------------------------------------------------*/
void UART_RxCplt(UART_HandleTypeDef* huart)
{
        prvvUARTRxISR();
}

/* --------------------------------------------------------------------------*/
