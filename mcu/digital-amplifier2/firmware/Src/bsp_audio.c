#include "base.h"
#include <string.h>
#include <stdio.h>
#include "bsp_audio.h"
#include "stm32f4xx_ll_dma.h"

static uint32_t audio_freq;
static uint32_t audio_buffer[4096];
static int16_t audio_buffer_size;
static uint16_t audio_sample_size;
static uint16_t audio_sample_max;
static volatile int16_t audio_wptr;
static volatile int16_t audio_sample_count;
static volatile uint8_t audio_state;
static uint16_t audio_buffer_max;

#define AUDIO_STATE_HALF 1
#define AUDIO_STATE_SYNC 2

static void BSP_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_ErrorCallback(SAI_HandleTypeDef *hsai);

void BSP_AUDIO_Init(uint32_t AudioFreq, uint8_t AudioSize, uint16_t bufferSize, uint8_t vol)
{
    printf("audio init %ld\n", AudioFreq);

    /* BSP_AUDIO_DeInit(); */
    __HAL_SAI_DISABLE(&hsai_out);

    BSP_SAI_ClockConfig(AudioFreq);

    audio_freq = AudioFreq;
    hsai_out.Init.AudioFrequency = AudioFreq;

    if (HAL_SAI_Init(&hsai_out) != HAL_OK)
    {
        return;
    }

    HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_TX_COMPLETE_CB_ID, BSP_SAI_TxCpltCallback);
    HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_TX_HALFCOMPLETE_CB_ID, BSP_SAI_TxHalfCpltCallback);
    HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_ERROR_CB_ID, BSP_SAI_ErrorCallback);

    __HAL_SAI_ENABLE(&hsai_out);

    memset(audio_buffer, 0, sizeof(audio_buffer));
    audio_wptr = 0;
    audio_sample_count = 0;
    audio_state = 0;
    audio_buffer_size = bufferSize;
    audio_sample_size = 4;
    audio_buffer_max = ((sizeof(audio_buffer) / audio_buffer_size) - 1) * audio_buffer_size;
    audio_sample_max = audio_buffer_max / audio_sample_size;

    HAL_StatusTypeDef ret = HAL_SAI_Transmit_DMA(&hsai_out, (uint8_t*)audio_buffer, audio_buffer_max/2);
    if (ret!= HAL_OK) {
        printf("play err:%d\n", ret);
    }

    BSP_AUDIO_OUT_SetVolume(vol);
    bsp_tas6424_play(AudioFreq);
}

uint8_t *BSP_AUDIO_AllocBuffer(void)
{
    return (uint8_t*)(&audio_buffer[audio_wptr/4]);
}

void BSP_AUDIO_PostBuffer(uint16_t len)
{
    if ( !(audio_state & AUDIO_STATE_SYNC) ) {
        if (audio_state & AUDIO_STATE_HALF) {
            audio_wptr = audio_buffer_max / 2;
        }

        audio_state |= AUDIO_STATE_SYNC;
    }

    audio_sample_count += len / audio_sample_size;

    if (audio_wptr + len < audio_buffer_max) {
        audio_wptr = audio_wptr + len;
    } else {
        audio_wptr = audio_wptr + len - audio_buffer_max;

        uint32_t *dst, *src;
        dst = &audio_buffer[0];
        src = &audio_buffer[audio_buffer_max/4];

        int32_t end = audio_wptr/4;
        while (end--) {
            *dst++ = *src++;
        }
    }
}

void BSP_AUDIO_OUT_SetVolume(uint8_t vol)
{
    printf("volume %d\n", vol);
    volume = vol;
    bsp_tas6424_vol(vol);
}

void BSP_AUDIO_OUT_SetMute(bool flag)
{
    printf("mute %d\n", (uint8_t)flag);

    volume_mute = flag;
    bsp_tas6424_mute((uint8_t)flag);
}

void BSP_AUDIO_DeInit(void)
{
    printf("audio deInit\n");

    bsp_tas6424_deInit();

    __HAL_SAI_DISABLE(&hsai_out);
    HAL_SAI_DeInit(&hsai_out);
}

static void bsp_sai_sync(void)
{
    audio_sample_count -= audio_sample_max / 2;

    if (audio_sample_count > audio_sample_max/4) {
        /* audio_sample_count -= 1; */
        audio_wptr -= audio_buffer_size;

        if (audio_wptr < 0) {
            audio_wptr += audio_buffer_max;
        }
        audio_sample_count = 0;
    } else if (audio_sample_count < -(audio_sample_max/4) ) {
        /* audio_sample_count += 1; */
        audio_wptr += audio_buffer_size;

        if (audio_wptr >= audio_buffer_max) {
            audio_wptr -= audio_buffer_max;
        }
        audio_sample_count = 0;
    }
}

static void BSP_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    audio_state |= AUDIO_STATE_HALF;

    if (audio_state & AUDIO_STATE_SYNC) {
        bsp_sai_sync();
    }
}

static void BSP_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    audio_state &= ~AUDIO_STATE_HALF;

    if (audio_state & AUDIO_STATE_SYNC) {
        bsp_sai_sync();
    }
}

static void BSP_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    uint32_t num = HAL_SAI_GetError(hsai);
    printf("sai err:%ld\n", num);
}





