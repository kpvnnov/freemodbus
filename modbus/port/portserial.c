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
#include "mb.h"
#include "mbport.h"
#include "stm32f0xx_hal.h"

#include "main.h"
#include "stdio.h"

/* ----------------------- Static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);

/* ----------------------- Variables ----------------------------------------*/
extern UART_HandleTypeDef *modbusUart;

uint8_t txByte = 0x00;
uint8_t rxByte = 0x00;

/* ----------------------- Start implementation -----------------------------*/
/* включение и отключение передатчика/приемника. Требуемые режимы задаются аргументами xRxEnable и xTxEnable. */
/*----------------------------------------------------------------------------*/
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable) {
	if (xRxEnable == FALSE) {
		HAL_UART_AbortReceive_IT(modbusUart);
	} else {
		HAL_GPIO_WritePin(USART2_RTS_GPIO_Port, USART2_RTS_Pin, GPIO_PIN_RESET);
		HAL_UART_Receive_IT(modbusUart, &rxByte, 1);
	}

	if (xTxEnable == FALSE) {
		HAL_UART_AbortTransmit_IT(modbusUart);
	} else {

		if (modbusUart->gState == HAL_UART_STATE_READY) {
			// SLAVE_RS485_TRANCE_MODE;
			HAL_GPIO_WritePin(USART2_RTS_GPIO_Port, USART2_RTS_Pin, GPIO_PIN_SET);
			prvvUARTTxReadyISR();
		}
	}
}

/* --------------------------------------------------------------------------*/
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity) {
	return TRUE;
}

/* --------------------------------------------------------------------------*/
/* Функции отправки байта данных */
BOOL xMBPortSerialPutByte(CHAR ucByte) {
	txByte = ucByte;
	HAL_UART_Transmit_IT(modbusUart, &txByte, 1);
	return TRUE;
}
BOOL xMBPortSerialPutBlock(uint8_t *ucByte, uint16_t length) {
	HAL_UART_Transmit_IT(modbusUart, ucByte, length);
	return TRUE;
}

/* --------------------------------------------------------------------------*/
/* Функции получения байта данных */
BOOL xMBPortSerialGetByte(CHAR *pucByte) {
	*pucByte = rxByte;
	HAL_UART_Receive_IT(modbusUart, &rxByte, 1);
	return TRUE;
}

/* --------------------------------------------------------------------------*/
/* 
 По приему:

 Попадаем в callback HAL_UART_RxCpltCallback()
 Оттуда в prvvUARTRxISR()
 Из нее вызываем pxMBFrameCBByteReceived()
 */
static void prvvUARTTxReadyISR(void) {
	pxMBFrameCBTransmitterEmpty();
}

/* --------------------------------------------------------------------------*/
static void prvvUARTRxISR(void) {
	pxMBFrameCBByteReceived();
}

/* --------------------------------------------------------------------------*/
/* По окончанию передачи все аналогично:

 HAL_UART_TxCpltCallback() - HAL вызывается по завершению передачи данных
 prvvUARTTxReadyISR()
 pxMBFrameCBTransmitterEmpty()
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == modbusUart->Instance) {
		prvvUARTTxReadyISR();
	}
}

/* --------------------------------------------------------------------------*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == modbusUart->Instance) {
		prvvUARTRxISR();
	}
}

/* --------------------------------------------------------------------------*/
