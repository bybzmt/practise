#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>

#include "stm32f1xx_hal.h"
#include "base.h"

void my_i2c_init(I2C_HandleTypeDef *handle);
bool my_i2c_mem_write(I2C_HandleTypeDef *, uint16_t addr, uint16_t mbuf, uint16_t mlen, uint8_t *buf, size_t len);
bool my_i2c_mem_read(I2C_HandleTypeDef *, uint16_t addr, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *buf, size_t len);
bool my_i2c_write(I2C_HandleTypeDef *, uint16_t addr, uint8_t *buf, size_t len);
bool my_i2c_read(I2C_HandleTypeDef *, uint16_t addr, uint8_t *buf, size_t len);

#endif
