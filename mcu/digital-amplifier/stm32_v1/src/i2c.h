#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>

#include "stm32f1xx_hal.h"
#include "base.h"

void my_i2c_init(void);
bool my_i2c_mem_write(uint16_t addr, uint16_t mbuf, uint16_t mlen, uint8_t *buf, size_t len);
bool my_i2c_mem_read(uint16_t addr, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *buf, size_t len);
bool my_i2c_write(uint16_t addr, uint8_t *buf, size_t len);
bool my_i2c_read(uint16_t addr, uint8_t *buf, size_t len);

#endif
