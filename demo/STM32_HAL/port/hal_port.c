/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 * @authors        : Peter Kostenko https://t.me/kpvnnov kpvnnov@gmail.com
 ******************************************************************************
 * @attention
 * All rights reserved.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "hal_port.h"

/* Declarations and definitions ----------------------------------------------*/
static uint32_t PriMsk;
UART_HandleTypeDef* uart_mb;
TIM_HandleTypeDef*  timer_mb;

/* Functions -----------------------------------------------------------------*/
void EnterCriticalSection()
{
    PriMsk = __get_PRIMASK();
    __set_PRIMASK(1);
}

/*----------------------------------------------------------------------------*/
void ExitCriticalSection()
{
    __set_PRIMASK(PriMsk);
}

/*----------------------------------------------------------------------------*/
void HAL_PORT_SetTimerModule(TIM_HandleTypeDef* timer)
{
    timer_mb = timer;
}

/*----------------------------------------------------------------------------*/
void HAL_PORT_SetUartModule(UART_HandleTypeDef* uart)
{
    uart_mb = uart;
}

/*----------------------------------------------------------------------------*/
