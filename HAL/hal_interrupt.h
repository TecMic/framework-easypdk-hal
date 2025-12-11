#ifndef __HAL_INTERRUPT_H__
#define __HAL_INTERRUPT_H__

#include <pdk/device.h>

typedef enum {
    IT_EDGE_PA0_RISE_FALL   = INTEGS_PA0_BOTH,
    IT_EDGE_PA0_RISING      = INTEGS_PA0_RISING,
    IT_EDGE_PA0_FALLING     = INTEGS_PA0_FALLING,

    IT_EDGE_PB0_RISE_FALL   = INTEGS_PB0_BOTH,
    IT_EDGE_PB0_RISING      = INTEGS_PB0_RISING,
    IT_EDGE_PB0_FALLING     = INTEGS_PB0_FALLING,

    IT_EDGE_TIM16_RISING    = INTEGS_T16_RISING,
    IT_EDGE_TIM16_FALLING   = INTEGS_T16_FALLING
} IT_EdgeDetection_e;

// Set trigger edge for Timer 16, PA0 and PB0 as defined in IT_EdgeDetection_e
// INTEGS is write only. All trigger edges (IT_EdgeDetection_e) must be set at the same time.
#define IT_Set_Trigger_Edge(edge)       INTEGS = edge


#define IT_Get_Active_Interrupts()      (uint8_t)(INTEN & INTRQ)

#define IT_TIM2_Enable_Interrupt()      INTEN |= (uint8_t)INTEN_TM2
#define IT_TIM2_Disable_Interrupt()     INTEN &= ~((uint8_t)INTEN_TM2)
#define IT_TIM2_Check_Interrupt()       (INTRQ & (uint8_t)INTRQ_TM2)
#define IT_TIM2_Check_En_Interrupt()    (IT_Get_Active_Interrupts() & (uint8_t)INTRQ_TM2)
#define IT_TIM2_Clear_Interrupt()       INTRQ &= ~((uint8_t)INTRQ_TM2)

#define IT_TIM3_Enable_Interrupt()      INTEN |= (uint8_t)INTEN_TM3
#define IT_TIM3_Disable_Interrupt()     INTEN &= ~((uint8_t)INTEN_TM3)
#define IT_TIM3_Check_Interrupt()       (INTRQ & (uint8_t)INTRQ_TM3)
#define IT_TIM3_Check_En_Interrupt()    (IT_Get_Active_Interrupts() & (uint8_t)INTRQ_TM3)
#define IT_TIM3_Clear_Interrupt()       INTRQ &= ~((uint8_t)INTRQ_TM3)

#define IT_TIM16_Enable_Interrupt()     INTEN |= (uint8_t)INTEN_T16
#define IT_TIM16_Disable_Interrupt()    INTEN &= ~((uint8_t)INTEN_T16)
#define IT_TIM16_Check_Interrupt()      (INTRQ & (uint8_t)INTRQ_T16)
#define IT_TIM16_Check_En_Interrupt()   (IT_Get_Active_Interrupts() & (uint8_t)INTRQ_T16)
#define IT_TIM16_Clear_Interrupt()      INTRQ &= ~((uint8_t)INTRQ_T16)

#define IT_PA0_Enable_Interrupt()       INTEN |= (uint8_t)INTEN_PA0
#define IT_PA0_Disable_Interrupt()      INTEN &= ~((uint8_t)INTEN_PA0)
#define IT_PA0_Check_Interrupt()        (INTRQ & (uint8_t)INTRQ_PA0)
#define IT_PA0_Check_En_Interrupt()     (IT_Get_Active_Interrupts() & (uint8_t)INTRQ_PA0)
#define IT_PA0_Clear_Interrupt()        INTRQ &= ~((uint8_t)INTRQ_PA0)

#define IT_PB0_Enable_Interrupt()       INTEN |= (uint8_t)INTEN_PB0
#define IT_PB0_Disable_Interrupt()      INTEN &= ~((uint8_t)INTEN_PB0)
#define IT_PB0_Check_Interrupt()        (INTRQ & (uint8_t)INTRQ_PB0)
#define IT_PB0_Check_En_Interrupt()     (IT_Get_Active_Interrupts() & (uint8_t)INTRQ_PB0)
#define IT_PB0_Clear_Interrupt()        INTRQ &= ~((uint8_t)INTRQ_PB0)

#define IT_Enable_Interrupts()          __engint()
#define IT_Disable_Interrupts()         __disgint()

// Place interrupt function directly at interrupt entry address
// For this, the code has to be pushed behind the interrupt function manually
//   board_build.code_loc = 0x0080
// ! Doesn't push p, so only use with asm !
#define INTERRUPT_FUNCTION_ASM_START() __asm__( \
    ".area	IVECT \n" \
    "push af \n"            \
    )
#define INTERRUPT_FUNCTION_ASM_END() __asm__( \
    "pop af \n" \
    "reti \n" \
    ".area CODE \n" \
    )

#endif //__HAL_INTERRUPT_H__