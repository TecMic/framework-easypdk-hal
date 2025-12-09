/*
 * ======== INCLUDES ========
 */
#include "hal_util.h"

/*
 * ======== DEFINES ========
 */


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

// create assembler regions for HAL lib. Will always be compiled
static void UTIL_Init_ASM(void) ASM_CORE
{
__asm
    // dummy IVECT region
    .area IVECT

    // pseudo register for assembler interrupts
    .area PIREG
pI::
    .ds 2
    .globl  pI

    // reserve 12 overlayable bytes in RAM
    .area   OSEG (OVR,DATA)
_OVERLAY_AREA:
    .ds 12
    .globl  _OVERLAY_AREA

    // reserve 4 bytes at known and aligned position
    .area TMPSEG (ABS,OVR)
_TMP_AREA:
    .ds 4
    .globl _TMP_AREA

    .area CODE
__endasm;
}