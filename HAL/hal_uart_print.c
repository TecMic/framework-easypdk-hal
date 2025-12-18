/*
 * ======== INCLUDES ========
 */
#include "hal_uart_print.h"

#include "hal_util.h"
#include "hal_uart.h"

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

#ifdef USE_PRINT_STRING
void print(uint8_t* string) ASM_CORE
{
    string;

    #define _data   _OVERLAY_AREA+0 // _print_PARM_1
    #define _ptr    _OVERLAY_AREA+2
__asm
    mov     a, _data+0
    mov     _ptr+0, a
    mov     a, _data+1
    mov     _ptr+1, a

00001$:
    mov     a, _ptr+1
    mov     p, a
    mov     a, _ptr+0
    call    _getConstChar

    cneqsn  a, #'\0'
    ret

    call    _UART_Send_Byte

    inc     _ptr+0
    addc    _ptr+1
    goto    00001$
__endasm;
    #undef _data
    #undef _ptr
}

 void println(uint8_t* string) ASM_CORE
{
    string;

__asm
    call    _print

    mov     a, #'\n'
    goto    _UART_Send_Byte
__endasm;
}
#endif

#ifdef USE_PRINT_HX
/* Expects low nibble value in data and in a */
void print_hx_nibble(uint8_t data) ASM_CORE
{
    data;

    #define _data   _print_hx_nibble_PARM_1
__asm
    mov     _data, a
    sub     a, #0x0a
    mov     a, #'0'
    t1sn.io f, c
    add     a, #('A'-'9'-1)
    
    add     a, _data
    goto    _UART_Send_Byte
__endasm;
    #undef _data
}

void print_u8_hx(uint8_t data) ASM_CORE
{
    data;

    #define _data   _print_u8_hx_PARM_1
    #define _tmp    _OVERLAY_AREA+2
__asm
    mov     _tmp, a // store for later

    swap    a
    and     a, #0x0f
    call    _print_hx_nibble

    mov     a, _tmp
    and     a, #0x0f
    goto    _print_hx_nibble
__endasm;
    #undef _data
    #undef _tmp
}
#endif

#ifdef USE_PRINT_U16
const uint16_t divConst[4] = {10, 100, 1000, 10000};

void print_u16(uint16_t data) ASM_CORE
{
    data;

    #define _data       _print_u16_PARM_1
    #define _ret        _math_return32
    #define _remain     _math_remain32
    #define _div        _div16_PARM_2
    #define _printZero  _OVERLAY_AREA+4
    #define _i          _OVERLAY_AREA+5
__asm
    clear   _printZero
    mov     a, #(6)
    mov     _i, a

    goto    00002$

00001$:
    // move remain to new dividend
    mov     a, _remain+0
    mov     _div16_PARM_1+0, a
    mov     a, _remain+1
    mov     _div16_PARM_1+1, a

00002$:
    mov     a, #>(_divConst + 0x8000 + 0)
    mov     p, a
    mov     a, _i
    add     a, #<(_divConst + 0)
    addc    p
    call    _getConstChar
    mov     _div+0, a

    mov     a, #>(_divConst + 0x8000 + 1)
    mov     p, a
    mov     a, _i
    add     a, #<(_divConst + 1)
    addc    p
    call    _getConstChar
    mov     _div+1, a

    call    _div16

    // if (digit was printed || quotient != 0) {print digit}
    mov     a, _ret+0
    or      a, _printZero
    cneqsn  a, #0x00
    goto    00003$

    mov     _printZero, a
    mov     a, _ret+0
    add     a, #'0'
    call    _UART_Send_Byte

00003$:
    mov     a, #0x02
    sub     _i, a
    t1sn.io f, c
    goto    00001$

    // print last remainder
    mov     a, _remain+0
    add     a, #'0'
    goto    _UART_Send_Byte
__endasm;
    #undef _data
    #undef _ret
    #undef _remain
    #undef _div
    #undef _printZero
}
#endif