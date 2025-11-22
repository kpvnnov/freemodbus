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

#ifndef __STM32_HAL_PORT_H
#define __STM32_HAL_PORT_H

#ifndef __HAL_ENTER_CRITICAL_SECTION
#define __HAL_ENTER_CRITICAL_SECTION()                                                                                 \
    uint32_t PriMsk;                                                                                                   \
    PriMsk = __get_PRIMASK();                                                                                          \
    __set_PRIMASK(1);
#endif

#ifndef __HAL_EXIT_CRITICAL_SECTION
#define __HAL_EXIT_CRITICAL_SECTION() __set_PRIMASK(PriMsk);
#endif
/* Includes ------------------------------------------------------------------*/

/* Detect STM32 family and include appropriate HAL */
#if defined(STM32F0xx)
  #include "stm32f0xx_hal.h"
#elif defined(STM32F1xx)
  #include "stm32f1xx_hal.h"
#elif defined(STM32F3xx)
  #include "stm32f3xx_hal.h"
#elif defined(STM32G4xx)
  #include "stm32g4xx_hal.h"
#elif defined(STM32F4xx)
  #include "stm32f4xx_hal.h"
#elif defined(STM32WLxx)
  #include "stm32wlxx_hal.h"
/* Add other STM32 families as needed */
#else
  #error "No STM32 device family defined!"
#endif

/* Declarations and definitions ----------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

extern void HAL_PORT_SetTimerModule(TIM_HandleTypeDef *timer);
extern void HAL_PORT_SetUartModule(UART_HandleTypeDef *uart);
extern void UART_RxCplt(UART_HandleTypeDef* huart);
extern void UART_TxCplt(UART_HandleTypeDef* huart);
extern void Timer_PeriodElapsed(TIM_HandleTypeDef *htim);

#endif // #ifndef HAL_PORT_H
