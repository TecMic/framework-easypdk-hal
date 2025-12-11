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
 * @brief   Set the start and timeout parameters to timeout struct
 *
 * @param1  Timeout element which stores the timeout parameters
 * @return  -
 */
void SYS_TIME_Set_Timeout(Timeout_t *TimeoutElement, uint16_t timeout) ASM_CORE
{
    TimeoutElement, timeout;

#define _ptrTimeoutElement  _SYS_TIME_Set_Timeout_PARM_1
#define _timeout            _SYS_TIME_Set_Timeout_PARM_2

__asm
    mov     a, _ptrTimeoutElement+0
    mov     p, a

    disgint
    mov     a, _upTime+0
    idxm    p, a            // lsb TimeoutElement.startTime
    mov	    a, _upTime+1
    engint

    inc     p
    idxm    p, a            // msb TimeoutElement.startTime

    mov     a, _timeout+0
    inc     p
    idxm    p, a            // lsb TimeoutElement.timeout

    mov     a, _timeout+1
    inc     p
    idxm    p, a            // msb TimeoutElement.timeout
    ret
__endasm;

#undef _ptrTimeoutElement
#undef _timeout
}

/*
 * @brief   Checks if previous set timeout has expired
 *
 * @param1  Timeout element which stores the timeout parameters
 * @return  true if time is elapsed, false if not yet elapsed (if not set, returns true)
 */
bool SYS_TIME_Check_Timeout(Timeout_t *TimeoutElement) ASM_CORE
{
    TimeoutElement;

#define _ptrTimeoutElement  _OVERLAY_AREA // _SYS_TIME_Check_Timeout_PARM_1
#define _startTime          _OVERLAY_AREA+2
#define _timeout            _OVERLAY_AREA+4
#define _tmp                _OVERLAY_AREA+6

__asm
    mov     a, _ptrTimeoutElement+0
    mov     p, a

    idxm    a, p            // lsb TimeoutElement.startTime
    mov     _startTime+0, a
    inc     p
    idxm    a, p            // msb TimeoutElement.startTime
    mov     _startTime+1, a
    inc     p

    idxm    a, p            // lsb TimeoutElement.timeout
    mov     _timeout+0, a
    inc     p
    idxm    a, p            // msb TimeoutElement.timeout
    mov     _timeout+1, a

    disgint
    mov     a, _upTime+1
    mov     _tmp, a
    mov     a, _upTime+0
    engint

    sub     a, _startTime+0 // lsb upTime - lsb startTime
    xch     _tmp            // diff lsb to tmp, msb upTime to a
    subc    a, _startTime+1 // msb upTime - msb startTime - c
    xch     _tmp            // diff lsb to a, diff msb to tmp
    sub     a, _timeout+0   // diff lsb - timeout lsb
    mov     a, _tmp
    subc    a, _timeout+1   // diff msb - timeout msb - c
    t0sn.io f, c            // skip next if c == 0 (only 1 if diff < delay)
    ret     #0x00

    mov     a, #0x00
    idxm    p, a            // clear msb timeout
    dec     p
    idxm    p, a            // clear lsb timeout
    ret     #0x01
__endasm;

#undef _ptrTimeoutElement
#undef _startTime
#undef _timeout
#undef _tmp
}

/*
 * @brief   Checks if timeout has been set
 *
 * @param1  Timeout element which stores the timeout parameters
 * @return  true if timeout was set, false if not set
 */
bool SYS_TIME_Is_Timeout_Set(Timeout_t *TimeoutElement) ASM_CORE
{
    TimeoutElement;

#define _ptrTimeoutElement  _OVERLAY_AREA // _SYS_TIME_Is_Timeout_Set_PARM_1
#define _tmp                _OVERLAY_AREA+2

__asm
    mov     a, _ptrTimeoutElement+0
    add     a, #0x02
    mov     p, a

    idxm    a, p            // lsb TimeoutElement.timeout
    mov     _tmp, a
    inc     p
    idxm    a, p            // msb TimeoutElement.timeout
    and     _tmp, a
    cneqsn  a, #0x00
    ret     #0x00

    ret     #0x01
__endasm;

#undef _ptrTimeoutElement
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