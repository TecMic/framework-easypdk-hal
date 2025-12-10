#ifndef __HAL_UTIL_H__
#define __HAL_UTIL_H__

/***********************************************************
 * INCLUDES
 ***********************************************************/
#include <stdint.h>
#include <pdk/device.h>
#include "hal_interrupt.h"

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
    #define INTERRUPT_FUNCTION_ASM_START()
    #define INTERRUPT_FUNCTION_ASM_END()
#else
    #define __SFRDEF(a,b)               __sfr __at b a;
    #define VOLATILE_AT(a)              volatile __at(a)
    #define VARIABLES_TO_STACK          __reentrant
    #define OVERLAYABLE                 __data volatile
    #define ASM_CORE                    __naked
    #define INTERRUPT_FUNCTION(a)       interrupt(a) __interrupt(0)
    #define INTERRUPT_FUNCTION_ASM(a)   INTERRUPT_FUNCTION(a) ASM_CORE
    #define INTERRUPT_FUNCTION_ASM_START() __asm__( \
                            ".area	IVECT \n" \
                            "push af \n"            \
                            )
    #define INTERRUPT_FUNCTION_ASM_END() __asm__( \
                            "pop af \n" \
                            "reti \n" \
                            ".area CODE \n" \
                            )
#endif

/***********************************************************
 * TYPEDEFS
 ***********************************************************/
typedef enum {LOW = 0, HIGH = !LOW} GPIO_State_e;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState_e;

/***********************************************************
 * EXPORTED VARIABLES
 ***********************************************************/


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

#endif //__HAL_UTIL_H__