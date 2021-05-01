#include "audio.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_ll_dma.h"

/* 16bit/8*4*1000 */
#define BUF_SIZE 8000

static uint8_t audio_buf[BUF_SIZE];
static uint8_t audio_state;
static int16_t audio_buf_size;
static int16_t audio_w_idx;
static uint8_t sample_bit;
static uint8_t sample_size;
static uint8_t audio_samples_num;
static int16_t samples_delta_count;

static void _dma_half_cplt(SAI_HandleTypeDef *hsai);
static uint16_t audio_rd_idx(void);
static void audio_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num);

int16_t audio_out2_clock_delta(void);

void audio_out2_init(uint32_t AudioFreq, uint8_t bit_depth)
{
    sample_bit = bit_depth;
    sample_size = bit_depth/8*2;
    audio_samples_num = BUF_SIZE/sample_size/2;
    audio_buf_size = sample_size * audio_samples_num * 2;

    hsai_out.Init.AudioFrequency = AudioFreq;

    if (bit_depth == 16U) {
        hsai_out.Init.DataSize = SAI_DATASIZE_16;
    } else {
        hsai_out.Init.DataSize = SAI_DATASIZE_24;
    }

    HAL_SAI_DeInit(&hsai_out);
    if (HAL_SAI_Init(&hsai_out) != HAL_OK) {
        printf("sai init error\n");
        return;
    }

    /* HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_TX_COMPLETE_CB_ID, _dma_half_cplt); */
    /* HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_TX_HALFCOMPLETE_CB_ID, _dma_half_cplt); */

    /* HAL_StatusTypeDef ret; */
    /* ret = HAL_SAI_Transmit_DMA(&hsai_out, (uint8_t *)&audio_buf[0], audio_buf_size/2); */
    /* if (ret!= HAL_OK) { */
        /* printf("play err:%d\n", ret); */
        /* return; */
    /* } */
}

void audio_out2_reset(void)
{
    memset(audio_buf, 0, sizeof(audio_buf));
    audio_state = AUDIO_STATE_INIT;
}

void audio_out2_check(void)
{
    printf("delta2 %d\n", samples_delta_count);
    samples_delta_count = 0;
    return;
}

static void audio_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num)
{
}

void audio_out2_append(uint8_t* buf, uint16_t buf_len)
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

void audio_out2_clock_sync(void)
{
    int16_t delta = audio_out2_clock_delta();

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
    uint16_t rd_ptr = (LL_DMA_ReadReg(hsai_out.hdmatx->Instance, NDTR) & 0xFFFF);
    uint16_t rd_idx = (rd_ptr / sample_size / 2);
    return rd_idx;
}

int16_t audio_out2_clock_delta(void)
{
    if (audio_state != AUDIO_STATE_RUN) {
        return 0;
    }

    uint16_t rd_idx = audio_rd_idx();

    /* 剩余可写空间 */
    int16_t cap;

    if (rd_idx < audio_w_idx) {
        cap = audio_samples_num - (audio_w_idx - rd_idx);
    } else {
        cap = rd_idx - audio_w_idx;
    }

    int16_t delta = cap - (int16_t)(audio_samples_num / 2);
    samples_delta_count += delta;
    return delta;
}

static void _dma_half_cplt(SAI_HandleTypeDef *hsai)
{
    if (audio_state == AUDIO_STATE_SET) {
        /* audio_out2_reset(); */
        return;
    }

    if (audio_state == AUDIO_STATE_RUN) {
        audio_state = AUDIO_STATE_SET;
    }
}

