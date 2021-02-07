#include "usbd_audio_if.h"
#include "bsp_audio.h"

extern USBD_HandleTypeDef USBD_Device;

USBD_AUDIO_ItfTypeDef USBD_AUDIO_fops = {
    .Init = BSP_AUDIO_Init,
    .DeInit = BSP_AUDIO_DeInit,
    .AllocBuffer = BSP_AUDIO_AllocBuffer,
    .PostBuffer = BSP_AUDIO_PostBuffer,
    .VolumeCtl = BSP_AUDIO_OUT_SetVolume,
    .MuteCtl = BSP_AUDIO_OUT_SetMute,
};
