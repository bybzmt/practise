
/* Includes ------------------------------------------------------------------*/
#include "usbd_audio_if.h"
#include "base.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int8_t Audio_Init(uint32_t AudioFreq, uint32_t Volume, uint32_t options);
static int8_t Audio_DeInit(uint32_t options);
static int8_t Audio_PlaybackCmd(uint8_t* pbuf, uint32_t size, uint8_t cmd);
static int8_t Audio_VolumeCtl(uint8_t vol);
static int8_t Audio_MuteCtl(uint8_t cmd);
static int8_t Audio_PeriodicTC(uint8_t *pbuf, uint32_t size, uint8_t cmd);
static int8_t Audio_GetState(void);

/* Private variables ---------------------------------------------------------*/
extern USBD_HandleTypeDef USBD_Device;
USBD_AUDIO_ItfTypeDef USBD_AUDIO_fops = {
    Audio_Init,
    Audio_DeInit,
    Audio_PlaybackCmd,
    Audio_VolumeCtl,
    Audio_MuteCtl,
    Audio_PeriodicTC,
    Audio_GetState,
};

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the AUDIO media low layer.
 * @param  AudioFreq: Audio frequency used to play the audio stream.
 * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
 * @param  options: Reserved for future use
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t Audio_Init(uint32_t  AudioFreq, uint32_t Volume, uint32_t options)
{
    return 0;
}

/**
 * @brief  De-Initializes the AUDIO media low layer.
 * @param  options: Reserved for future use
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t Audio_DeInit(uint32_t options)
{
    return 0;
}

/**
 * @brief  Handles AUDIO command.
 * @param  pbuf: Pointer to buffer of data to be sent
 * @param  size: Number of data to be sent (in bytes)
 * @param  cmd: Command opcode
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t Audio_PlaybackCmd(uint8_t *pbuf, uint32_t size, uint8_t cmd)
{
    return 0;
}

/**
 * @brief  Controls AUDIO Volume.
 * @param  vol: Volume level (0..100)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t Audio_VolumeCtl(uint8_t vol)
{
    audio_setVolume(vol);
    return 0;
}

/**
 * @brief  Controls AUDIO Mute.
 * @param  cmd: Command opcode
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t Audio_MuteCtl(uint8_t cmd)
{
    audio_setMute(cmd);
    return 0;
}

/**
 * @brief  Audio_PeriodicTC
 * @param  cmd: Command opcode
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t Audio_PeriodicTC(uint8_t *pbuf, uint32_t size, uint8_t cmd)
{
    return 0;
}

/**
 * @brief  Gets AUDIO State.
 * @param  None
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t Audio_GetState(void)
{
    return 0;
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void) {
    USBD_AUDIO_Sync(&USBD_Device, AUDIO_OFFSET_FULL);
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void) {
    USBD_AUDIO_Sync(&USBD_Device, AUDIO_OFFSET_HALF);
}

