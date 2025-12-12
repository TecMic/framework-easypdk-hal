#ifndef __HAL_SYS_TIMER_H__
#define __HAL_SYS_TIMER_H__

/***********************************************************
 * INCLUDES
 ***********************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <pdk/device.h>

#include "hal_interrupt.h"

/***********************************************************
 * DEFINES
 ***********************************************************/
// Define in project config use more precice timing
// #define SYS_TIME_USE_TIM2

/***********************************************************
 * TYPEDEFS
 ***********************************************************/
typedef struct {
    uint16_t startTime;
    uint16_t timeout;
} Timeout_t;

/***********************************************************
 * EXPORTED VARIABLES
 ***********************************************************/
extern volatile uint16_t upTime; // ms

/***********************************************************
 * MAKROS
 ***********************************************************/
#define SYS_TIME_Count_Up()         ++upTime
#define SYS_TIME_Get_Time()         upTime

#ifdef SYS_TIME_USE_TIM2
    #define SYS_TIME_Check_Interrupt()      IT_TIM2_Check_Interrupt()
    #define SYS_TIME_Check_En_Interrupt()   IT_TIM2_Check_En_Interrupt()
    #define SYS_TIME_Clear_Interrupt()      IT_TIM2_Clear_Interrupt()
    #define SYS_TIME_Tick_Handler() \
        __asm__( \
            "t1sn.io    __intrq, #"_STR(INTRQ_TM2_BIT)" \n" \
            "goto 00099$                                \n" \
            "set0.io    __intrq, #"_STR(INTRQ_TM2_BIT)" \n" \
            "inc        _upTime+0                       \n" \
            "addc       _upTime+1                       \n" \
            "00099$:                                    \n" \
        )
#else
    #define SYS_TIME_Check_Interrupt()      IT_TIM16_Check_Interrupt()
    #define SYS_TIME_Check_En_Interrupt()   IT_TIM16_Check_En_Interrupt()
    #define SYS_TIME_Clear_Interrupt()      IT_TIM16_Clear_Interrupt()
    #define SYS_TIME_Tick_Handler() \
        __asm__( \
            "t1sn.io    __intrq, #"_STR(INTRQ_T16_BIT)" \n" \
            "goto 00099$                                \n" \
            "set0.io    __intrq, #"_STR(INTRQ_T16_BIT)" \n" \
            "inc        _upTime+0                       \n" \
            "addc       _upTime+1                       \n" \
            "00099$:                                    \n" \
        )
#endif

/***********************************************************
 * FUNCTIONS
 ***********************************************************/
void SYS_TIME_Init(void);
void SYS_TIME_Set_Timeout(Timeout_t *TimeoutElement, uint16_t timeout);
bool SYS_TIME_Check_Timeout(Timeout_t *TimeoutElement);
bool SYS_TIME_Is_Timeout_Set(Timeout_t *TimeoutElement);
void Delay_ms(uint16_t delay);

#endif // __HAL_SYS_TIMER_H__