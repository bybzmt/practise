#ifndef BASE_H
#define BASE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "cmsis_os.h"
#include <stdbool.h>
#include "stm32f4xx_hal.h"

// #include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_audio.h"

#include "audio.h"

#define MODE_IDLE  0
#define MODE_USB   1
#define MODE_SPDIF 2
#define MODE_BT    3


extern SAI_HandleTypeDef hsai;
extern I2S_HandleTypeDef hi2s;

extern uint8_t device_mode;

void device_mode_change(uint8_t mode);

extern I2C_HandleTypeDef hi2c1;

void tas6424_init(void);
void tas6424_en(bool ok);
void tas6424_play(uint32_t AudioFreq, uint8_t bit_depth);
void tas6424_mute(bool ok);
void tas6424_volume(uint8_t volume);
void tas6424_stop(void);
void tas6424_check(void);

void pcm1792_init(void);
void pcm1792_play(uint32_t AudioFreq, uint8_t bit_depth);
void pcm1792_mute(bool ok);
void pcm1792_volume(uint8_t volume);
void pcm1792_stop(void);

void spdif_start(void);
void spdif_stop(void);
void spdif_check(void);

#ifdef __cplusplus
}
#endif

#endif
