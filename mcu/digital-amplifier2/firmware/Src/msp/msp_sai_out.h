#include "base.h"
#include <string.h>
#include <stdint.h>

extern SAI_HandleTypeDef hsai_out;

void msp_sai_out_init(uint32_t AudioFreq, uint8_t bit_depth);
void msp_sai_out_deInit(void);
uint16_t msp_sai_out_dma_remaining(void);
