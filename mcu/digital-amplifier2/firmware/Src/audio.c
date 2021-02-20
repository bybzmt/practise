#include "audio.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_ll_dma.h"

static inline void audio_sample_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num);
static void audio_play(void);
static void my_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
static void my_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void my_SAI_ErrorCallback(SAI_HandleTypeDef *hsai);

void audio_init(uint32_t AudioFreq, uint8_t bit_depth)
{
    printf("audio init\n");

    audio.freq = AudioFreq;
    audio.bit_depth = bit_depth;
    audio.sample_size = bit_depth/8*2;
    audio.sample_count = 0;
    audio.sample_diff = 0;
    audio.wr_idx = 0;
    audio.state = AUDIO_STATE_INIT;

    HAL_SAI_DeInit(&audio.hsai);

    audio.hsai.Init.AudioFrequency = AudioFreq;

    audio.hsai.Init.DataSize = SAI_DATASIZE_16;
    audio.hsai.FrameInit.FrameLength = 16 * 2;
    audio.hsai.FrameInit.ActiveFrameLength = 16;
    audio.hsai.SlotInit.SlotNumber = 2;

    memset(&audio.buf[0], 0, AUDIO_BUF_SIZE);

    if (HAL_SAI_Init(&audio.hsai) != HAL_OK) {
        printf("sai init error\n");
        return;
    }

    HAL_SAI_RegisterCallback(&audio.hsai, HAL_SAI_TX_COMPLETE_CB_ID, my_SAI_TxCpltCallback);
    HAL_SAI_RegisterCallback(&audio.hsai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, my_SAI_TxHalfCpltCallback);
    HAL_SAI_RegisterCallback(&audio.hsai, HAL_SAI_ERROR_CB_ID, my_SAI_ErrorCallback);
}

void audio_deInit()
{
    printf("audio deInit\n");
    audio.state = AUDIO_STATE_INIT;
    tas6424_en(false);
    HAL_SAI_DeInit(&audio.hsai);
}

static void audio_play(void)
{
    printf("play\n");

    HAL_StatusTypeDef ret = HAL_SAI_Transmit_DMA(&audio.hsai, &audio.buf[0], AUDIO_BUF_SIZE/2);
    if (ret!= HAL_OK) {
        printf("play err:%d\n", ret);
        return;
    }

    tas6424_en(true);
    tas6424_vol(audio.vol);
    tas6424_mute(audio.mute);
    tas6424_play(audio.freq);
}

static inline void audio_sample_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num)
{
    uint16_t base, oft;

    for (uint16_t i=0; i<sample_num; i++) {
        base = AUDIO_SAMPLE_SIZE * idx;
        oft = audio.sample_size * i;

        if (audio.bit_depth == 16) {
            audio.buf[base+0] = buf[oft + 0];
            audio.buf[base+1] = buf[oft + 1];
            audio.buf[base+2] = buf[oft + 2];
            audio.buf[base+3] = buf[oft + 3];
        }
        else if (audio.bit_depth == 24) {
            audio.buf[base+0] = buf[oft + 0];
            audio.buf[base+1] = buf[oft + 1];
            audio.buf[base+2] = buf[oft + 3];
            audio.buf[base+3] = buf[oft + 4];
        }

        idx = (idx + 1) % AUDIO_BUF_SAMPLE_NUM;
    }
}

void audio_append_adapt(uint8_t* buf, uint16_t buf_len)
{
    if (audio.sample_diff > 0) {
        audio.sample_diff--;

        if (audio.wr_idx == 0) {
            audio.wr_idx = AUDIO_BUF_SAMPLE_NUM -1;
        } else {
            audio.wr_idx--;
        }

        audio.wr_idx %= AUDIO_BUF_SAMPLE_NUM;
    } else if (audio.sample_diff < 0) {
        audio.sample_diff++;

        audio_sample_copy(audio.wr_idx, buf, 1);

        audio.wr_idx++;
        audio.wr_idx %= AUDIO_BUF_SAMPLE_NUM;
    }

    audio_append(buf, buf_len);
}

void audio_append(uint8_t* buf, uint16_t buf_len)
{
    uint16_t sample_num = buf_len / audio.sample_size;
    audio.sample_count += sample_num;

    audio_sample_copy(audio.wr_idx, buf, sample_num);

    audio.wr_idx += sample_num;
    audio.wr_idx %= AUDIO_BUF_SAMPLE_NUM;

    if (audio.state == AUDIO_STATE_INIT) {
        if (audio.wr_idx >= AUDIO_BUF_SAMPLE_NUM/2) {
            audio_play();
            audio.state = AUDIO_STATE_SYNC;
        }
    }
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
    if (audio.state == AUDIO_STATE_INIT) {
        return;
    }

    if (audio.state == AUDIO_STATE_SYNC) {
        audio.state = AUDIO_STATE_RUN;
        audio.sample_count = 0;
        audio.sample_diff = 0;
        return;
    }

    if (audio.state == AUDIO_STATE_RUN) {
        audio.sample_count -= AUDIO_BUF_SAMPLE_NUM;
        audio.sample_diff += audio.sample_count;
        audio.sample_count = 0;

        if (audio.sample_diff < -(AUDIO_BUF_SAMPLE_NUM*2)) {
            audio.state = AUDIO_STATE_ERROR;
            tas6424_en(false);
        }
    }
}

static void my_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
}

static void my_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    uint32_t num = HAL_SAI_GetError(hsai);
    printf("sai err:%ld\n", num);
}

uint16_t audio_buf_size(void)
{
    return AUDIO_BUF_SAMPLE_NUM * audio.sample_size;
}

uint16_t audio_remaining_writable_buffer(void)
{
    uint16_t buf_size = audio_buf_size();

    if (audio.state != AUDIO_STATE_RUN) {
        return buf_size/2;
    }

    uint16_t rd_ptr = (AUDIO_BUF_SIZE - (LL_DMA_ReadReg(audio.hdma_tx.Instance, NDTR) & 0xFFFF));
    uint16_t rd_idx = rd_ptr / 4 / 4;
    uint16_t out;

    if (rd_idx < audio.wr_idx) {
      out = rd_idx + AUDIO_BUF_SAMPLE_NUM - audio.wr_idx;
    } else {
      out = rd_idx - audio.wr_idx;
    }

    return out * audio.sample_size;
}
