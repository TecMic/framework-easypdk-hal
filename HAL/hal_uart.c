/*
 * ======== INCLUDES ========
 */
#include "hal_uart.h"
#include "hal_util.h"
#include "hal_timer.h"

/*
 * ======== DEFINES ========
 */
#if UART_BAUD == 115200
    // 115107.91 (-92.09 Hz)
    #define UART_TIM_PRESCALER  TIM2_3_PRESCALE_DIV1
    #define UART_TIM_SCALER     TIM2_3_SCALE_DIV1
    #define UART_TIM_BOUND      138
#elif UART_BAUD == 57600
    // 57553.96 (-46.04 Hz)
    #define UART_TIM_PRESCALER  TIM2_3_PRESCALE_DIV1
    #define UART_TIM_SCALER     TIM2_3_SCALE_DIV2
    #define UART_TIM_BOUND      138
#elif UART_BAUD == 38400
    // 38369.30 (-30.70 Hz)
    #define UART_TIM_PRESCALER  TIM2_3_PRESCALE_DIV1
    #define UART_TIM_SCALER     TIM2_3_SCALE_DIV3
    #define UART_TIM_BOUND      138   
#elif UART_BAUD == 19200
    // 19208.68 (+7.68 Hz)
    // #define UART_TIM_PRESCALER  TIM2_3_PRESCALE_DIV1
    // #define UART_TIM_SCALER     TIM2_3_SCALE_DIV7
    // #define UART_TIM_BOUND      118
    // 19184.65 (-15.35 Hz)
    #define UART_TIM_PRESCALER  TIM2_3_PRESCALE_DIV1
    #define UART_TIM_SCALER     TIM2_3_SCALE_DIV6
    #define UART_TIM_BOUND      138
#elif UART_BAUD == 9600
    // 9603.84 Hz (+3.84)
    // #define UART_TIM_PRESCALER  TIM2_3_PRESCALE_DIV1
    // #define UART_TIM_SCALER     TIM2_3_SCALE_DIV7
    // #define UART_TIM_BOUND      237
    // 9592.33 Hz (-7.67)
    #define UART_TIM_PRESCALER  TIM2_3_PRESCALE_DIV1
    #define UART_TIM_SCALER     TIM2_3_SCALE_DIV12
    #define UART_TIM_BOUND      138
#else
    #error "Unsupported UART baud defined!"
#endif

// -4 ensures higher freq --> detection of start bit in first half of bit time
#define UART_TIM_BOUND_SEARCH_START         ((UART_TIM_BOUND / 2) - 4)


#if UART_BAUD == 115200
    #define UART_TIMER_SCALE_DIV    1
#elif UART_BAUD == 57600
    #define UART_TIMER_SCALE_DIV    2
#elif UART_BAUD == 38400
    #define UART_TIMER_SCALE_DIV    3
#elif UART_BAUD == 19200
    #define UART_TIMER_SCALE_DIV    6
#elif UART_BAUD == 9600
    #define UART_TIMER_SCALE_DIV    12
#endif

#if F_CPU == 16000000
    #define UART_CPU_CLK_DIV    1
#elif F_CPU == 8000000
    #define UART_CPU_CLK_DIV    2
#elif F_CPU == 4000000
    #define UART_CPU_CLK_DIV    4
#elif F_CPU == 2000000
    #define UART_CPU_CLK_DIV    8
#elif F_CPU == 1000000
    #define UART_CPU_CLK_DIV    16
#endif

#define UART_RX_CYCLES_TILL_SAMPLE_POINT    12
#define UART_RX_TIMER_VALUE_AT_SAMPLE_POINT ((UART_RX_CYCLES_TILL_SAMPLE_POINT * UART_CPU_CLK_DIV) / UART_TIMER_SCALE_DIV)
#define UART_RX_SAMPLE_POINT_DELAY          (UART_RX_TIMER_VALUE_AT_SAMPLE_POINT - 1) // smaller timer cnt delays the sample point

#define UART_TX_CYCLES_TILL_SAMPLE_POINT    15
#define UART_TX_TIMER_VALUE_AT_SAMPLE_POINT ((UART_TX_CYCLES_TILL_SAMPLE_POINT * UART_CPU_CLK_DIV) / UART_TIMER_SCALE_DIV)
#define UART_TX_SAMPLE_POINT_DELAY          (UART_TX_TIMER_VALUE_AT_SAMPLE_POINT + (UART_TIM_BOUND / 10)) // move bit transition forward for 1/10 of period


// Ringbuffer mask
#define UART_DATA_BUFFER_MASK   (UART_DATA_BUFFER_SIZE - 1)

// Pin defines
#if (UART_PIN_BIT_RX == 0)
    #define UART_RX_PIN_IT
#endif

#if defined(UART_TX_PA)
    #define _UART_TX_Px     __pa
    #define _UART_TX_PxC    __pac
    #define _UART_TX_PxPH   __paph
    #define _UART_TX_PxDIER __padier
#else
    #define _UART_TX_Px     __pb
    #define _UART_TX_PxC    __pbc
    #define _UART_TX_PxPH   __pbph
    #define _UART_TX_PxDIER __pbdier
#endif

#if defined(UART_RX_PA)
    #define _UART_RX_Px     __pa
    #define _UART_RX_PxC    __pac
    #define _UART_RX_PxPH   __paph
    #define _UART_RX_PxDIER __padier
#else
    #define _UART_RX_Px     __pb
    #define _UART_RX_PxC    __pbc
    #define _UART_RX_PxPH   __pbph
    #define _UART_RX_PxDIER __pbdier
#endif

// RX States
#define UART_STATE_RX_RECEIVING_B           0

#define UART_STATE_RX_SEARCH_START_BIT      0x00
#define UART_STATE_RX_RECEIVING             (1 << UART_STATE_RX_RECEIVING_B)

// TX States
#define UART_STATE_TX_TRANSMITTING_B        0
#define UART_STATE_TX_STOP_BIT_B            1
#define UART_STATE_TX_START_BIT_B           2

#define UART_STATE_TX_IDLE                  0x00
#define UART_STATE_TX_TRANSMITTING          (1 << UART_STATE_TX_TRANSMITTING_B)
#define UART_STATE_TX_STOP_BIT              (1 << UART_STATE_TX_STOP_BIT_B)
#define UART_STATE_TX_START_BIT             (1 << UART_STATE_TX_START_BIT_B)

typedef struct {
    uint8_t write;  // index to write to buffer
    uint8_t read;   // index to write from buffer
    uint8_t data[UART_DATA_BUFFER_SIZE];
} UartBuffer_t;

typedef struct {
    uint8_t state;
    uint8_t bitCnt;
    uint8_t currentByte;
    UartBuffer_t buffer;
} UartModuleHandler_t;

#define _txHandler_state        _txHandler+0
#define _txHandler_bitCnt       _txHandler+1
#define _txHandler_currentByte  _txHandler+2
#define _txHandler_write        _txHandler+3
#define _txHandler_read         _txHandler+4
#define _txHandler_data         _txHandler+5

#define _rxHandler_state        _rxHandler+0
#define _rxHandler_bitCnt       _rxHandler+1
#define _rxHandler_currentByte  _rxHandler+2
#define _rxHandler_write        _rxHandler+3
#define _rxHandler_read         _rxHandler+4
#define _rxHandler_data         _rxHandler+5

/*
 * ======== GLOBAL VARIABLES ========
 */


/*
 * ======== LOCAL VARIABLES ========
 */
#if UART_USE_RX
volatile UartModuleHandler_t rxHandler;
#endif
#if UART_USE_TX
volatile UartModuleHandler_t txHandler;
#endif

/*
 * ======== LOCAL FUNCTIONS ========
 */


/*
 * ======== FUNCTIONS ========
 */


#if UART_USE_TX
void UART_Init_TX(void) ASM_CORE
{
__asm
    clear   _txHandler_state // UART_STATE_TX_IDLE
    mov     a, #0x09
    mov     _txHandler_bitCnt, a

#ifdef UART_TX_TYPE_OD
    set0.io _UART_TX_PxC, #UART_TX_PIN_BIT  // input
    set0.io _UART_TX_Px, #UART_TX_PIN_BIT   // output low
    set1.io _UART_TX_PxPH, #UART_TX_PIN_BIT // pull up
#else
    set1.io _UART_TX_PxC, #UART_TX_PIN_BIT  // output
    set1.io _UART_TX_Px, #UART_TX_PIN_BIT   // output high
#endif

    // init timer 3
    mov     a, #(UART_TIM_PRESCALER | UART_TIM_SCALER)
    mov.io  __tm3s, a
    mov     a, #UART_TIM_BOUND
    mov.io  __tm3b, a
    mov     a, #(TIM2_3_CLK_SRC_DISABLE | TIM2_3_MODE_PERIOD)
    mov.io  __tm3c, a

    set0.io __intrq, #UART_INTERRUPT_BIT_TX
    set1.io __inten, #UART_INTERRUPT_BIT_TX
    ret
__endasm;
}

void UART_Handle_Interrupt_TX_Start_Bit(void) ASM_CORE
{
__asm
    // set next state
    mov     a, #UART_STATE_TX_TRANSMITTING
    mov     _txHandler_state, a

    // read byte from buffer and put it to current byte
    mov     a, #(_txHandler_data)
    add     a, _txHandler_read
    mov     pI, a
    idxm    a, pI
    mov     _txHandler_currentByte, a

    // increase read pointer
    inc     _txHandler_read
    mov     a, #UART_DATA_BUFFER_MASK
    and     _txHandler_read, a

    // reset timer to known position to match start bit to next bit duration
    mov     a, #UART_TX_SAMPLE_POINT_DELAY
    mov.io  __tm3ct, a

#ifdef UART_TX_TYPE_OD
    set1.io _UART_TX_PxC, #UART_TX_PIN_BIT  // pin as output/low
    not     _txHandler_currentByte          // invert byte to correct for low = set1.io __pac
#else
    set0.io _UART_TX_Px, #UART_TX_PIN_BIT   // pin low
#endif
    ret
__endasm;
}

void UART_Handle_Interrupt_TX_Transmitting(void) ASM_CORE
{
__asm
    dzsn    _txHandler_bitCnt
    goto    00002$

    // bitCnt == 0
    mov     a, #0x09
    mov     _txHandler_bitCnt, a

#ifdef UART_TX_TYPE_OD
    set0.io _UART_TX_PxC, #UART_TX_PIN_BIT // input/high
#else
    set1.io _UART_TX_Px, #UART_TX_PIN_BIT  // high
#endif

    mov     a, #0x00
    mov.io  __tm3ct, a

    mov     a, _txHandler_read
    ceqsn   a, _txHandler_write // check read == write
    goto    00001$

    // read == write -> no byte to send
    // set state
    mov     a, #UART_STATE_TX_STOP_BIT
    mov     _txHandler_state, a
    ret

00001$:
    // read != write -> send next byte
    mov     a, #UART_STATE_TX_START_BIT
    mov     _txHandler_state, a
    ret

00002$:
    // bitCnt > 0
    sr      _txHandler_currentByte
#ifdef UART_TX_TYPE_OD
    swapc.io    _UART_TX_PxC, #UART_TX_PIN_BIT // input (0) if bit value is 1 - current byte has been inverted
#else
    swapc.io    _UART_TX_Px, #UART_TX_PIN_BIT  // carry to pin
#endif
    ret
__endasm;
}

void UART_Handle_Interrupt_TX(void) ASM_CORE
{
__asm
    // clear interrupt
    set0.io __intrq, #UART_INTERRUPT_BIT_TX

    // check state UART_STATE_TX_TRANSMITTING
    t0sn    _txHandler_state, #UART_STATE_TX_TRANSMITTING_B
    goto    _UART_Handle_Interrupt_TX_Transmitting

    // check state UART_STATE_TX_START_BIT
    t0sn    _txHandler_state, #UART_STATE_TX_START_BIT_B
    goto    _UART_Handle_Interrupt_TX_Start_Bit

    // state == UART_STATE_TX_STOP_BIT
    set0.io __tm3c, #5          // disable timer3
    clear   _txHandler_state    // set state to UART_STATE_TX_IDLE
    ret
__endasm;
}

// expects byte value in a for asm use (default for sdcc compiled c code)
void UART_Send_Byte(uint8_t byte) ASM_CORE
{
    byte;

    #define _byte   _OVERLAY_AREA // _UART_Send_Byte_PARM_1
__asm
    mov     _byte, a

    // pointer to write
    mov     a, #(_txHandler_data)
    add     a, _txHandler_write
    mov     p, a

    mov     a, _txHandler_write
    add     a, #0x01
    and     a, #UART_DATA_BUFFER_MASK

00001$:
    cneqsn  a, _txHandler_read      // wait while buffer.write + 1 == buffer.read
    goto    00001$

    // write byte to data[write]
    xch     _byte
    idxm    p, a

    // write = write + 1
    mov     a, _byte
    mov     _txHandler_write, a

    // check and update state
    mov     a, #UART_STATE_TX_START_BIT
    disgint
    // if already transmitting -> skip, else state = send start bit
    t1sn    _txHandler_state, #UART_STATE_TX_TRANSMITTING_B
    mov     _txHandler_state, a
    engint

    set1.io __tm3c, #5 // enable timer 3
    ret
__endasm;
    #undef _byte
}

// Send only data from RAM. No const char/string !
void UART_Send_Data(uint8_t* data, uint8_t size) ASM_CORE
{
    data, size;

    #define _data   _OVERLAY_AREA+0 // _UART_Send_Data_PARM_1
    #define _size   _OVERLAY_AREA+2 // _UART_Send_Data_PARM_2
    #define _ptr    _TMP_AREA+0     // aligned. Can be used for idxm
__asm
    // clear data bytes. will be used by _UART_Send_Byte
    mov	    a, _data+0
    mov     _ptr+0, a

00001$:
    idxm    a, _ptr+0
    call    _UART_Send_Byte

    inc     _ptr+0

    dzsn    _size
    goto    00001$

    ret
__endasm;
    #undef _data
    #undef _size
    #undef _ptr
}
#endif

#if UART_USE_RX
void UART_Init_RX(void) ASM_CORE
{
__asm
    mov     a, #UART_STATE_RX_SEARCH_START_BIT
    mov     _rxHandler_state, a

    set0.io _UART_RX_PxC, #UART_PIN_BIT_RX      // input
    set1.io _UART_RX_PxDIER, #UART_PIN_BIT_RX   // enable digital input

    // init timer
#ifdef UART_RX_PIN_IT
    mov     a, #(UART_TIM_PRESCALER | UART_TIM_SCALER)
    mov.io  __tm2s, a
    mov     a, #UART_TIM_BOUND
    mov.io  __tm2b, a
    mov     a, #(TIM2_3_CLK_SRC_DISABLE | TIM2_3_MODE_PERIOD) // config timer but keep it disabled
    mov.io  __tm2c, a

    set0.io __intrq, #UART_INTERRUPT_BIT_PIN // clear pa0 interrupt
    set1.io __inten, #UART_INTERRUPT_BIT_PIN // enable pa0 interrupt
#else
    mov     a, #(UART_TIM_PRESCALER | UART_TIM_SCALER)
    mov.io  __tm2s, a
    mov     a, #UART_TIM_BOUND_SEARCH_START
    mov.io  __tm2b, a
    mov     a, #(TIM2_3_CLK_SRC_IHRC | TIM2_3_MODE_PERIOD)  // enable timer
    mov.io  __tm2c, a
#endif

    set0.io __intrq, #UART_INTERRUPT_BIT_RX // clear timer interrupt
    set1.io __inten, #UART_INTERRUPT_BIT_RX // enable timer interrupt
    ret
__endasm;
}

void UART_Handle_Interrupt_RX_Receiving(void) ASM_CORE
{
__asm
    dzsn    _rxHandler_bitCnt
    goto    00001$

    // bitCnt == 0
#ifdef UART_RX_PIN_IT
    set0.io __tm2c, #5  //disable timer2
    set0.io __intrq, #UART_INTERRUPT_BIT_PIN // clear PA0 interrupt
    set1.io __inten, #UART_INTERRUPT_BIT_PIN // enable interrupt PA0
#else
    // change timer duration to search for new start bit
    mov     a, #UART_TIM_BOUND_SEARCH_START
    mov.io  __tm2b, a
#endif

    // set state UART_STATE_RX_SEARCH_START_BIT
    clear   _rxHandler_state

    // increase write marker
    inc     _rxHandler_write
    mov     a, #UART_DATA_BUFFER_MASK
    and     _rxHandler_write, a
    ret

00001$:
    // bitCnt > 0
    swapc.io    _UART_RX_Px, #UART_PIN_BIT_RX  // read pin
    src         _rxHandler_currentByte

    // check if last byte received
    // done here to reduce isr length for stop bit
    mov     a, #0x01
    ceqsn   a, _rxHandler_bitCnt
    ret

    // write received to buffer 
    mov     a, _rxHandler_write
    add     a, #(_rxHandler_data)
    mov     pI, a
    mov     a, _rxHandler_currentByte
    idxm    pI, a
    ret
__endasm;
}

void UART_Handle_Interrupt_RX(void) ASM_CORE
{
__asm
    // clear interrupt
    set0.io __intrq, #UART_INTERRUPT_BIT_RX

    // check state != UART_STATE_RX_RECEIVING
    t0sn    _rxHandler_state, #UART_STATE_RX_RECEIVING_B
    goto    _UART_Handle_Interrupt_RX_Receiving // state is UART_STATE_RX_RECEIVING

#ifdef UART_RX_PIN_IT
    set0.io __inten, #UART_INTERRUPT_BIT_PIN    // disable GPIO interrupt
    set1.io __tm2c, #5                          // enable timer 2 with IHRC

    mov     a, #0x00
#else
    // check if pin is low (start condition)
    t0sn.io _UART_RX_Px, #UART_PIN_BIT_RX
    ret // return if pin is high

    // start condition -> change timer duration to baud rate
    mov     a, #UART_TIM_BOUND
    mov.io  __tm2b, a

    mov     a, #(UART_RX_SAMPLE_POINT_DELAY)
#endif

    // reset timer to known count
    mov.io  __tm2ct, a
    set0.io __intrq, #UART_INTERRUPT_BIT_RX // ensure interrupt is cleared

    // set state UART_STATE_RX_RECEIVING
    set1    _rxHandler_state, #UART_STATE_RX_RECEIVING_B

    // set bitCnt = 9
    mov     a, #0x09
    mov     _rxHandler_bitCnt, a

    ret
__endasm;
}

#ifdef UART_RX_PIN_IT
void UART_Handle_Interrupt_RX_Pin(void) ASM_CORE
{
__asm
    t1sn.io __intrq, #UART_INTERRUPT_BIT_PIN
    ret

    goto    _UART_Handle_Interrupt_RX
__endasm;
}
#endif

bool UART_Is_RX_Available(void) ASM_CORE
{
__asm
    mov     a, _rxHandler_read
    cneqsn  a, _rxHandler_write
    ret     #0x00
    ret     #0x01
__endasm;
}

uint8_t UART_Get_Byte(void) ASM_CORE
{
__asm
    mov     a, _rxHandler_read
    cneqsn  a, _rxHandler_write // return false if read == write
    ret     #0x00

    add     a, #(_rxHandler_data) // buffer.read + &buffer.data
    mov     p, a

    // increase read index
    inc     _rxHandler_read
    mov     a, #UART_DATA_BUFFER_MASK
    and     _rxHandler_read, a

    idxm    a, p
    ret
__endasm;
}
#endif