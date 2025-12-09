#ifndef __HAL_STARTUP_H__
#define __HAL_STARTUP_H__

#include <pdk/device.h>

#define DEFAULT_FLAG                // (0x00)
#define DEFAULT_REG_01              // (0x01)
#define DEFAULT_SP                  // (0x02)
#define DEFAULT_CLKMD       0x6f    // (0x03) PA5 IO, WD enable, ILRC enable, CLK Type 0, IHRC enable, ILRC
#define DEFAULT_INTEN       0x00    // (0x04) No Interrupts enabled
#define DEFAULT_INTRQ       0x00    // (0x05) No Interrupt requests
#define DEFAULT_T16M        0x00    // (0x06) Timer 16 disable
#define DEFAULT_REG_07              // (0x07)
#define DEFAULT_MISC        0x00    // (0x08) Normal WU, VDD/2 disable, LVR enable, WD 8192 ILRC clock period
#define DEFAULT_TM2B        0x00    // (0x09) Bound 0
#define DEFAULT_EOSCR       0x00    // (0x0a) Disable extern osc, power down LVR and Band-Gap
#define DEFAULT_IHRCR               // (0x0b)
#define DEFAULT_INTEGS      0x00    // (0x0c) Tim16 rising edge, PB0 PA0 Both
#define DEFAULT_PADIER      0x00    // (0x0d) Disable interrupt and wakeup event
#define DEFAULT_PBDIER      0x00    // (0x0e) Disable interrupt and wakeup event
#define DEFAULT_MISC2               // (0x0f)
#define DEFAULT_PA          0x00    // (0x10) All pins low
#define DEFAULT_PAC         0x00    // (0x11) All pins input
#define DEFAULT_PAPH        0x00    // (0x12) Disable pull up
#define DEFAULT_REG_13              // (0x13)
#define DEFAULT_PB          0x00    // (0x14) All pins low
#define DEFAULT_PBC         0x00    // (0x15) All pins input
#define DEFAULT_PBPH        0x00    // (0x16) Disable pull up
#define DEFAULT_TM2S        0x00    // (0x17) 8-bit pwm, no scaler / prescaler
#define DEFAULT_GPCC        0x00    // (0x18) Comparator disabled
#define DEFAULT_GPCS        0x00    // (0x19) Comparator selection default
#define DEFAULT_BGTR                // (0x1a)
#define DEFAULT_MISCLVR             // (0x1b)
#define DEFAULT_TM2C        0x00    // (0x1c) Disable, output disable, period mode, not inverse
#define DEFAULT_TM2CT       0x00    // (0x1d) Cnt 0
#define DEFAULT_REG_1E              // (0x1e)
#define DEFAULT_REG_1F              // (0x1f)
#define DEFAULT_PWMG0C      0x00    // (0x20) Disable, not inverse, not reset, no output, sysclk
#define DEFAULT_PWMG0S      0x00    // (0x21) ITR on duty, no scaler / prescaler
#define DEFAULT_PWMG0DTH    0x00    // (0x22) Upper duty 0
#define DEFAULT_PWMG0DTL    0x00    // (0x23) Lower duty 0
#define DEFAULT_PWMG0CUBH   0x00    // (0x24) Upper bound 0
#define DEFAULT_PWMG0CUBL   0x00    // (0x25) Lower bound 0
#define DEFAULT_PWMG1C      0x00    // (0x26) Disable, not inverse, not reset, no output, sysclk
#define DEFAULT_PWMG1S      0x00    // (0x27) ITR on duty, no scaler / prescaler
#define DEFAULT_PWMG1DTH    0x00    // (0x28) Upper duty 0
#define DEFAULT_PWMG1DTL    0x00    // (0x29) Lower duty 0
#define DEFAULT_PWMG1CUBH   0x00    // (0x2a) Upper bound 0
#define DEFAULT_PWMG1CUBL   0x00    // (0x2b) Lower bound 0
#define DEFAULT_PWMG2C      0x00    // (0x2c) Disable, not inverse, not reset, no output, sysclk
#define DEFAULT_PWMG2S      0x00    // (0x2d) ITR on duty, no scaler / prescaler
#define DEFAULT_PWMG2DTH    0x00    // (0x2e) Upper duty 0
#define DEFAULT_PWMG2DTL    0x00    // (0x2f) Lower duty 0
#define DEFAULT_PWMG2CUBH   0x00    // (0x30) Upper bound 0
#define DEFAULT_PWMG2CUBL   0x00    // (0x31) Lower bound 0
#define DEFAULT_TM3C        0x00    // (0x32) Disable, output disable, period mode, not inverse
#define DEFAULT_TM3CT       0x00    // (0x33) Cnt 0
#define DEFAULT_TM3S        0x00    // (0x34) 8-bit pwm, no scaler / prescaler
#define DEFAULT_TM3B        0x00    // (0x35) Bound 0
#define DEFAULT_RFCC                // (0x36)
#define DEFAULT_RFCCRH              // (0x37)
#define DEFAULT_RFCCRL              // (0x38)
#define DEFAULT_ILRCR               // (0x39)


#define STARTUP_Set_Registers_Default() \
    __asm__( \
        "mov    a, #0x00        \n" \
        "mov.io __inten, a      \n" \
        "mov.io __misc, a       \n" \
        "mov.io __pa, a         \n" \
        "mov.io __pb, a         \n" \
        "mov.io __eoscr, a      \n" \
                                    \
        "mov.io __tm2c, a       \n" \
        "mov.io __tm3c, a       \n" \
        "mov.io __t16m, a       \n" \
        "mov.io __pwmg0c, a     \n" \
        "mov.io __pwmg1c, a     \n" \
        "mov.io __pwmg2c, a     \n" \
        "mov.io __gpcc, a       \n" \
                                    \
        "mov.io __intrq, a      \n" \
        "mov.io __integs, a     \n" \
        "mov.io __padier, a     \n" \
        "mov.io __pbdier, a     \n" \
        "mov.io __pac, a        \n" \
        "mov.io __paph, a       \n" \
        "mov.io __pbc, a        \n" \
        "mov.io __pbph, a       \n" \
        "mov.io __gpcs, a       \n" \
    )

#define STARTUP_Set_Registers_Default_Optional() \
    __asm__( \
        "mov    a, #0x00        \n" \
        "mov.io  __tm2ct, a     \n" \
        "mov.io  __tm2s, a      \n" \
        "mov.io  __tm2b, a      \n" \
        "mov.io  __tm3ct, a     \n" \
        "mov.io  __tm3s, a      \n" \
        "mov.io  __tm3b, a      \n" \
                                    \
        "mov.io  __pwmg0s, a    \n" \
        "mov.io  __pwmg0cubh, a \n" \
        "mov.io  __pwmg0cubl, a \n" \
        "mov.io  __pwmg0dtl, a  \n" \
        "mov.io  __pwmg0dth, a  \n" \
        "mov.io  __pwmg1s, a    \n" \
        "mov.io  __pwmg1cubh, a \n" \
        "mov.io  __pwmg1cubl, a \n" \
        "mov.io  __pwmg1dtl, a  \n" \
        "mov.io  __pwmg1dth, a  \n" \
        "mov.io  __pwmg2s, a    \n" \
        "mov.io  __pwmg2cubh, a \n" \
        "mov.io  __pwmg2cubl, a \n" \
        "mov.io  __pwmg2dtl, a  \n" \
        "mov.io  __pwmg2dth, a  \n" \
    )

#endif //__HAL_STARTUP_H__