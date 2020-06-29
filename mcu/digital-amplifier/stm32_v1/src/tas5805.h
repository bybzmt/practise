#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "base.h"
#include "i2c.h"

#define I2C_ADDR_5805_1 0x58
#define I2C_ADDR_5805_2 0x5a


void tas5805_init();
void tas5805_volume();
void tas5805_check();
void tas5805_show_sampling_rate();


