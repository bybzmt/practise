#include "base.h"
#include <string.h>
#include <stdio.h>
#include "bsp_audio.h"
#include "stm32f4xx_ll_dma.h"

#define AUDIO_FREQ_44     44100U
#define AUDIO_FREQ_48     48000U
#define AUDIO_FREQ_96     96000U
#define AUDIO_PACKET(FREQ)       ((FREQ * 4U) / 1000U)
#define AUDIO_PACKET_NUM   40u
#define AUDIO_PACKET_MAX(FREQ)   (AUDIO_PACKET(FREQ) * AUDIO_PACKET_NUM)

static uint32_t audio_freq;
static uint32_t audio_buffer[AUDIO_PACKET_MAX(AUDIO_FREQ_96)/4];
static volatile int16_t audio_wptr;
static volatile int16_t audio_sample_count;
static volatile uint8_t audio_offset;
static uint16_t audio_sample_max;

static void BSP_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_ErrorCallback(SAI_HandleTypeDef *hsai);

static QueueHandle_t audio_queue;
struct audio_task {
    uint8_t *ptr;
    uint16_t len;
};

uint8_t BSP_AUDIO_Init(uint32_t AudioFreq, uint8_t AudioSize, uint8_t vol)
{
    printf("audio init %ld\n", AudioFreq);

    /* BSP_AUDIO_DeInit(); */
    __HAL_SAI_DISABLE(&hsai_out);

    BSP_SAI_ClockConfig(AudioFreq);

    audio_freq = AudioFreq;
    hsai_out.Init.AudioFrequency = AudioFreq;

    if (HAL_SAI_Init(&hsai_out) != HAL_OK)
    {
        return AUDIO_ERROR;
    }

    HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_TX_COMPLETE_CB_ID, BSP_SAI_TxCpltCallback);
    HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_TX_HALFCOMPLETE_CB_ID, BSP_SAI_TxHalfCpltCallback);
    HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_ERROR_CB_ID, BSP_SAI_ErrorCallback);

    __HAL_SAI_ENABLE(&hsai_out);

    audio_wptr = 0;
    audio_sample_count = 0;
    audio_offset = 0;
    audio_sample_max = AUDIO_PACKET_MAX(audio_freq) / 4;
    memset(audio_buffer, 0, sizeof(audio_buffer));

    HAL_StatusTypeDef ret = HAL_SAI_Transmit_DMA(&hsai_out, audio_buffer, AUDIO_PACKET_MAX(audio_freq)/2);
    if (ret!= HAL_OK) {
        printf("play err:%d\n", ret);
    }

    BSP_AUDIO_OUT_SetVolume(vol);
    bsp_tas6424_play(AudioFreq);

    return AUDIO_OK;
}

uint8_t BSP_AUDIO_Play(uint8_t* pBuffer, uint16_t Size)
{
    struct audio_task tmp = {
        .ptr = pBuffer,
        .len = Size,
    };

    xQueueSendFromISR(audio_queue, &tmp, NULL);
    return AUDIO_OK;
}

void BSP_AUDIO_service(void)
{
    struct audio_task tmp;

    audio_queue = xQueueCreate(2, sizeof(struct audio_task));

    for(;;) {
        if (xQueueReceive(audio_queue, &tmp, pdMS_TO_TICKS(500)) == pdPASS) {
            /* 16bit audio * 2 */
            uint8_t step = 4;
            int16_t num = tmp.len / step;
            int16_t end;

            audio_sample_count += num;

            if (audio_offset < 2) {
                if (audio_offset == 0) {
                    audio_wptr = audio_sample_max / 2;
                }
                audio_offset = 2;
            }

            uint32_t *dst, *src;
            src = (uint32_t*)(tmp.ptr);

            if (audio_wptr + num >= audio_sample_max)
            {
                end = audio_sample_max - audio_wptr;
                dst = &audio_buffer[audio_wptr];
                audio_wptr = audio_wptr + num - audio_sample_max;

                while (end--) {
                    *dst++ = *src++;
                }

                dst = &audio_buffer[0];
                end = audio_wptr;

                while (end--) {
                    *dst++ = *src++;
                }
            } else {
                dst = &audio_buffer[audio_wptr];
                end = num;

                while (end--) {
                    *dst++ = *src++;
                }

                audio_wptr = audio_wptr + num;
            }
        }
    }

}

uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t vol)
{
    printf("volume %d\n", vol);
    volume = vol;
    bsp_tas6424_vol(vol);
    return AUDIO_OK;
}

uint8_t BSP_AUDIO_OUT_SetMute(bool flag)
{
    printf("mute %d\n", (uint8_t)flag);

    volume_mute = flag;
    bsp_tas6424_mute((uint8_t)flag);
    return AUDIO_OK;
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

    if (audio_sample_count > audio_sample_max / 4) {
        audio_sample_count -= 1;
        audio_wptr = audio_wptr - 1;

        if (audio_wptr < 0) {
            audio_wptr += audio_sample_max;
        }
    } else if (audio_sample_count < -((int)(audio_sample_max / 4)) ) {
        audio_sample_count += 1;
        audio_wptr = audio_wptr + 1;

        if (audio_wptr >= audio_sample_max) {
            audio_wptr -= audio_sample_max;
        }
    }
}

static void BSP_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (audio_offset>1) {
        bsp_sai_sync();
    } else {
        audio_offset=0;
    }
}

static void BSP_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (audio_offset>1) {
        bsp_sai_sync();
    } else {
        audio_offset=1;
    }
}

static void BSP_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    uint32_t num = HAL_SAI_GetError(hsai);
    printf("sai err:%ld\n", num);
}





