#ifndef __HAL_INTERRUPT_H__
#define __HAL_INTERRUPT_H__

#include <pdk/device.h>

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