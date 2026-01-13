#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

#include "hal_util.h"


/* Pin definitions */
#define PA0     0
#define PA3     3
#define PA4     4
#define PA5     5
#define PA6     6
#define PA7     7

#define PB0     0
#define PB1     1
#define PB2     2
#define PB3     3
#define PB4     4
#define PB5     5
#define PB6     6
#define PB7     7

/* Initialize GPIO pins as inputs or outputs */
#define GPIO_PA_Init_Output(pin)    PAC |= (uint8_t)BIT(pin)
#define GPIO_PB_Init_Output(pin)    PBC |= (uint8_t)BIT(pin)

#define GPIO_PA_Init_Input(pin)     PAC &= ~(uint8_t)BIT(pin)
#define GPIO_PB_Init_Input(pin)     PBC &= ~(uint8_t)BIT(pin)

/* Enable digital input for a pin to read input state and enable wakeup capability 
 * PADIER is write only. Enable/disable for single pin only (sdcc will compile it to single bit set/clear)
 */
#define GPIO_PA_Enable_Digital_Input(pin)  PADIER |= (uint8_t)BIT(pin)
#define GPIO_PB_Enable_Digital_Input(pin)  PBDIER |= (uint8_t)BIT(pin)

#define GPIO_PA_Disable_Digital_Input(pin) PADIER &= ~((uint8_t)BIT(pin))
#define GPIO_PB_Disable_Digital_Input(pin) PBDIER &= ~((uint8_t)BIT(pin))

/* Enable Pull-Up */
#define GPIO_PA_Enable_PullUp(pins)     PAPH |= (uint8_t)BIT(pin)
#define GPIO_PB_Enable_PullUp(pins)     PBPH |= (uint8_t)BIT(pin)

#define GPIO_PA_Disable_PullUp(pins)    PAPH &= ~((uint8_t)BIT(pin))
#define GPIO_PB_Disable_PullUp(pins)    PBPH &= ~((uint8_t)BIT(pin))

/* Set GPIO outputs */
#define GPIO_PA_Write_All_Pins(val)     PA = (uint8_t)val
#define GPIO_PB_Write_All_Pins(val)     PB = (uint8_t)val

#define GPIO_PA_Write_Low(pin)          PA &= ~((uint8_t)BIT(pin))
#define GPIO_PB_Write_Low(pin)          PB &= ~((uint8_t)BIT(pin))

#define GPIO_PA_Write_High(pin)         PA |= (uint8_t)BIT(pin)
#define GPIO_PB_Write_High(pin)         PB |= (uint8_t)BIT(pin)

#define GPIO_PA_Write_Reverse(pin)      __asm__( \
                                            "mov        a, #"_STR(pin)" \n" \
                                            "xor.io     __pa, a         \n" \
                                        )
#define GPIO_PB_Write_Reverse(pin)      __asm__( \
                                            "mov        a, #"_STR(pin)" \n" \
                                            "xor.io     __pb, a         \n" \
                                        )

/* Read one or multiple pin states */
#define GPIO_PA_Read_All_Pins()         PA
#define GPIO_PB_Read_All_Pins()         PB
#define GPIO_PA_Read_Pin(pin)           (PA & (uint8_t)BIT(pin))
#define GPIO_PB_Read_Pin(pin)           (PB & (uint8_t)BIT(pin))

/* Open-Drain Pin */
#define GPIO_PA_Init_OD(pin)    do {                            \
                                    GPIO_PA_Init_Input(pin);    \
                                } while (0)
#define GPIO_PB_Init_OD(pin)    do {                            \
                                    GPIO_PB_Init_Input(pin);    \
                                } while (0)

#define GPIO_PA_Init_OD_PU(pin) do {                            \
                                    GPIO_PA_Init_OD(pin);       \
                                    GPIO_PA_Enable_PullUp(pin); \
                                } while (0)
#define GPIO_PB_Init_OD_PU(pin) do {                            \
                                    GPIO_PB_Init_OD(pin);       \
                                    GPIO_PB_Enable_PullUp(pin); \
                                } while (0)


#define GPIO_PA_OD_LOW(pin)     do {                            \
                                    GPIO_PA_Init_Output(pin);   \
                                    GPIO_PA_Write_Low(pin);     \
                                } while (0)
#define GPIO_PB_OD_LOW(pin)     do {                            \
                                    GPIO_PB_Init_Output(pin);   \
                                    GPIO_PB_Write_Low(pin);     \
                                } while (0)

#define GPIO_PA_OD_HIGH(pin)    do {                            \
                                    GPIO_PA_Init_Input(pin);    \
                                } while (0)
#define GPIO_PB_OD_HIGH(pin)    do {                            \
                                    GPIO_PB_Init_Input(pin);    \
                                } while (0)

#endif //__HAL_GPIO_H__