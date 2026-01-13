#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__

/***********************************************************
 * INCLUDES
 ***********************************************************/
#include "stdint.h"
#include "stdbool.h"

#include "hal_config.h"

/***********************************************************
 * DEFINES
 ***********************************************************/
#if !defined(I2C_SDA_PA) && !defined(I2C_SDA_PB)
    #define I2C_SDA_PA  0
#endif

#if !defined(I2C_SCL_PA) && !defined(I2C_SCL_PB)
    #define I2C_SCL_PA  4
#endif


/***********************************************************
 * TYPEDEFS
 ***********************************************************/


/***********************************************************
 * EXPORTED VARIABLES
 ***********************************************************/


/***********************************************************
 * MAKROS
 ***********************************************************/


/***********************************************************
 * FUNCTIONS
 ***********************************************************/
void I2C_Init(void);
/* Returns true on success, false on failure */
bool I2C_Write(uint8_t address, const uint8_t *buf, uint8_t len);
/* Returns true on success, false on failure */
bool I2C_Read(uint8_t address, uint8_t *buf, uint8_t len);

#endif /* __HAL_I2C_H__ */