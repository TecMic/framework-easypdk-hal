#ifndef __HAL_CONFIG_H__
#define __HAL_CONFIG_H__

/*
 *  To use this config file in platformIO, put into project_dir/include
 */

/********************
 * USER DEFINE BEGIN 
 ********************/

/* UART */

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

// TX Pin. Must be defined as PA or! PB
#define UART_TX_PA              7
// #define UART_TX_PB              3

// RX Pin. Must be defined as PA or! PB
#define UART_RX_PA              0
// #define UART_RX_PB              0

// UART Baud: 9600, 19200, 38400, 57600, 115200
#define UART_BAUD               9600

// UART Buffer size. Size of ringbuffer for each RX and TX
#define UART_DATA_BUFFER_SIZE   8

// Configure TX Pin as open-drain pin with pull-up (weak pull-up)
// #define UART_TX_TYPE_OD

// UART - used modules. Comment if not used to save RAM and memory
#define USE_UART_RX 
#define USE_UART_TX


/* UART Print */

// UART Print - used modules. Comment if not used to save RAM and memory
#define USE_PRINT_HX
#define USE_PRINT_U16
#define USE_PRINT_STRING


/* Helper-Util Functions */

/*
Util - used modules. Uncomment if used in application code.
- HAL functions will handle it itself if needed.
*/
// #define USE_GET_CONST_CHAR

// #define USE_MATH_MULTI8
// #define USE_MATH_DIV8
// #define USE_MATH_MULTI16
// #define USE_MATH_DIV16
// #define USE_MATH_MULTI32
// #define USE_MATH_DIV32


/********************
 * USER DEFINE END 
 ********************/


/********************
 * HAL DEFINES 
 ********************/

#ifdef USE_PRINT_STRING
    #define USE_GET_CONST_CHAR
#endif

#ifdef USE_PRINT_U16
    #define USE_MATH_DIV16
#endif

#endif //__HAL_CONFIG_H__