#include <stdint.h>
#include "base.h"


void bsp_tas5805_init();
void bsp_tas5805_play(uint32_t AudioFreq, uint8_t bit_depth, volume_t vol);
void bsp_tas5805_volume(uint8_t vol);
void bsp_tas5805_deInit(void);

