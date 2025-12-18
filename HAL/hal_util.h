#ifndef __HAL_UTIL_H__
#define __HAL_UTIL_H__

/***********************************************************
 * INCLUDES
 ***********************************************************/
#include <stdint.h>
#include <pdk/device.h>
#include "hal_interrupt.h"
#include "hal_config.h"

/***********************************************************
 * DEFINES
 ***********************************************************/
#ifdef __INTELLISENSE__
    #define __SFRDEF(a,b)               volatile unsigned char a;
    #define VOLATILE_AT(a)              volatile
    #define VARIABLES_TO_STACK
    #define OVERLAYABLE
    #define ASM_CORE
    #define INTERRUPT_FUNCTION(a)       interrupt(a)
    #define INTERRUPT_FUNCTION_ASM(a)   INTERRUPT_FUNCTION(a)
#else
    #define __SFRDEF(a,b)               __sfr __at b a;
    #define VOLATILE_AT(a)              volatile __at(a)
    #define VARIABLES_TO_STACK          __reentrant
    #define OVERLAYABLE                 __data volatile
    #define ASM_CORE                    __naked
    #define INTERRUPT_FUNCTION(a)       interrupt(a) __interrupt(0)
    #define INTERRUPT_FUNCTION_ASM(a)   INTERRUPT_FUNCTION(a) ASM_CORE
#endif

#define BIT(bit)   (1 << bit)

/***********************************************************
 * TYPEDEFS
 ***********************************************************/
typedef enum {LOW = 0, HIGH = !LOW} GPIO_State_e;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState_e;

/***********************************************************
 * EXPORTED VARIABLES
 ***********************************************************/
extern volatile uint32_t math_return32;
extern volatile uint32_t math_remain32;

/***********************************************************
 * MAKROS
 ***********************************************************/


/***********************************************************
 * FUNCTIONS
 ***********************************************************/
inline uint16_t Safe_Read_U16(uint16_t* ptr)
{
    uint16_t ret;

    IT_Disable_Interrupts();
    ret = *ptr;
    IT_Enable_Interrupts();
    return ret;
}

#ifdef USE_GET_CONST_CHAR
void getConstChar(void); // use in asm only!
#endif

#ifdef USE_MATH_MULTI8
// result in math_return32 as ui8
void multi8(uint8_t n, uint8_t mult);
#endif
#ifdef USE_MATH_DIV8
// result in math_return32 as ui8, remainder (modulo) in math_remain32 as ui8
void div8(uint8_t n, uint8_t div);
#endif

#ifdef USE_MATH_MULTI16
// result in math_return32 as ui16
void multi16(uint16_t n, uint16_t mult);
#endif
#ifdef USE_MATH_DIV16
// result in math_return32 as ui16, remainder (modulo) in math_remain32 as ui16
void div16(uint16_t n, uint16_t div);
#endif

#ifdef USE_MATH_MULTI32
// result in math_return32 as ui32
void multi32(uint32_t n, uint32_t mult);
#endif
#ifdef USE_MATH_DIV32
// result in math_return32 as ui16, remainder (modulo) in math_remain32 as ui32
void div32(uint32_t n, uint32_t div);
#endif

#endif //__HAL_UTIL_H__