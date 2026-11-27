/*
 * ======== INCLUDES ========
 */
#include "hal_spi.h"

#include "hal_gpio.h"

/*
 * ======== DEFINES ========
 */

// Pin macros
#if defined(SPI_SCLK_PA)
    #define PIN_SCLK            SPI_SCLK_PA
    #define SPI_Init_SCLK()     GPIO_PA_Init_Output(PIN_SCLK)
    #define _SPI_SCLK_Px        __pa
    #define _SPI_SCLK_PxC        __pac
#else
    #define PIN_SCLK            SPI_SCLK_PB
    #define SPI_Init_SCLK()     GPIO_PB_Init_Output(PIN_SCLK)
    #define _SPI_SCLK_Px        __pb
    #define _SPI_SCLK_PxC       __pbc
#endif

#if defined(SPI_MOSI_PA)
    #define PIN_MOSI            SPI_MOSI_PA
    #define SPI_Init_MOSI()     GPIO_PA_Init_Output(PIN_MOSI)
    #define _SPI_MOSI_Px        __pa
    #define _SPI_MOSI_PxC       __pac
#else
    #define PIN_MOSI             SPI_MOSI_PB
    #define SPI_Init_MOSI()      GPIO_PB_Init_Output(PIN_MOSI)
    #define _SPI_MOSI_Px        __pb
    #define _SPI_MOSI_PxC       __pbc
#endif


/*
 * ======== GLOBAL VARIABLES ========
 */


/*
 * ======== LOCAL VARIABLES ========
 */


/*
 * ======== LOCAL FUNCTIONS ========
 */


/*
 * ======== FUNCTIONS ========
 */

void SPI_Init(void)
{
    SPI_Init_SCLK();
    SPI_Init_MOSI();
}

void SPI_Write_Byte(uint8_t byte) ASM_CORE
{
    byte;

#define _byte   _SPI_Write_Byte_PARM_1
    __asm
    mov     a, #0x08
    mov     p, a

00001$:
    sl          _byte
    swapc.io    _SPI_MOSI_Px, #PIN_MOSI
    set1.io     _SPI_SCLK_Px, #PIN_SCLK
    set0.io     _SPI_SCLK_Px, #PIN_SCLK

    dzsn    p
    goto    00001$

    ret
__endasm;
#undef _byte
}