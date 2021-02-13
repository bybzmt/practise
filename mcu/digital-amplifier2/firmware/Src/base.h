#ifndef BASE_H
#define BASE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "cmsis_os.h"
#include <stdbool.h>
#include "stm32f4xx_hal.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "usbd_audio_if.h"

#include "audio.h"

extern I2C_HandleTypeDef hi2c1;
extern PCD_HandleTypeDef hpcd;

extern bool volume_mute;
extern uint8_t volume;

void bsp_tas6424_init(void);
void bsp_tas6424_deInit(void);
void bsp_tas6424_play(uint32_t AudioFreq);
void bsp_tas6424_mute(bool ok);
void bsp_tas6424_vol(uint8_t volume);

void bsp_ssd1306_init(void);

void spdif_init(void);
void spdif_start(void);
void spdif_stop(void);

#ifdef __cplusplus
}
#endif

#endif
