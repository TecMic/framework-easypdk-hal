/*
 * RAM Overlaying in asm
 * 
 * OVR Byte:        |   0   |   1   |   2   |   3   |   4   |
 * ----------------------------------------------------------
 * I2C_Write_Bit    |       |       |       |       |       | - Bit parameter in a
 * I2C_Read_Bit     |       |       |       |       |       | - keep value in p while delay call (uses only a)
 * I2C_Write_Byte   | data  |       |   i   |       |       |
 * I2C_Read_Byte    | ack   |       |   i   |       | data  |
 * I2C_Read         | addr  | ptr   |       | len   |       |- ptr is 2 byte, but only lsb is used
 * I2C_Write        | addr  | ptr   |       | len   |       |- ptr is 2 byte, but only lsb is used
 * ----------------------------------------------------------
 */

/*
 * ======== INCLUDES ========
 */
#include "hal_i2c.h"

#include "hal_gpio.h"

/*
 * ======== DEFINES ========
 */
#define I2C_DELAY_US        5u  // 100 kHz
#define I2C_DELAY_TICKS     ((I2C_DELAY_US * F_CPU + 999999UL) / 1000000UL)
#define I2C_DELAY_CYCLES    ((I2C_DELAY_TICKS - 2u) / 3u) 

// Pin macros
#if defined(I2C_SDA_PA)
    #define PIN_SDA             I2C_SDA_PA

    #define I2C_Init_SDA()      do {                                        \
                                    GPIO_PA_Init_OD(PIN_SDA);               \
                                    GPIO_PA_Enable_Digital_Input(PIN_SDA);  \
                                } while (0)

    #define I2C_Release_SDA()   GPIO_PA_OD_HIGH(PIN_SDA)
    #define I2C_Drive_Low_SDA() GPIO_PA_OD_LOW(PIN_SDA)
    #define I2C_Read_SDA()      GPIO_PA_Read_Pin(PIN_SDA)

    #define _I2C_SDA_Px         __pa
    #define _I2C_SDA_PxC        __pac
#else
    #define PIN_SDA             I2C_SDA_PB

    #define I2C_Init_SDA()      do {                                        \
                                    GPIO_PB_Init_OD(PIN_SDA);               \
                                    GPIO_PB_Enable_Digital_Input(PIN_SDA);  \
                                } while (0)

    #define I2C_Release_SDA()   GPIO_PB_OD_HIGH(PIN_SDA)
    #define I2C_Drive_Low_SDA() GPIO_PB_OD_LOW(PIN_SDA)
    #define I2C_Read_SDA()      GPIO_PB_Read_Pin(PIN_SDA)

    #define _I2C_SDA_Px         __pb
    #define _I2C_SDA_PxC        __pbc
#endif

#if defined(I2C_SCL_PA)
    #define PIN_SCL             I2C_SCL_PA

    #define I2C_Init_SCL()      GPIO_PA_Init_OD(PIN_SCL)

    #define I2C_Release_SCL()   GPIO_PA_OD_HIGH(PIN_SCL)
    #define I2C_Drive_Low_SCL() GPIO_PA_OD_LOW(PIN_SCL)

    #define _I2C_SCL_Px         __pa
    #define _I2C_SCL_PxC        __pac
#else
    #define PIN_SCL             I2C_SCL_PB

    #define I2C_Init_SCL()      GPIO_PB_Init_OD(PIN_SCL)

    #define I2C_Release_SCL()   GPIO_PB_OD_HIGH(PIN_SCL)
    #define I2C_Drive_Low_SCL() GPIO_PB_OD_LOW(PIN_SCL)

    #define _I2C_SCL_Px         __pb
    #define _I2C_SCL_PxC        __pbc
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
void I2C_Delay(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write_Bit(uint8_t bitval);
uint8_t I2C_Read_Bit(void);
uint8_t I2C_Write_Byte(uint8_t data);
uint8_t I2C_Read_Byte(uint8_t ack);

/*
 * ======== FUNCTIONS ========
 */

void I2C_Init(void)
{
    I2C_Init_SDA();
    I2C_Init_SCL();
}

// 2 (call) + 1 (mov) + 2 (ret) + 2 (n-th loop) + 3*(n-1) = 7 + 3 * (n-1)
void I2C_Delay(void)
{
#if (I2C_DELAY_TICKS > 5)
    uint8_t i;
    for (i = 0; i < I2C_DELAY_CYCLES; ++i) {}
#endif
}

/* Generate START: SDA goes high->low while SCL is high */
void I2C_Start(void)
{
    /* ensure released */
    I2C_Release_SDA();
    I2C_Release_SCL();
    I2C_Delay();

    /* start: pull SDA low while SCL high */
    I2C_Drive_Low_SDA();
    I2C_Delay();
    /* then pull SCL low to begin bit transfer */
    I2C_Drive_Low_SCL();
}

/* Generate STOP: SDA goes low->high while SCL is high */
void I2C_Stop(void)
{
    /* Ensure SDA low */
    I2C_Drive_Low_SDA();
    I2C_Delay();
    /* Release SCL (let it go high) */
    I2C_Release_SCL();
    I2C_Delay();
    /* Release SDA (stop condition) */
    I2C_Release_SDA();
}

/* Write one bit (msb-first). For '1' we release SDA; for '0' drive low.
   Clock: raise SCL, wait, lower SCL. */
#ifndef HAL_C_FUNCTIONS
void I2C_Write_Bit(uint8_t bitval) ASM_CORE
{
    bitval;

__asm
    cneqsn  a, #0x00
    goto    00001$

    // release SDA
    set0.io _I2C_SDA_PxC, #PIN_SDA
    goto    00002$
00001$:
    // drive low SDA
    set1.io _I2C_SDA_PxC, #PIN_SDA
    set0.io _I2C_SDA_Px, #PIN_SDA
00002$:
    call    _I2C_Delay
    // release SCL
    set0.io _I2C_SCL_PxC, #PIN_SCL
    call    _I2C_Delay
    // drive low SCL
    set1.io _I2C_SCL_PxC, #PIN_SCL
    set0.io _I2C_SCL_Px, #PIN_SCL

    ret
__endasm;
}
#else
void I2C_Write_Bit(uint8_t bitval)
{
    if (bitval) {
        I2C_Release_SDA();
    } else {
        I2C_Drive_Low_SDA();
    }

    I2C_Delay();
    /* clock high */
    I2C_Release_SCL();
    I2C_Delay();
    /* clock low */
    I2C_Drive_Low_SCL();
}
#endif

/* Read one bit: release SDA, raise SCL, sample SDA, lower SCL */
#ifndef HAL_C_FUNCTIONS
uint8_t I2C_Read_Bit(void) ASM_CORE
{
__asm
    // release SDA
    set0.io _I2C_SDA_PxC, #PIN_SDA
    call    _I2C_Delay
    // release SCL
    set0.io _I2C_SCL_PxC, #PIN_SCL

    clear   p
    t0sn.io _I2C_SDA_Px, #PIN_SDA
    set1    p, #0
    call    _I2C_Delay
    mov     a, p

    // drive low SCL
    set1.io _I2C_SCL_PxC, #PIN_SCL
    set0.io _I2C_SCL_Px, #PIN_SCL

    ret
__endasm;
}
#else
uint8_t I2C_Read_Bit(void)
{
    uint8_t b;

    I2C_Release_SDA(); /* release SDA */
    I2C_Delay();
    I2C_Release_SCL();
    b = I2C_Read_SDA() ? 1 : 0;
    I2C_Delay();
    /* bring SCL low */
    I2C_Drive_Low_SCL();
    return b;
}
#endif

/* Write byte, return 0 on ACK received, 1 on NACK or error */
#ifndef HAL_C_FUNCTIONS
uint8_t I2C_Write_Byte(uint8_t data) ASM_CORE
{
    data;

#define _data   _I2C_Write_Byte_OVR_PARM_1
#define _i      _I2C_Write_Byte_OVR_PARM_2

__asm
    .area   OSEG (OVR,DATA)

_data:      // first byte overlays with _I2C_Write_Byte_PARM_1
    .ds 2
    .globl  _data
_i:
    .ds 1
    .globl  _i
    
    .area CODE
    mov     a, #0x08
    mov     _i, a

000001$:
    mov     a, #0x00
    sl      _data   // msb to carry
    slc     a       // carry to bit 0 of a

    call    _I2C_Write_Bit

    dzsn    _i
    goto    000001$

    goto    _I2C_Read_Bit   // will return ack
__endasm;

#undef _data
#undef _i
}
#else
uint8_t I2C_Write_Byte(uint8_t data)
{
    int i;
    for (i = 7; i >= 0; --i)
    {
        I2C_Write_Bit( (data >> i) & 1 );
    }

    /* Read ACK bit (master releases SDA, then reads while SCL high) */
    uint8_t ack = I2C_Read_Bit(); /* ack==0 means slave pulled low */
    return (ack ? 1 : 0);
}
#endif

/* Read byte, ack=1 to send ACK, ack=0 to send NACK after reading */
#ifndef HAL_C_FUNCTIONS
uint8_t I2C_Read_Byte(uint8_t ack) ASM_CORE
{
    ack;

#define _ack    _I2C_Read_Byte_OVR_PARM_1
#define _i      _I2C_Read_Byte_OVR_PARM_2
#define _data   _I2C_Read_Byte_OVR_PARM_3

__asm
    .area   OSEG (OVR,DATA)
_ack:       // first byte overlays with _I2C_Read_Byte_PARM_1
    .ds 2   // second byte is for spacing
    .globl  _ack
_i:
    .ds 2   // second byte is for spacing
    .globl  _i
_data:
    .ds 1
    .globl  _data
    
    .area CODE
    mov     a, #0x08
    mov     _i, a

    clear   _data

00001$:
    call    _I2C_Read_Bit
    sr      a           // pin value to carry
    slc     _data       // pin value to lsb of ret value

    dzsn    _i
    goto    000001$

    // a is zero at this point
    cneqsn  a, _ack     // if ack == 1 send 0
    mov     a, #0x01

    call    _I2C_Write_Bit

    mov     a, _data
    ret
__endasm;

#undef _ack
#undef _i
#undef _data
}
#else
uint8_t I2C_Read_Byte(uint8_t ack)
{
    uint8_t val = 0;
    for (int i = 7; i >= 0; --i)
    {
        uint8_t bit = I2C_Read_Bit();
        val |= (bit << i);
    }
    /* send ACK/NACK: ack==1 -> drive low (ACK), ack==0 -> release (NACK) */
    I2C_Write_Bit(!ack);

    return val;
}
#endif

/* High-level convenience: writes 'len' bytes to device (7-bit address).
   device_addr: 7-bit address (0..127). Returns 0 on success, non-zero on failure. */
#ifndef HAL_C_FUNCTIONS
bool I2C_Write(uint8_t address, const uint8_t *buf, uint8_t len) ASM_CORE
{
    address, buf, len;

#define _address    _I2C_Write_PARM_1
#define _buf        _I2C_Write_PARM_2
#define _len        _I2C_Write_PARM_3

__asm
    sl      _address        // (address << 1) | 0 => write
    call    _I2C_Start

    call    _I2C_Write_Byte // write parameter is overlayed with address

    cneqsn  a, #0x00        // return -1 if a == 1 (nack)
    goto    00002$

00001$:
    call    _I2C_Stop
    ret     #0x00

00002$:
    mov     a, _buf
    mov     p, a
    idxm    a, p

    mov     _I2C_Write_Byte_PARM_1, a
    call    _I2C_Write_Byte

    ceqsn   a, #0x00    // return -1 if a == 1 (nack)
    goto    00001$

    inc     _buf

    dzsn    _len        // break loop if --len == 0
    goto    00002$

    call    _I2C_Stop
    ret     #0x01
__endasm;

#undef _address
#undef _buf
#undef _len
}
#else
bool I2C_Write(uint8_t address, const uint8_t *buf, uint8_t len)
{
    I2C_Start();
    /* address + write(0) */
    if (I2C_Write_Byte( (address << 1) | 0 ) )
    {
        I2C_Stop();
        return false;
    }
    for (uint8_t i = 0; i < len; ++i)
    {
        if (I2C_Write_Byte(buf[i]))
        {
            I2C_Stop();
            return false;
        }
    }
    I2C_Stop();
    return true;
}
#endif


/* High-level convenience: read 'len' bytes from device into buf (7-bit address).
   Returns 0 on success, non-zero on failure. */
#ifndef HAL_C_FUNCTIONS
bool I2C_Read(uint8_t address, uint8_t *buf, uint8_t len) ASM_CORE
{
    address, buf, len;

#define _address    _I2C_Read_PARM_1
#define _buf        _I2C_Read_PARM_2
#define _len        _I2C_Read_PARM_3

__asm
    set1.io f, c
    slc     _address        // (address << 1) | 1 => read
    call    _I2C_Start

    call    _I2C_Write_Byte // write parameter is overlayed with address

    cneqsn  a, #0x00        // return -1 if a == 1 (nack)
    goto    00002$

00001$:
    call    _I2C_Stop
    ret     #0x00

00002$:
    mov     a, #0x01
    cneqsn  a, _I2C_Read_PARM_3+0   // if len != 1 ? 1 = ack : 0 = nack
    mov     a, #0x00

    mov     _I2C_Read_Byte_PARM_1, a
    call    _I2C_Read_Byte
    xch     p

    mov     a, _buf
    xch     p
    idxm    p, a

    inc     _buf

    dzsn    _len // break loop if --len == 0
    goto    00002$

    call    _I2C_Stop
    ret     #0x01
__endasm;

#undef _address
#undef _buf
#undef _len
}
#else
bool I2C_Read(uint8_t address, uint8_t *buf, uint8_t len)
{
    I2C_Start();
    /* address + read(1) */
    if (I2C_Write_Byte( (address << 1) | 1 ))
    {
        I2C_Stop();
        return false;
    }
    for (uint8_t i = 0; i < len; ++i)
    {
        /* ACK every byte except last (NACK) */
        uint8_t ack = (i < (len - 1)) ? 1 : 0;
        buf[i] = I2C_Read_Byte(ack);
    }
    I2C_Stop();
    return true;
}
#endif