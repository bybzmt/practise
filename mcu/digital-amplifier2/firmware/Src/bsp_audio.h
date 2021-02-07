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


uint8_t BSP_AUDIO_Init(uint32_t AudioFreq, uint8_t AudioSize, uint8_t Volume);
uint8_t BSP_AUDIO_Play(uint8_t* pBuffer, uint16_t Size);
uint8_t BSP_AUDIO_OUT_Pause(void);
uint8_t BSP_AUDIO_OUT_Resume(void);
uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t Volume);
uint8_t BSP_AUDIO_OUT_SetMute(bool flag);
void    BSP_AUDIO_DeInit(void);
void    BSP_AUDIO_OUT_TransferComplete_CallBack(void);
void    BSP_AUDIO_OUT_HalfTransfer_CallBack(void);

void BSP_AUDIO_service(void);


#ifdef __cplusplus
}
#endif

#endif
