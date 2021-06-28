#include "base.h"
#include <stdint.h>

extern SAI_HandleTypeDef hsai_in;

void msp_sai_in_init(uint32_t AudioFreq, uint8_t bit_depth);

void msp_sai_in_deInit(void);
