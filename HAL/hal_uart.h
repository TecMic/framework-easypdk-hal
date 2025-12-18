#ifndef __HAL_UART_H__
#define __HAL_UART_H__

/*
Recommended clock speeds:
115200 baud:
- RX and TX: 16 MHz (5 V - not officially supported)
- RX or TX: 8 MHz

57600 baud:
- RX and TX: 8 MHz
- RX or TX: 4 MHz

38400 baud:
- RX and TX: 8 MHz
- RX or TX: 4 MHz

19200 baud:
- RX and TX: 4 MHz
- RX or TX: 2 MHz

9600 baud:
- RX and TX: 2 MHz
- RX or TX: 1 MHz
*/

/***********************************************************
 * INCLUDES
 ***********************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <pdk/device.h>

#include "hal_config.h"

/***********************************************************
 * DEFINES
 ***********************************************************/
#if !defined(UART_TX_PA) && !defined(UART_TX_PB)
    #define UART_TX_PA  7
#endif

#if !defined(UART_RX_PA) && !defined(UART_RX_PB)
    #define UART_RX_PA  0
#endif

#ifndef UART_BAUD
    #define UART_BAUD   9600
#endif

// must be 2^n (2, 4, 8, ...)
#ifndef UART_DATA_BUFFER_SIZE
    #define UART_DATA_BUFFER_SIZE   8
#endif


/* End of user defines */

#if defined(UART_TX_PA)
    #define UART_TX_PIN_BIT         UART_TX_PA
#else
    #define UART_TX_PIN_BIT         UART_TX_PB
#endif

#if defined(UART_RX_PA)
    #define UART_PIN_BIT_RX         UART_RX_PA
    #define UART_INTERRUPT_BIT_PIN  INTRQ_PA0_BIT
#else
    #define UART_PIN_BIT_TX         UART_RX_PB
    #define UART_INTERRUPT_BIT_PIN  INTRQ_PB0_BIT
#endif

#define UART_INTERRUPT_BIT_RX       INTRQ_TM2_BIT
#define UART_INTERRUPT_BIT_TX       INTRQ_TM3_BIT

/***********************************************************
 * TYPEDEFS
 ***********************************************************/


/***********************************************************
 * EXPORTED VARIABLES
 ***********************************************************/


/***********************************************************
 * MAKROS
 ***********************************************************/
#if (UART_PIN_BIT_RX == 0)
#define UART_Interrupt_RX_Handler() \
    __asm__( \
        "t0sn.io    __intrq, #"_STR(UART_INTERRUPT_BIT_RX)" \n" \
        "call       _UART_Handle_Interrupt_RX               \n" \
        "t0sn.io    __inten, #"_STR(UART_INTERRUPT_BIT_PIN)"\n" \
        "call       _UART_Handle_Interrupt_RX_Pin           \n" \
    )
#else
#define UART_Interrupt_RX_Handler() \
    __asm__( \
        "t0sn.io    __intrq, #"_STR(UART_INTERRUPT_BIT_RX)" \n" \
        "call       _UART_Handle_Interrupt_RX               \n" \
    )
#endif

#define UART_Interrupt_TX_Handler() \
    __asm__( \
        "t0sn.io    __intrq, #"_STR(UART_INTERRUPT_BIT_TX)" \n" \
        "call       _UART_Handle_Interrupt_TX               \n" \
    )

/***********************************************************
 * FUNCTIONS
 ***********************************************************/
void UART_Init_TX(void);
void UART_Init_RX(void);
void UART_Send_Byte(uint8_t byte);
void UART_Send_Data(uint8_t* data, uint8_t size);
bool UART_Is_RX_Available(void);
uint8_t UART_Get_Byte(void);

void UART_Handle_Interrupt_RX_Pin(void);
void UART_Handle_Interrupt_RX(void);
void UART_Handle_Interrupt_TX(void);

#endif /* __HAL_UART_H__ */