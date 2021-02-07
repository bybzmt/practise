#include "usbd_audio_if.h"
#include "bsp_audio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int8_t Audio_Init(uint32_t  AudioFreq, uint8_t AudioSize, uint8_t Volume);
static int8_t Audio_DeInit(void);
static int8_t Audio_PlaybackCmd(uint8_t* pbuf, uint16_t size);
static int8_t Audio_VolumeCtl(uint8_t vol);
static int8_t Audio_MuteCtl(uint8_t cmd);

/* Private variables ---------------------------------------------------------*/
extern USBD_HandleTypeDef USBD_Device;

USBD_AUDIO_ItfTypeDef USBD_AUDIO_fops = {
    .Init = Audio_Init,
    .DeInit = Audio_DeInit,
    .AudioPlay = Audio_PlaybackCmd,
    .VolumeCtl = Audio_VolumeCtl,
    .MuteCtl = Audio_MuteCtl,
};

/* Private functions ---------------------------------------------------------*/

static int8_t Audio_Init(uint32_t  AudioFreq, uint8_t AudioSize, uint8_t Volume)
{
    BSP_AUDIO_Init(AudioFreq, AudioSize, Volume);
    return 0;
}

static int8_t Audio_DeInit(void)
{
    BSP_AUDIO_DeInit();
    return 0;
}

static int8_t Audio_PlaybackCmd(uint8_t *pbuf, uint16_t size)
{
    BSP_AUDIO_Play(pbuf, size);
    return 0;
}

static int8_t Audio_VolumeCtl(uint8_t vol)
{
    BSP_AUDIO_OUT_SetVolume(vol);
    return 0;
}

static int8_t Audio_MuteCtl(uint8_t cmd)
{
    BSP_AUDIO_OUT_SetMute(cmd);
    return 0;
}
