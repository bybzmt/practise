#include "audio.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_ll_dma.h"

static inline void audio_sample_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num);
static void audio_play(void);
static inline uint16_t audio_buf_size(void);
static int16_t audio_remaining_writable_samples(void);
int16_t audio_clock_samples_delta(void);

void audio_init(uint32_t AudioFreq, uint8_t bit_depth)
{
    printf("audio init\n");

    audio.freq = AudioFreq;
    audio.bit_depth = bit_depth;
    audio.sample_size = bit_depth/8*2;
    audio.wr_idx = AUDIO_BUF_SAMPLE_NUM / 2;
    audio.state = AUDIO_STATE_INIT;

    HAL_SAI_DeInit(&audio.hsai);

    audio.hsai.Init.AudioFrequency = AudioFreq;

    audio.hsai.Init.DataSize = SAI_DATASIZE_16;
    audio.hsai.FrameInit.FrameLength = 16 * 2;
    audio.hsai.FrameInit.ActiveFrameLength = 16;
    audio.hsai.SlotInit.SlotNumber = 2;

    memset(&audio.buf[0], 0, sizeof(audio.buf));

    if (HAL_SAI_Init(&audio.hsai) != HAL_OK) {
        printf("sai init error\n");
        return;
    }
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

    HAL_StatusTypeDef ret = HAL_SAI_Transmit_DMA(&audio.hsai, &audio.buf[0], audio_buf_size()/2);
    if (ret!= HAL_OK) {
        printf("play err:%d\n", ret);
        return;
    }

    tas6424_en(true);
    tas6424_vol(audio.vol);
    tas6424_mute(audio.mute);
    tas6424_play(audio.freq);

    audio.state = AUDIO_STATE_RUN;
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
    int16_t delta = audio_clock_samples_delta();

    if (delta > 1) {
        audio_sample_copy(audio.wr_idx, buf, 1);

        audio.wr_idx++;
        audio.wr_idx %= AUDIO_BUF_SAMPLE_NUM;
    }
    else if (delta < -1) {
        if (audio.wr_idx == 0) {
            audio.wr_idx = AUDIO_BUF_SAMPLE_NUM -1;
        } else {
            audio.wr_idx--;
        }
        audio.wr_idx %= AUDIO_BUF_SAMPLE_NUM;
    }

    audio_append(buf, buf_len);
}

void audio_append(uint8_t* buf, uint16_t buf_len)
{
    uint16_t sample_num = buf_len / audio.sample_size;

    audio_sample_copy(audio.wr_idx, buf, sample_num);

    audio.wr_idx += sample_num;
    audio.wr_idx %= AUDIO_BUF_SAMPLE_NUM;

    if (audio.state == AUDIO_STATE_INIT) {
        audio_play();
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

static inline uint16_t audio_buf_size(void)
{
    return (int32_t)AUDIO_BUF_SAMPLE_NUM * audio.sample_size;
}

static int16_t audio_remaining_writable_samples(void)
{
    if (audio.state != AUDIO_STATE_RUN) {
        return AUDIO_BUF_SAMPLE_NUM/2;
    }

    uint16_t rd_ptr = (LL_DMA_ReadReg(audio.hdma_tx.Instance, NDTR) & 0xFFFF);
    uint16_t rd_idx = AUDIO_BUF_SAMPLE_NUM - (rd_ptr / audio.sample_size);
    uint16_t out;

    if (rd_idx < audio.wr_idx) {
      out = rd_idx + AUDIO_BUF_SAMPLE_NUM - audio.wr_idx;
    } else {
      out = rd_idx - audio.wr_idx;
    }

    return out;
}

int16_t audio_clock_samples_delta(void)
{
    return (int16_t)audio_remaining_writable_samples() - (int16_t)(AUDIO_BUF_SAMPLE_NUM / 2);
}
