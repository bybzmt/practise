

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_AUDIO_H
#define __BSP_AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if defined(BSP_AUDIO_USE_RTOS)
#include "k_mem.h"
#else
#include <stdlib.h>
#endif

#include "stm32f4xx_hal.h"

/** @defgroup BSP_Audio_Out_Option BSP AUDIO OUT Option
  * @{
  */
#define BSP_AUDIO_OUT_CIRCULARMODE      ((uint32_t)0x00000001) /* BUFFER CIRCULAR MODE */
#define BSP_AUDIO_OUT_NORMALMODE        ((uint32_t)0x00000002) /* BUFFER NORMAL MODE   */
#define BSP_AUDIO_OUT_STEREOMODE        ((uint32_t)0x00000004) /* STEREO MODE          */
#define BSP_AUDIO_OUT_MONOMODE          ((uint32_t)0x00000008) /* MONO MODE            */

/**
  * @}
  */

/*------------------------------------------------------------------------------
                          USER SAI defines parameters
 -----------------------------------------------------------------------------*/
/** @defgroup CODEC_AudioFrame_SLOT_TDMMode  STM32469I Discovery Audio Slot TDM mode
  * @brief In W8994 codec the Audio frame contains 4 slots : TDM Mode
  * TDM format :
  * +------------------|------------------|--------------------|-------------------+
  * | CODEC_SLOT0 Left | CODEC_SLOT1 Left | CODEC_SLOT0 Right  | CODEC_SLOT1 Right |
  * +------------------------------------------------------------------------------+
  * @{
  */
/* To have 2 separate audio stream in Both headphone and speaker the 4 slot must be activated */
#define CODEC_AUDIOFRAME_SLOT_0123                   SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1 | SAI_SLOTACTIVE_2 | SAI_SLOTACTIVE_3
/* To have an audio stream in headphone only SAI Slot 0 and Slot 2 must be activated */
#define CODEC_AUDIOFRAME_SLOT_02                     SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_2
/* To have an audio stream in speaker only SAI Slot 1 and Slot 3 must be activated */
#define CODEC_AUDIOFRAME_SLOT_13                     SAI_SLOTACTIVE_1 | SAI_SLOTACTIVE_3
/**
  * @}
  */

/* SAI peripheral configuration defines */
#define AUDIO_SAIx                           SAI1_Block_B

/* SAI DMA Stream definitions */
#define AUDIO_SAIx_DMAx_STREAM               DMA2_Stream5
#define AUDIO_SAIx_DMAx_CHANNEL              DMA_CHANNEL_0
#define AUDIO_SAIx_DMAx_IRQ                  DMA2_Stream5_IRQn
#define AUDIO_SAIx_DMAx_PERIPH_DATA_SIZE     DMA_PDATAALIGN_WORD
#define AUDIO_SAIx_DMAx_MEM_DATA_SIZE        DMA_MDATAALIGN_WORD
#define DMA_MAX_SZE                          0xFFFF

#define AUDIO_SAIx_DMAx_IRQHandler           DMA2_Stream5_IRQHandler

/* Select the interrupt preemption priority for the DMA interrupt */
#define AUDIO_OUT_IRQ_PREPRIO           6   /* Select the preemption priority level(0 is the highest) */



/*------------------------------------------------------------------------------
             CONFIGURATION: Audio Driver Configuration parameters
------------------------------------------------------------------------------*/

#define AUDIODATA_SIZE                      4   /* 16-bit or 24-bit audio sample in 32-bit frame */

/* Audio status definition */
#define AUDIO_OK                            ((uint8_t)0)
#define AUDIO_ERROR                         ((uint8_t)1)
#define AUDIO_TIMEOUT                       ((uint8_t)2)

#define DMA_MAX(x)           (((x) <= DMA_MAX_SZE)? (x):DMA_MAX_SZE)


/** @defgroup STM32469I-Discovery_AUDIO_OUT_Exported_Functions  STM32469I Discovery AUDIO OUT Exported Functions
  * @{
  */
uint8_t BSP_AUDIO_Init(uint8_t mode, uint8_t Volume, uint32_t AudioFreq);
uint8_t BSP_AUDIO_Play(uint8_t* pBuffer, uint32_t Size);
uint8_t BSP_AUDIO_OUT_Pause(void);
uint8_t BSP_AUDIO_OUT_Resume(void);
uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t Volume);
uint8_t BSP_AUDIO_OUT_SetMute(uint32_t Cmd);
void    BSP_AUDIO_DeInit(void);
uint32_t BSP_AUDIO_OUT_GetRemainingDataSize(void);

uint8_t BSP_AUDIO_Record(void);

/* User Callbacks: user has to implement these functions in his code if they are needed. */
/* This function is called when the requested data has been completely transferred.*/
void    BSP_AUDIO_OUT_TransferComplete_CallBack(void);

/* This function is called when half of the requested buffer has been transferred. */
void    BSP_AUDIO_OUT_HalfTransfer_CallBack(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32469I_DISCOVERY_AUDIO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
