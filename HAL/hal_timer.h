#ifndef __HAL_TIMER_H__
#define __HAL_TIMER_H__

#include <pdk/device.h>

/* Timer 2 and 3 Control Register */
#define TIM2_3_CLK_SRC_DISABLE      TM2C_CLK_DISABLE
#define TIM2_3_CLK_SRC_SYSCLK       TM2C_CLK_SYSCLK
#define TIM2_3_CLK_SRC_IHRC         TM2C_CLK_IHRC
#define TIM2_3_CLK_SRC_EOSC         TM2C_CLK_EOSC
#define TIM2_3_CLK_SRC_ILRC         TM2C_CLK_ILRC
#define TIM2_3_CLK_SRC_COMP         TM2C_CLK_COMPOUT
#define TIM2_3_CLK_SRC_PA0_RISE     TM2C_CLK_PA0_RISE
#define TIM2_3_CLK_SRC_PA0_FALL     TM2C_CLK_PA0_FALL
#define TIM2_3_CLK_SRC_PA4_RISE     TM2C_CLK_PA4_RISE
#define TIM2_3_CLK_SRC_PA4_FALL     TM2C_CLK_PA4_FALL

#define TIM2_3_MODE_PERIOD          TM2C_MODE_PERIOD
#define TIM2_3_MODE_PWM             TM2C_MODE_PWM

#define TIM2_3_OUT_POL_NO_INV       0,
#define TIM2_3_OUT_POL_INV          TM2C_INVERT_OUT

/* Timer 2 Output Pin */
#define TIM2_OUT_PIN_DISABLE        TM2C_OUT_DISABLE
#define TIM2_OUT_PIN_PB2            TM2C_OUT_PB2
#define TIM2_OUT_PIN_PA3            TM2C_OUT_PA3
#define TIM2_OUT_PIN_PB4            TM2C_OUT_PB4

/* Timer 3 Output Pin */
#define TIM3_OUT_PIN_DISABLE        TM3C_OUT_DISABLE
#define TIM3_OUT_PIN_PB5            TM3C_OUT_PB5
#define TIM3_OUT_PIN_PB6            TM3C_OUT_PB6
#define TIM3_OUT_PIN_PB7            TM3C_OUT_PB7

/* Timer 2 and 3 Scale Register */
#define TIM2_3_PWM_RES_8B           TM2S_PWM_RES_8BIT
#define TIM2_3_PWM_RES_6B           TM2S_PWM_RES_6BIT

#define TIM2_3_PRESCALE_DIV1        TM2S_PRESCALE_NONE
#define TIM2_3_PRESCALE_DIV4        TM2S_PRESCALE_DIV4
#define TIM2_3_PRESCALE_DIV16       TM2S_PRESCALE_DIV16
#define TIM2_3_PRESCALE_DIV64       TM2S_PRESCALE_DIV64

#define TIM2_3_SCALE_DIV1           TM2S_SCALE_NONE
#define TIM2_3_SCALE_DIV2           TM2S_SCALE_DIV2
#define TIM2_3_SCALE_DIV3           TM2S_SCALE_DIV3
#define TIM2_3_SCALE_DIV4           TM2S_SCALE_DIV4
#define TIM2_3_SCALE_DIV5           TM2S_SCALE_DIV5
#define TIM2_3_SCALE_DIV6           TM2S_SCALE_DIV6
#define TIM2_3_SCALE_DIV7           TM2S_SCALE_DIV7
#define TIM2_3_SCALE_DIV8           TM2S_SCALE_DIV8
#define TIM2_3_SCALE_DIV9           TM2S_SCALE_DIV9
#define TIM2_3_SCALE_DIV10          TM2S_SCALE_DIV10
#define TIM2_3_SCALE_DIV11          TM2S_SCALE_DIV11
#define TIM2_3_SCALE_DIV12          TM2S_SCALE_DIV12
#define TIM2_3_SCALE_DIV13          TM2S_SCALE_DIV13
#define TIM2_3_SCALE_DIV14          TM2S_SCALE_DIV14
#define TIM2_3_SCALE_DIV15          TM2S_SCALE_DIV15
#define TIM2_3_SCALE_DIV16          TM2S_SCALE_DIV16
#define TIM2_3_SCALE_DIV17          TM2S_SCALE_DIV17
#define TIM2_3_SCALE_DIV18          TM2S_SCALE_DIV18
#define TIM2_3_SCALE_DIV19          TM2S_SCALE_DIV19
#define TIM2_3_SCALE_DIV20          TM2S_SCALE_DIV20
#define TIM2_3_SCALE_DIV21          TM2S_SCALE_DIV21
#define TIM2_3_SCALE_DIV22          TM2S_SCALE_DIV22
#define TIM2_3_SCALE_DIV23          TM2S_SCALE_DIV23
#define TIM2_3_SCALE_DIV24          TM2S_SCALE_DIV24
#define TIM2_3_SCALE_DIV25          TM2S_SCALE_DIV25
#define TIM2_3_SCALE_DIV26          TM2S_SCALE_DIV26
#define TIM2_3_SCALE_DIV27          TM2S_SCALE_DIV27
#define TIM2_3_SCALE_DIV28          TM2S_SCALE_DIV28
#define TIM2_3_SCALE_DIV29          TM2S_SCALE_DIV29
#define TIM2_3_SCALE_DIV30          TM2S_SCALE_DIV30
#define TIM2_3_SCALE_DIV31          TM2S_SCALE_DIV31
#define TIM2_3_SCALE_DIV32          TM2S_SCALE_DIV32

/* Timer 2 Macros */
#define TIM2_Set_Conrol(control)    TM2C = (uint8_t)control // src | out | mode | pol
#define TIM2_Set_Scale(scale)       TM2S = (uint8_t)scale   // pwmRes | preScale | scale
#define TIM2_Set_Period(period)     TM2B = (uint8_t)period

#define TIM2_Enable(clk_src)        TM2C |= (uint8_t)clk_src
#define TIM2_Disable()              TM2C &= ~0xF0
#define TIM2_Get_Count()            TM2CT
#define TIM2_Set_Count(cnt)         TM2CT = (uint8_t)cnt

/* Timer 3 Macros */
#define TIM3_Set_Conrol(control)    TM3C = (uint8_t)control // src | out | mode | pol
#define TIM3_Set_Scale(scale)       TM3S = (uint8_t)scale   // pwmRes | preScale | scale
#define TIM3_Set_Period(period)     TM3B = (uint8_t)period

#define TIM3_Enable(clk_src)        TM3C |= (uint8_t)clk_src
#define TIM3_Disable()              TM3C &= ~0xF0
#define TIM3_Get_Count()            TM3CT
#define TIM3_Set_Count(cnt)         TM3CT = (uint8_t)cnt


/* Timer 16 Mode Register */
#define TIM16_CLK_SRC_DISABLE       T16M_CLK_DISABLE
#define TIM16_CLK_SRC_SYSCLK        T16M_CLK_SYSCLK
#define TIM16_CLK_SRC_PA4_FALL      T16M_CLK_PA4_FALL
#define TIM16_CLK_SRC_IHRC          T16M_CLK_IHRC
#define TIM16_CLK_SRC_EOSC          T16M_CLK_EOSC
#define TIM16_CLK_SRC_ILRC          T16M_CLK_ILRC
#define TIM16_CLK_SRC_PA0_FALL      T16M_CLK_PA0_FALL

#define TIM16_PRESCALE_DIV1         T16M_CLK_DIV1
#define TIM16_PRESCALE_DIV4         T16M_CLK_DIV4
#define TIM16_PRESCALE_DIV16        T16M_CLK_DIV16
#define TIM16_PRESCALE_DIV64        T16M_CLK_DIV64

#define TIM16_INT_ON_CHANGE_BIT8    T16M_INTSRC_8BIT
#define TIM16_INT_ON_CHANGE_BIT9    T16M_INTSRC_9BIT
#define TIM16_INT_ON_CHANGE_BIT10   T16M_INTSRC_10BIT
#define TIM16_INT_ON_CHANGE_BIT11   T16M_INTSRC_11BIT
#define TIM16_INT_ON_CHANGE_BIT12   T16M_INTSRC_12BIT
#define TIM16_INT_ON_CHANGE_BIT13   T16M_INTSRC_13BIT
#define TIM16_INT_ON_CHANGE_BIT14   T16M_INTSRC_14BIT
#define TIM16_INT_ON_CHANGE_BIT15   T16M_INTSRC_15BIT

/* Timer 16 Macros */
#define TIM16_Set_Control(control)  T16M = (uint8_t)control // preScale | intOnBit | clkSrc

#endif //__HAL_TIMER_H__