#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

#include <pdk/device.h>

#define PA0     (uint8_t)BIT0
#define PA3     (uint8_t)BIT3
#define PA4     (uint8_t)BIT4
#define PA5     (uint8_t)BIT5
#define PA6     (uint8_t)BIT6
#define PA7     (uint8_t)BIT7


/* Initialize GPIO pins as inputs or outputs */
#define GPIO_PA_Init_Output(pins)       PAC |= (uint8_t)pins
#define GPIO_PA_Init_Input(pins)        PAC &= ~(uint8_t)pins

/* Enable digital input for a pin to read input state and enable wakeup capability 
 * PADIER is write only. Enable/disable for single pin only (sdcc will compile it to single bit set/clear)
 */
#define GPIO_PA_Enable_Digital_Input(pin)  PADIER |= (uint8_t)pin
#define GPIO_PA_Disable_Digital_Input(pin) PADIER &= ~((uint8_t)pin)

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