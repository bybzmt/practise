#ifndef BASE_H
#define BASE_H

#include "cmsis_os.h"
#include <stdbool.h>
#include "stm32f4xx_hal.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "usbd_audio_if.h"

#include "bsp_audio.h"

extern I2C_HandleTypeDef hi2c1;
extern PCD_HandleTypeDef hpcd;
extern SAI_HandleTypeDef hsai_out;
extern SPDIFRX_HandleTypeDef SpdifrxHandle;

extern bool volume_mute;
extern uint8_t volume;

void bsp_tas6424_init(void);
void bsp_tas6424_deInit(void);
void bsp_tas6424_play(uint32_t AudioFreq);
void bsp_tas6424_mute(bool ok);
void bsp_tas6424_vol(uint8_t volume);

void bsp_ssd1306_init(void);

bool my_spdif_has(void);
void my_spdif_start(void);
void my_spdif_stop(void);

#endif
