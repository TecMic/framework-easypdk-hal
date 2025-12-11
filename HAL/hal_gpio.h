#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

#include <pdk/device.h>

#define PA0     (uint8_t)BIT0
#define PA3     (uint8_t)BIT3
#define PA4     (uint8_t)BIT4
#define PA5     (uint8_t)BIT5
#define PA6     (uint8_t)BIT6
#define PA7     (uint8_t)BIT7

typedef enum {
    GPIO_PA0_IT_RISE_FALL   = INTEGS_PA0_BOTH,
    GPIO_PA0_IT_RISING      = INTEGS_PA0_RISING,
    GPIO_PA0_IT_FALLING     = INTEGS_PA0_FALLING,

    GPIO_PB0_IT_RISE_FALL   = INTEGS_PB0_BOTH,
    GPIO_PB0_IT_RISING      = INTEGS_PB0_RISING,
    GPIO_PB0_IT_FALLING     = INTEGS_PB0_FALLING
} GPIO_Pin_EdgeDetection_e;

/* Initialize GPIO pins as inputs or outputs */
#define GPIO_PA_Init_Output(pins)       PAC |= (uint8_t)pins
#define GPIO_PA_Init_Input(pins)        PAC &= ~(uint8_t)pins

/* !!!  PADIER is write only. Setting a single bit will clear the other bits
 *      -> All pins must be configured at the same time !!!
 *      - Enables digital input to read pin state
 *      - Enables wakeup capability
 */
#define GPIO_PA_Enable_Digital_Input(pins)  PADIER = (uint8_t)pins

#define GPIO_PA_Enable_PullUp(pins)     PAPH |= (uint8_t)pins
#define GPIO_PA_Disable_PullUp(pins)    PAPH &= ~((uint8_t)pins)

/* Set GPIO outputs */
#define GPIO_PA_Write_All_Pins(val)     PA = (uint8_t)val
#define GPIO_PA_Write_Low(pins)         PA &= ~((uint8_t)pins)
#define GPIO_PA_Write_High(pins)        PA |= (uint8_t)pins
#define GPIO_PA_Write_Reverse(pins)     PA ^= (uint8_t)pins

/* Read one or multiple pin states */
#define GPIO_PA_Read_All_Pins()         PA
#define GPIO_PA_Read_Pins(pins)         (PA & (uint8_t)pins)

/* Set edge detection for pin interrupt as defined in GPIO_Pin_EdgeDetection_e */
// TODO: write only register. same as PADIER?
#define GPIO_PA_IntEdgeDetect(edge)     INTEGS = (INTEGS & ~3) | edge
#define GPIO_PB_IntEdgeDetect(edge)     INTEGS = (INTEGS & ~(3 << 2)) | edge

/* Open-Drain Pin */
#define GPIO_PA_Init_OD(pins)           do {                            \
                                            GPIO_PA_Init_Input(pins);   \
                                        } while (0)
#define GPIO_PA_Init_OD_PU(pins)        do {                            \
                                            GPIO_PA_Init_OD(pins);      \
                                            GPIO_PA_Enable_PullUp(pins);\
                                        } while (0)

#define GPIO_PA_OD_LOW(pins)            do {                            \
                                            GPIO_PA_Init_Output(pins);  \
                                            GPIO_PA_Write_Low(pins);    \
                                        } while (0)
#define GPIO_PA_OD_HIGH(pins)            do {                           \
                                            GPIO_PA_Init_Input(pins);   \
                                        } while (0)

#endif //__HAL_GPIO_H__