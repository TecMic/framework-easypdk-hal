#ifndef __HAL_UART_H__
#define __HAL_UART_H__

/*
Usage:

Recommended clock speeds:
115200 baud:
- RX and TX: 16 MHz (5 V)
- RX or TX: 8 MHz

57600 baud:
- RX and TX: 8 MHz
- RX or TX: 4 MHz

38400 baud:
- RX and TX: 4 MHz
- RX or TX: 2 MHz

19200 baud:
- RX and TX: 2 MHz
- RX or TX: 1 MHz

9600 baud:
- RX and TX: 1 MHz
- RX or TX: 1 MHz

Use global defines in platformio.ini:

- Configure tx pin as open-drain (includes pull up on rx pin)
    #define UART_TX_TYPE_OD

- Use pin interrupt for RX. Only available on PA0
    #define UART_USE_RX_PA0_IT
*/

/***********************************************************
 * INCLUDES
 ***********************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <pdk/device.h>

/***********************************************************
 * DEFINES
 ***********************************************************/
#ifndef TX
    #define TX          7
#endif

#ifndef RX
    #define RX          6
#endif

#ifndef UART_BAUD
    #define UART_BAUD   9600
#endif

// must be 2^n (2, 4, 8, ...)
#ifndef UART_DATA_BUFFER_SIZE
    #define UART_DATA_BUFFER_SIZE   8
#endif


#ifndef UART_USE_RX
    #define UART_USE_RX     1
#endif
#ifndef UART_USE_TX
    #define UART_USE_TX     1
#endif


#define UART_INTERRUPT_BIT_PA   INTRQ_PA0_BIT
#define UART_INTERRUPT_BIT_RX   INTRQ_TM2_BIT
#define UART_INTERRUPT_BIT_TX   INTRQ_TM3_BIT

/***********************************************************
 * TYPEDEFS
 ***********************************************************/


/***********************************************************
 * EXPORTED VARIABLES
 ***********************************************************/


/***********************************************************
 * MAKROS
 ***********************************************************/

#ifdef UART_USE_RX_PA0_IT
#define UART_Interrupt_RX_Handler() \
    __asm__( \
        "t0sn.io    __intrq, #"_STR(UART_INTERRUPT_BIT_RX)" \n" \
        "call       _UART_Handle_Interrupt_RX               \n" \
        "t0sn.io    __inten, #"_STR(UART_INTERRUPT_BIT_PA)" \n" \
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