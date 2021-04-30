#include "audio.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_ll_dma.h"

/* 16bit/8*2*2000 */
#define BUF_SIZE (1024*16)


extern I2S_HandleTypeDef hi2s1;

static uint8_t audio_buf[BUF_SIZE];
static uint8_t audio_state;
static int16_t audio_buf_size;
static int16_t audio_w_idx;
static uint8_t sample_bit;
static uint8_t sample_size;
static uint8_t audio_sample_size;
static int16_t audio_samples_num;
static int16_t audio_samples_delta;

static void _dma_half_cplt(I2S_HandleTypeDef *hsai);
static void _dma_cplt(I2S_HandleTypeDef *hsai);
static uint16_t audio_rd_idx(void);
static void audio_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num);

int16_t audio_out1_clock_delta(void);

void audio_out1_init(uint32_t AudioFreq, uint8_t bit_depth)
{
    sample_bit = bit_depth;
    sample_size = bit_depth/8*2;

    hi2s1.Init.AudioFreq = AudioFreq;

    if (bit_depth == 16U) {
        hi2s1.Init.DataFormat = I2S_DATAFORMAT_16B;
        audio_sample_size = 4;
    } else if (bit_depth == 24) {
        hi2s1.Init.DataFormat = I2S_DATAFORMAT_24B;
        audio_sample_size = 8;
    } else {
        hi2s1.Init.DataFormat = I2S_DATAFORMAT_32B;
        audio_sample_size = 8;
    }

    audio_samples_num = BUF_SIZE/audio_sample_size;
    audio_buf_size = audio_sample_size * audio_samples_num;

    HAL_I2S_DeInit(&hi2s1);

    if (HAL_I2S_Init(&hi2s1) != HAL_OK) {
        printf("i2s init error\n");
        return;
    }

    HAL_I2S_RegisterCallback(&hi2s1, HAL_I2S_TX_COMPLETE_CB_ID, _dma_cplt);
    HAL_I2S_RegisterCallback(&hi2s1, HAL_I2S_TX_HALF_COMPLETE_CB_ID, _dma_half_cplt);

    HAL_StatusTypeDef ret;
    ret = HAL_I2S_Transmit_DMA(&hi2s1, (uint16_t *)&audio_buf[0], audio_buf_size/2);
    if (ret!= HAL_OK) {
        printf("i2s trans err:%d\n", ret);
        return;
    }

    audio_state = AUDIO_STATE_INIT;
}

void audio_out1_reset(void)
{
    memset(audio_buf, 0, sizeof(audio_buf));
    audio_state = AUDIO_STATE_INIT;
}

void audio_out1_check(void)
{
    return;
}

static void audio_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num)
{
    uint16_t oft, oft2;

    if (sample_bit==16) {
        for (int i=0; i<sample_num; i++) {
            oft2 = ((idx+i) % audio_samples_num) * audio_sample_size;
            oft = 4 * i;
            /* 调换左右声道 */
            audio_buf[oft2+2] = buf[oft];
            audio_buf[oft2+3] = buf[oft+1];
            audio_buf[oft2+0] = buf[oft+2];
            audio_buf[oft2+1] = buf[oft+3];
        }
    } else if (sample_bit==24) {
        for (int i=0; i<sample_num; i++) {
            oft2 = ((idx+i) % audio_samples_num) * audio_sample_size;
            oft = 6 * i;
            audio_buf[oft2+1] = buf[oft+0];
            audio_buf[oft2+2] = buf[oft+1];
            audio_buf[oft2+3] = buf[oft+2];

            audio_buf[oft2+5] = buf[oft+3];
            audio_buf[oft2+6] = buf[oft+4];
            audio_buf[oft2+7] = buf[oft+5];
        }
    } else {
        for (int i=0; i<sample_num; i++) {
            oft2 = ((idx+i) % audio_samples_num) * audio_sample_size;
            oft = 8 * i;
            audio_buf[oft2+0] = buf[oft];
            audio_buf[oft2+1] = buf[oft+1];
            audio_buf[oft2+2] = buf[oft+2];
            audio_buf[oft2+3] = buf[oft+3];
            audio_buf[oft2+4] = buf[oft+4];
            audio_buf[oft2+5] = buf[oft+5];
            audio_buf[oft2+6] = buf[oft+6];
            audio_buf[oft2+7] = buf[oft+7];
        }
    }
}

void audio_out1_append(uint8_t* buf, uint16_t buf_len)
{
    if (audio_state == AUDIO_STATE_INIT) {
        audio_w_idx = audio_rd_idx();
        audio_w_idx += audio_samples_num / 2;
        audio_w_idx %= audio_samples_num;
    }
    audio_state = AUDIO_STATE_RUN;

    uint16_t sample_num = buf_len / sample_size;

    audio_copy(audio_w_idx, buf, sample_num);

    audio_w_idx += sample_num;
    audio_w_idx %= audio_samples_num;
}

void audio_out1_clock_sync(void)
{
    int16_t delta = audio_out1_clock_delta();

    if (delta > 1) {
        audio_w_idx++;
        audio_w_idx %= audio_samples_num;
    }
    else if (delta < -1) {
        if (audio_w_idx == 0) {
            audio_w_idx = audio_samples_num -1;
        } else {
            audio_w_idx--;
        }
    }
}

/* 当前读取指针的位置 */
static uint16_t audio_rd_idx(void)
{
    uint16_t rd_ptr = (LL_DMA_ReadReg(hi2s1.hdmatx->Instance, NDTR) & 0xFFFF);
    uint16_t rd_idx = audio_samples_num - (rd_ptr * 2 / audio_sample_size);
    return rd_idx;
}

int16_t audio_out1_clock_delta(void)
{
    if (audio_state == AUDIO_STATE_INIT) {
        return 0;
    }

    uint16_t rd_idx = audio_rd_idx();

    /* 剩余可写空间 */
    int16_t cap;

    if (audio_w_idx <= rd_idx) {
        cap = rd_idx - audio_w_idx;
    } else {
        cap = audio_samples_num - (audio_w_idx - rd_idx);
    }

    /* 大于0则写入慢了 小于0则 写入快了 */
    int16_t delta = cap - (int16_t)(audio_samples_num / 2);

    return delta;
}

static void _dma_cplt(I2S_HandleTypeDef *hsai)
{
    if (audio_state == AUDIO_STATE_RUN) {
        audio_samples_delta = audio_w_idx - (int16_t)(audio_samples_num / 2);
    }
}

static void _dma_half_cplt(I2S_HandleTypeDef *hsai)
{
    if (audio_state == AUDIO_STATE_SET) {
        audio_out1_reset();
        return;
    }

    if (audio_state == AUDIO_STATE_RUN) {
        audio_state = AUDIO_STATE_SET;
    }
}



