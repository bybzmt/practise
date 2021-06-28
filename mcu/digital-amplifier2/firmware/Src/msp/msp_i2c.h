#include <stdint.h>

void msp_i2c_init(void);
void msp_i2c_deInit(void);
bool msp_i2c_read(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len);
bool msp_i2c_write(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len);
uint8_t msp_i2c_read_reg(uint8_t addr, uint8_t reg);
bool msp_i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data);


