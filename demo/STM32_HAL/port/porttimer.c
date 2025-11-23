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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "hal_port.h"
#include "mb.h"
#include "mbport.h"
/* ----------------------- Static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Variables ----------------------------------------*/
extern TIM_HandleTypeDef* timer_mb;

uint16_t timerPeriod = 0;
uint16_t timerCounter = 0;

/* ----------------------- Start implementation -----------------------------*/

/*----------------------------------------------------------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    timerPeriod = usTim1Timerout50us;
    MP_DEBUG_PRINT( DEBUG_TRACE, "MBPortTI timer init\n" );

    if ( HAL_TIM_RegisterCallback( timer_mb, HAL_TIM_PERIOD_ELAPSED_CB_ID, Timer_PeriodElapsed ) != HAL_OK )
    {
        MP_DEBUG_PRINT( DEBUG_ERROR, "MBPortTI ERROR reg callback\n" );
    }
    return TRUE;
}

/* --------------------------------------------------------------------------*/
inline void vMBPortTimersEnable()
{
    timerCounter = 0;
    MP_DEBUG_PRINT( DEBUG_TRACE, "MBPortTE timer start\n" );

    if ( HAL_TIM_Base_Start_IT( timer_mb ) != HAL_OK )
    {
        MP_DEBUG_PRINT( DEBUG_ERROR, "MBPortTE ERROR timer start\n" );
    }
}

/* --------------------------------------------------------------------------*/
inline void vMBPortTimersDisable()
{
    MP_DEBUG_PRINT( DEBUG_TRACE, "MBPortTD timer stop\n" );
    if ( HAL_TIM_Base_Stop_IT( timer_mb ) != HAL_OK )
    {
        MP_DEBUG_PRINT( DEBUG_ERROR, "MBPortTD ERROR timer stop\n" );
    }
}

/* --------------------------------------------------------------------------*/
static inline void prvvTIMERExpiredISR( void )
{
    (void) pxMBPortCBTimerExpired();
}

/* --------------------------------------------------------------------------*/
void Timer_PeriodElapsed( TIM_HandleTypeDef* htim )
{
    timerCounter++;

    if ( timerCounter == timerPeriod )
    {
        MP_DEBUG_PRINT( DEBUG_TRACE, "%ld timer expired\n", HAL_GetTick() );
        prvvTIMERExpiredISR();
    }
}

/* --------------------------------------------------------------------------*/
