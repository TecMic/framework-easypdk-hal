#include "hal_util.h"
#include "hal_gpio.h"
#include "hal_sys_timer.h"
#include "hal_interrupt.h"
#include "hal_startup.h"
#include "hal_uart.h"

#include "hal_uart_print.h"

#include "startup.h"
#include "auto_sysclock.h"


// LED is placed on the PA4 pin (Port A, Bit 4) with a current sink configuration
#define LED_PIN         PA4


// SDCC safe interrupt function
// void INTERRUPT_FUNCTION(void)
// {
//     UART_Interrupt_RX_Handler();    // Handle receive of a byte via UART
//     UART_Interrupt_TX_Handler();    // Handle sending a byte via UART
//     SYS_TIME_Tick_Handler();        // Clear interrupt and increase millis
// }

/*
 * This interrupt handler is a hacked one. 
 * !!! Only use it if you understand the following !!!
 * Else, use the INTERRUPT_FUNCTION above
 *
 * - It is directly placed at the interrupt vector location 0x0020.
 * - Therefore it is faster. It removes the jump to the ISR function (2 cycles)
 * - It also removes the push / pop of the p pseudo register (4 cycles)
 *   -> Don't use any function in this handler! Special HAL functions avoid using p
 *
 * - In this example it can only be 64 byte (32 instructions) long. l_IVECT gives the current length
 *   -> For larger ISR push the code further back (board_build.code_loc = 0x0060 in platformio.ini)
 */
void INTERRUPT_FUNCTION_ASM(void)
{
    INTERRUPT_FUNCTION_ASM_START();

    UART_Interrupt_RX_Handler();    // Handle receive of a byte via UART
    UART_Interrupt_TX_Handler();    // Handle sending a byte via UART
    SYS_TIME_Tick_Handler();        // Clear interrupt and increase millis

    INTERRUPT_FUNCTION_ASM_END();
}


// Main program
void main(void)
{
    // Initialize hardware
    GPIO_PA_Init_Output(LED_PIN);   // Set LED as output (all pins are input by default)

    SYS_TIME_Init(); // Initialize timer for interrupt every ~ 1 ms
    UART_Init_TX();
    UART_Init_RX();

    IT_Enable_Interrupts();

    println("Hello World!");

    uint16_t byteCnt = 0;

    // Main processing loop
    while (1)
    {
        if (UART_Is_RX_Available())
        {
            ++byteCnt;
            uint8_t rxByte = UART_Get_Byte();
            UART_Send_Byte(rxByte);
            GPIO_PA_Write_Reverse(LED_PIN);

            if (rxByte == '\n')
            {
                print_u16(byteCnt);
                println(" Bytes in previous line.");
                byteCnt = 0;
            }
        }
    }
}

// Startup code - Setup/calibrate system clock
unsigned char STARTUP_FUNCTION(void)
{
    // Initialize the system clock (CLKMD register) with the IHRC, ILRC, or EOSC clock source and correct divider.
    // The AUTO_INIT_SYSCLOCK() macro uses F_CPU (defined in the Makefile) to choose the IHRC or ILRC clock source and divider.
    // Alternatively, replace this with the more specific PDK_SET_SYSCLOCK(...) macro from pdk/sysclock.h
    AUTO_INIT_SYSCLOCK();

    // All registers are in a undefined state at startup, so we clear the important ones
    STARTUP_Set_Registers_Default();
    // STARTUP_Set_Registers_Default_Optional();

    // Insert placeholder code to tell EasyPdkProg to calibrate the IHRC or ILRC internal oscillator.
    // The AUTO_CALIBRATE_SYSCLOCK(...) macro uses F_CPU (defined in the Makefile) to choose the IHRC or ILRC oscillator.
    // Alternatively, replace this with the more specific EASY_PDK_CALIBRATE_IHRC(...) or EASY_PDK_CALIBRATE_ILRC(...) macro from easy-pdk/calibrate.h
    AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);

    return 0; // Return 0 to inform SDCC to continue with normal initialization.
}
