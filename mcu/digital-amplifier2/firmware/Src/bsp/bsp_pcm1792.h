#include "base.h"

void bsp_pcm1792_init(void);
void bsp_pcm1792_play(uint32_t AudioFreq, uint8_t bit_depth, volume_t vol);
void bsp_pcm1792_volume(uint8_t vol);
void bsp_pcm1792_stop(void);
void bsp_pcm1792_deInit(void);
