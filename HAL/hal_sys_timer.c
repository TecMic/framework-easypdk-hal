/*
 * ======== INCLUDES ========
 */
#include "hal_sys_timer.h"
#include "hal_util.h"
#include "hal_timer.h"

/*
 * ======== DEFINES ========
 */


/*
 * ======== GLOBAL VARIABLES ========
 */


/*
 * ======== LOCAL VARIABLES ========
 */
volatile uint16_t upTime; // ms

/*
 * ======== LOCAL FUNCTIONS ========
 */


/*
 * ======== FUNCTIONS ========
 */

/*
 * @brief   Init sysTick timer for interrupt every 1 ms
 *
 * @param1  -
 * @return  -
 */
void SYS_TIME_Init(void)
{
#ifdef SYS_TIME_USE_TIM2
    // f = 1000.00 Hz (1.00 ms)
    TIM2_Set_Conrol(TIM2_3_CLK_SRC_IHRC);
    TIM2_Set_Scale(TIM2_3_PRESCALE_DIV4 | TIM2_3_SCALE_DIV16);
    TIM2_Set_Period(249);
    IT_TIM2_Enable_Interrupt();
#else
    // f = 976.56 Hz (1.02 ms)
    TIM16_Set_Control((uint8_t)(TIM16_CLK_SRC_IHRC | TIM16_PRESCALE_DIV16 | TIM16_INT_ON_CHANGE_BIT9));
    IT_TIM16_Enable_Interrupt();
#endif
}

/*
 * @brief   Set the time parameters to an ElapseTime element
 *
 * @param1  ElapseTime element which stores the time parameters
 * @return  -
 */
void SYS_TIME_Set_Elapse_Time(ElapseTime_t *DelayElement, uint16_t delayTime) ASM_CORE
{
    DelayElement, delayTime;

#define _ptrDelayElement    _SYS_TIME_Set_Elapse_Time_PARM_1
#define _delayTime          _SYS_TIME_Set_Elapse_Time_PARM_2

__asm
    mov     a, _ptrDelayElement+0
    mov     p, a

    disgint
    mov     a, _upTime+0
    idxm    p, a            // lsb ElapseTime_t.startTime
    mov	    a, _upTime+1
    engint

    inc     p
    idxm    p, a            // msb ElapseTime_t.startTime

    mov     a, _delayTime+0
    inc     p
    idxm    p, a            // lsb ElapseTime_t.delay

    mov     a, _delayTime+1
    inc     p
    idxm    p, a            // msb ElapseTime_t.delay
    ret
__endasm;

#undef _ptrDelayElement
#undef _delayTime
}

/*
 * @brief   Checks if previous set time has elapsed
 *
 * @param1  ElapseTime element which stores the time parameters
 * @return  true if time is elapsed, false if not yet elapsed
 */
bool SYS_TIME_Is_Elapsed(ElapseTime_t *DelayElement) ASM_CORE
{
    DelayElement;

#define _ptrDelayElement    _OVERLAY_AREA // _SYS_TIME_Is_Elapsed_PARM_1
#define _startTime          _OVERLAY_AREA+2
#define _delayTime          _OVERLAY_AREA+4
#define _tmp                _OVERLAY_AREA+6

__asm
    mov     a, _ptrDelayElement+0
    mov     p, a

    idxm    a, p            // lsb ElapseTime_t.startTime
    mov     _startTime+0, a
    inc     p
    idxm    a, p            // msb ElapseTime_t.startTime
    mov     _startTime+1, a
    inc     p

    idxm    a, p            // lsb ElapseTime_t.delay
    mov     _delayTime+0, a
    inc     p
    idxm    a, p            // msb ElapseTime_t.delay
    mov     _delayTime+1, a

    disgint
    mov     a, _upTime+1
    mov     _tmp, a
    mov     a, _upTime+0
    engint

    sub     a, _startTime+0 // lsb upTime - lsb startTime
    xch     _tmp            // diff lsb to tmp, msb upTime to a
    subc    a, _startTime+1 // msb upTime - msb startTime - c
    xch     _tmp            // diff lsb to a, diff msb to tmp
    sub     a, _delayTime+0 // diff lsb - delay lsb
    mov     a, _tmp
    subc    a, _delayTime+1 // diff msb - delay msb - c
    t0sn.io f, c            // skip next if c == 0 (only 1 if diff < delay)
    ret     #0x00

    mov     a, #0x00
    idxm    p, a            // clear msb delay
    dec     p
    idxm    p, a            // clear msb delay
    ret     #0x01
__endasm;

#undef _ptrDelayElement
#undef _startTime
#undef _delayTime
}

/*
 * @brief   Delay for some milliseconds (blocking)
 *
 * @param1  Delay time in ms
 * @return  -
 */
void Delay_ms(uint16_t delay) ASM_CORE
{
    delay;

#define _delay      _OVERLAY_AREA // _Delay_ms_PARM_1
#define _startTime  _OVERLAY_AREA+2

__asm
    disgint
    mov     a, _upTime+0
    mov	    _startTime+0, a
    mov	    a, _upTime+1
    engint
    mov	    _startTime+1, a

00001$:
    disgint
    mov     a, _upTime+1
    mov     p, a
    mov     a, _upTime+0
    engint

    sub     a, _startTime+0     // lsb upTime - lsb startTime
    xch     p                   // diff lsb to p, msb upTime to a
    subc    a, _startTime+1     // msb upTime - msb startTime - c
    xch     p                   // diff lsb to a, diff msb to p
    sub     a, _delay+0         // diff lsb - delay lsb
    mov     a, p
    subc    a, _delay+1         // diff msb - delay msb - c
    t0sn.io f, c                // skip next if c == 0 (only 1 if diff < delay)
    goto    00001$

    ret
__endasm;

#undef _delay
#undef _startTime
}