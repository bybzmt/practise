#ifndef __BSP_AUDIO_H
#define __BSP_AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define AUDIO_OK                            ((uint8_t)0)
#define AUDIO_ERROR                         ((uint8_t)1)
#define AUDIO_TIMEOUT                       ((uint8_t)2)


void BSP_AUDIO_Init(uint32_t audioFreq, uint8_t audioSize, uint16_t bufferSize, uint8_t volume);
uint8_t *BSP_AUDIO_AllocBuffer(void);
void BSP_AUDIO_PostBuffer(uint16_t Size);
void BSP_AUDIO_OUT_SetVolume(uint8_t Volume);
void BSP_AUDIO_OUT_SetMute(bool flag);
void BSP_AUDIO_DeInit(void);


#ifdef __cplusplus
}
#endif

#endif
