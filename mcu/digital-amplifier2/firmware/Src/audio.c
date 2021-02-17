#include "audio.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_ll_dma.h"

static void my_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
static void my_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void my_SAI_ErrorCallback(SAI_HandleTypeDef *hsai);

void audio_init(uint32_t AudioFreq, uint8_t bit_depth)
{
    printf("audio init\n");

    audio.freq = AudioFreq;
    audio.bit_depth = bit_depth;
    audio.heartbeat = 0;
    audio.wr_ptr = 0;
    audio.channel_num = 2;
    audio.enable = true;
    audio.sync = false;

    HAL_SAI_DeInit(&audio.hsai);

    audio.hsai.Init.AudioFrequency = AudioFreq;

    if (bit_depth == 16) {
        audio.hsai.Init.DataSize = SAI_DATASIZE_16;
    } else if (bit_depth == 24) {
        audio.hsai.Init.DataSize = SAI_DATASIZE_24;
    } else if (bit_depth == 32) {
        audio.hsai.Init.DataSize = SAI_DATASIZE_32;
    } else {
        printf("bit_depth error\n");
        return;
    }

    audio.hsai.FrameInit.FrameLength = bit_depth * audio.channel_num;
    audio.hsai.FrameInit.ActiveFrameLength = bit_depth;
    audio.hsai.SlotInit.SlotNumber = audio.channel_num;

    if (HAL_SAI_Init(&audio.hsai) != HAL_OK) {
        printf("sai init error\n");
        return;
    }

    HAL_SAI_RegisterCallback(&audio.hsai, HAL_SAI_TX_COMPLETE_CB_ID, my_SAI_TxCpltCallback);
    HAL_SAI_RegisterCallback(&audio.hsai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, my_SAI_TxHalfCpltCallback);
    HAL_SAI_RegisterCallback(&audio.hsai, HAL_SAI_ERROR_CB_ID, my_SAI_ErrorCallback);

    memset(&audio.buf[0], 0, AUDIO_BUF_SIZE);

    HAL_StatusTypeDef ret;
    ret = HAL_SAI_Transmit_DMA(&audio.hsai, &audio.buf[0], AUDIO_BUF_SIZE/(audio.bit_depth/8));
    if (ret!= HAL_OK) {
        printf("play err:%d\n", ret);
        return;
    }

    tas6424_en(true);
    tas6424_play(audio.freq);
    tas6424_vol(audio.vol);
    tas6424_mute(audio.mute);
}

void audio_deInit()
{
    audio.sync = false;
    audio.enable = false;
    tas6424_mute(true);
    HAL_SAI_DeInit(&audio.hsai);
}

void audio_append(uint8_t* buf, uint16_t buf_len)
{
    uint16_t sample_size = audio.bit_depth/8*2;

    if (audio.sync == false) {
        uint16_t idx = audio_play_idx() / sample_size * sample_size;
        audio.wr_ptr = idx + (AUDIO_BUF_SIZE/2/sample_size*sample_size);
        audio.sync = true;
    }
    audio.heartbeat = 0;

    uint16_t sample_num = buf_len / sample_size;
    uint16_t base;

    for (uint16_t i=0; i<sample_num; i++) {
        base = sample_size * i;

        for (uint16_t x=0; x<sample_size; x++) {
            audio.buf[(audio.wr_ptr+x) % AUDIO_BUF_SIZE] = buf[base+x];
        }

        audio.wr_ptr += sample_size;
        audio.wr_ptr %= AUDIO_BUF_SIZE;
    }

    /* audio.wr_ptr %= AUDIO_BUF_SIZE; */
}

void audio_setVolume(uint8_t vol)
{
    printf("volume: %d\n", vol);
    audio.vol = vol;
    tas6424_vol(vol);
}

void audio_setMute(bool flag)
{
    printf("mute %d\n", flag);
    audio.mute = flag;
    tas6424_mute(flag);
}

static void my_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (audio.heartbeat > 1) {
        audio.enable = false;
        tas6424_mute(true);
    }
    audio.heartbeat++;
}

static void my_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
}

static void my_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    uint32_t num = HAL_SAI_GetError(hsai);
    printf("sai err:%ld\n", num);
}

uint16_t audio_play_idx(void)
{
    return (AUDIO_BUF_SIZE - (LL_DMA_ReadReg(audio.hdma_tx.Instance, NDTR) & 0xFFFF));
}

uint16_t audio_remaining_writable_buffer(void)
{
    uint16_t rd_ptr = audio_play_idx();
    uint16_t out;

    if (rd_ptr < audio.wr_ptr) {
      out = rd_ptr + AUDIO_BUF_SIZE - audio.wr_ptr;
    } else {
      out = rd_ptr - audio.wr_ptr;
    }

    return out;
}
