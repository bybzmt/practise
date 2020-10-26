/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_AUDIO_IF_H
#define __USBD_AUDIO_IF_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_audio.h"

/* Exported types ------------------------------------------------------------*/
typedef struct {
  uint32_t playing;
  uint32_t frequency;
} AUDIO_STATUS_TypeDef;

/* Exported constants --------------------------------------------------------*/
extern USBD_AUDIO_ItfTypeDef USBD_AUDIO_fops;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __USBD_AUDIO_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
