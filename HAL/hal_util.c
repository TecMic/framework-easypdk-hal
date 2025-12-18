/*
 * ======== INCLUDES ========
 */
#include "hal_util.h"


/*
 * ======== DEFINES ========
 */


/*
 * ======== GLOBAL VARIABLES ========
 */


/*
 * ======== LOCAL VARIABLES ========
 */


/*
 * ======== LOCAL FUNCTIONS ========
 */


/*
 * ======== FUNCTIONS ========
 */

// create assembler regions for HAL lib. Will always be compiled
static void UTIL_Init_ASM(void) ASM_CORE
{
__asm
    // dummy IVECT region
    .area IVECT

    // pseudo register for assembler interrupts
    .area PIREG
pI::
    .ds 2
    .globl  pI

    // reserve 12 overlayable bytes in RAM
    .area   OSEG (OVR,DATA)
_OVERLAY_AREA:
    .ds 12
    .globl  _OVERLAY_AREA

    // byte 8-11 of the overlayable can be accessed via math_remain32
    .area   OSEG (OVR,DATA)
_MATH32_SPACER:
    .ds 8
_math_remain32:
    .ds 4
    .globl  _math_remain32

    // reserve 4 bytes at known and aligned position
    .area TMPSEG (ABS,OVR)
_TMP_AREA:
    .ds 4
    .globl _TMP_AREA

    // the _TMP_AREA bytes are also used to store the result of mult/div functions (math_return32)
    .area   TMPSEG (ABS,OVR)
_math_return32:
    .ds 4
    .globl _math_return32

    .area CODE
__endasm;
}

#ifdef USE_GET_CONST_CHAR
// lower byte of pointer in a, upper in p
void getConstChar(void) ASM_CORE
{
__asm
    // Put pointer on stack
    push    af      // Put lower byte of pointer on stack.
    mov.io  a, sp
    add     a, #-1
    xch     a, p
    idxm    p, a // Put upper byte of pointer on stack.

    // Jump to it. ret there will return from the call to _ptrGet
    ret
__endasm;
}
#endif


#ifdef USE_MATH_MULTI8
void multi8(uint8_t n, uint8_t mult) ASM_CORE
{
    n, mult;

#define _ret    _math_return32
#define _n      _multi8_PARM_1
#define _mult   _OVERLAY_AREA+2
#define _i      p
__asm
    clear   _ret+0
    clear   _ret+1

    mov     a, #8
    mov     _i, a

    mov     a, _multi8_PARM_2+0
    mov     _mult, a
    clear   _multi8_PARM_2+0

00001$:
    sr      _mult+0
    t1sn.io f, c
    goto    00002$

    // ret += n
    mov     a, _n+0
    add     _ret+0, a
    mov     a, _n+1
    addc    _ret+1, a

00002$:
    sl      _n+0
    slc     _n+0

    dzsn    _i
    goto    00001$

    ret
__endasm;
#undef _ret
#undef _n
#undef _mult
#undef _i
}
#endif
 
#ifdef USE_MATH_DIV8
void div8(uint8_t n, uint8_t div) ASM_CORE
{
    n, div;

#define _ret    _math_return32
#define _remain _math_remain32
#define _n      _div8_PARM_1
#define _div    _div8_PARM_2
#define _i      p
__asm
    clear   _remain+0

    mov     a, #8
    mov     _i, a

00001$:
    // shift ms-bit of n into remain
    sl      _n+0

    slc     _remain+0

    // shift ret and set ls-Bit to 0
    sl      _ret+0

    // if (remain >= div)
    mov     a, _remain+0
    sub     a, _div+0

    t0sn.io f, c
    goto    00002$ // if c == 1, remain < div, skip if

    // remain -= div;
    mov     _remain+0, a

    // ret |= (1 << i); set ls-Bit to 1
    set1    _ret+0, #0

00002$:
    dzsn    _i
    goto    00001$

    ret
__endasm;
#undef _ret
#undef _remain
#undef _n
#undef _div
#undef _i
}
#endif

#ifdef USE_MATH_MULTI16
void multi16(uint16_t n, uint16_t mult) ASM_CORE
{
    n, mult;

#define _ret    _math_return32
#define _n      _multi16_PARM_1
#define _mult   _OVERLAY_AREA+4
#define _i      p
__asm
    clear   _ret+0
    clear   _ret+1
    clear   _ret+2
    clear   _ret+3

    mov     a, #16
    mov     _i, a

    mov     a, _multi16_PARM_2+0
    mov     _mult+0, a
    clear   _multi16_PARM_2+0
    mov     a, _multi16_PARM_2+1
    mov     _mult+1, a
    clear   _multi16_PARM_2+1

00001$:
    sr      _mult+1
    src     _mult+0
    t1sn.io f, c
    goto    00002$

    // ret += n
    mov     a, _n+0
    add     _ret+0, a
    mov     a, _n+1
    addc    _ret+1, a
    mov     a, _n+2
    addc    _ret+2, a
    mov     a, _n+3
    addc    _ret+3, a

00002$:
    sl      _n+0
    slc     _n+1
    slc     _n+2
    slc     _n+3

    dzsn    _i
    goto    00001$

    ret
__endasm;
#undef _ret
#undef _n
#undef _mult
#undef _i
}
#endif

#ifdef USE_MATH_DIV16
void div16(uint16_t n, uint16_t div) ASM_CORE
{
    n, div;

#define _ret    _math_return32
#define _remain _math_remain32
#define _n      _div16_PARM_1
#define _div    _div16_PARM_2
#define _i      p
__asm
    clear   _remain+0
    clear   _remain+1

    mov     a, #16
    mov     _i, a

00001$:
    // shift ms-bit of n into remain
    sl      _n+0
    slc     _n+1

    slc     _remain+0
    slc     _remain+1

    // shift ret and set ls-Bit to 0
    sl      _ret+0
    slc     _ret+1

    // if (remain >= div)
    mov     a, _remain+0
    sub     a, _div+0
    mov     a, _remain+1
    subc    a, _div+1

    t0sn.io f, c
    goto    00002$ // if c == 1, remain < div, skip if

    // remain -= div;
    mov     a, _div+0
    sub     _remain+0, a
    mov     a, _div+1
    subc    _remain+1, a

    // ret |= (1 << i); set ls-Bit to 1
    set1    _ret+0, #0

00002$:
    dzsn    _i
    goto    00001$

    ret
__endasm;
#undef _ret
#undef _remain
#undef _n
#undef _div
#undef _i
}
#endif

#ifdef USE_MATH_MULTI32
void multi32(uint32_t n, uint32_t mult) ASM_CORE
{
    // uint32_t ret = 0;
    
    // for (uint8_t i = 0; i < 32; ++i)
    // {
    //     if (mult & (1 << i))
    //     {
    //         ret += n;
    //     }
    //     n <<= 1;
    // }

    // overlayed1.mathReturn = ret;

    n, mult;

#define _ret    _math_return32
#define _n      _multi32_PARM_1
#define _mult   _multi32_PARM_2
#define _i      p
__asm
    clear   _ret+0
    clear   _ret+1
    clear   _ret+2
    clear   _ret+3

    mov     a, #32
    mov     _i, a

00001$:
    sr      _mult+3
    src     _mult+2
    src     _mult+1
    src     _mult+0
    t1sn.io f, c
    goto    00002$

    // ret += n
    mov     a, _n+0
    add     _ret+0, a
    mov     a, _n+1
    addc    _ret+1, a
    mov     a, _n+2
    addc    _ret+2, a
    mov     a, _n+3
    addc    _ret+3, a

00002$:
    sl      _n+0
    slc     _n+1
    slc     _n+2
    slc     _n+3

    dzsn    _i
    goto    00001$

    ret
__endasm;
#undef _ret
#undef _n
#undef _mult
#undef _i
}
#endif

#ifdef USE_MATH_DIV32
void div32(uint32_t n, uint32_t div) ASM_CORE
{
    // // Integer division (unsigned) with remainder
    // uint32_t ret = 0;
    // uint16_t remain = 0;
    
    // for(int8_t i = sizeof(uint32_t) * 8 - 1; i >= 0; --i)
    // {
    //     remain <<= 1;
    //     remain |= (n >> i) & 0x01;
        
    //     if (remain >= div)
    //     {
    //         remain -= div;
    //         ret |= (1 << i);
    //     }
    // }
    
    // overlayed1.mathReturn = ret;

    n, div;

#define _ret    _math_return32
#define _remain _math_remain32
#define _n      _div32_PARM_1
#define _div    _div32_PARM_2
#define _i      p
__asm
    clear   _remain+0
    clear   _remain+1
    clear   _remain+2
    clear   _remain+3

    mov     a, #32
    mov     _i, a

00001$:
    // shift ms-bit of n into remain
    sl      _n+0
    slc     _n+1
    slc     _n+2
    slc     _n+3

    slc     _remain+0
    slc     _remain+1
    slc     _remain+2
    slc     _remain+3

    // shift ret and set ls-Bit to 0
    sl      _ret+0
    slc     _ret+1
    slc     _ret+2
    slc 	_ret+3

    // if (remain >= div)
    mov     a, _remain+0
    sub     a, _div+0
    mov     a, _remain+1
    subc    a, _div+1
    mov     a, _remain+2
    subc    a, _div+2
    mov     a, _remain+3
    subc    a, _div+3
    t0sn.io f, c
    goto    00002$ // if c == 1, remain < div, skip if

    // remain -= div;
    mov     a, _div+0
    sub     _remain+0, a
    mov     a, _div+1
    subc    _remain+1, a
    mov     a, _div+2
    subc    _remain+2, a
    mov     a, _div+3
    subc    _remain+3, a

    // ret |= (1 << i); set ls-Bit to 1
    set1    _ret+0, #0

00002$:
    dzsn    _i
    goto    00001$

    ret
__endasm;
#undef _ret
#undef _remain
#undef _n
#undef _div
#undef _i
}
#endif