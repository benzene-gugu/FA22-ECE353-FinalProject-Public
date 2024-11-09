#ifndef I2C_H_
#define I2C_H_

#include "msp.h"
#include <stdint.h>

/**
 * Initialize the I2C Interface that is being used for communication
 */
void i2c_init(void);

/**
 * Reads 16 bits of data from the subordinate device
 */
uint16_t i2c_read_16(uint8_t subordinate_address, uint8_t reg_address);

/**
 * Writes 16 bits of data to the subordinate device
 */
void i2c_write_16(uint8_t subordinate_address, uint8_t reg_address, uint16_t data);



#endif /* I2C_H_ */
