#include "audio.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_ll_dma.h"

static void audio_sai_clockConfig(uint32_t AudioFreq);
static inline void audio_sample_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num);
static void audio_play(void);
static inline uint16_t audio_buf_size(void);
static int16_t audio_remaining_writable_samples(void);
int16_t audio_clock_samples_delta(void);

Audio audio;

uint8_t buf[2048];

void audio_init(uint32_t AudioFreq, uint8_t bit_depth)
{
    printf("audio init\n");

    audio_sai_clockConfig(AudioFreq);

    audio.freq = AudioFreq;
    audio.bit_depth = bit_depth;
    audio.sample_size = bit_depth/8*2;
    audio.wr_idx = AUDIO_BUF_SAMPLE_NUM / 2;
    audio.state = AUDIO_STATE_INIT;

    HAL_SAI_DeInit(&hsai);

    hsai.Init.AudioFrequency = AudioFreq;
    hi2s.Init.AudioFreq = AudioFreq;

    if (bit_depth == 16U) {
        hsai.Init.DataSize = SAI_DATASIZE_16;
        hi2s.Init.DataFormat = I2S_DATAFORMAT_16B;
    } else {
        hsai.Init.DataSize = SAI_DATASIZE_24;
        hi2s.Init.DataFormat = I2S_DATAFORMAT_24B;
    }
    hsai.FrameInit.FrameLength = audio.bit_depth * 4;
    hsai.FrameInit.ActiveFrameLength = audio.bit_depth;
    hsai.SlotInit.SlotNumber = 4;

    memset(&audio.buf[0], 0, sizeof(audio.buf));

    if (HAL_SAI_Init(&hsai) != HAL_OK) {
        printf("sai init error\n");
        return;
    }
    if (HAL_I2S_Init(&hi2s) != HAL_OK) {
        printf("i2s init error\n");
        return;
    }

    HAL_StatusTypeDef ret;
    ret = HAL_SAI_Transmit_DMA(&hsai, (uint8_t *)&audio.buf[0], audio_buf_size()/2);
    if (ret!= HAL_OK) {
        printf("play err:%d\n", ret);
        return;
    }

    ret = HAL_I2S_Transmit_DMA(&hi2s, (uint16_t *)&buf[0], 2048/2);
    if (ret!= HAL_OK) {
        printf("play err:%d\n", ret);
        return;
    }
}

void audio_deInit(void)
{
    printf("audio deInit\n");
    audio.state = AUDIO_STATE_INIT;
    tas6424_stop();
    HAL_SAI_DeInit(&hsai);
    HAL_I2S_DeInit(&hi2s);
}

static void audio_play(void)
{
    printf("play\n");

    tas6424_en(true);
    tas6424_volume(audio.volume);
    tas6424_mute(audio.mute);
    tas6424_play(audio.freq, audio.bit_depth);

    pcm1792_volume(audio.volume);
    pcm1792_mute(audio.mute);
    pcm1792_play(audio.freq, audio.bit_depth);
}

void audio_check(void)
{
    if (audio.state == AUDIO_STATE_SYNC) {
        audio_deInit();
        audio.state = AUDIO_STATE_ERROR;
        return;
    }

    if (audio.state == AUDIO_STATE_RUN) {
        audio.state = AUDIO_STATE_SYNC;
    }
}

static inline void audio_sample_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num)
{
    uint16_t base, oft;

    for (uint16_t i=0; i<sample_num; i++) {
        oft = audio.sample_size * i;
        base = audio.sample_size * idx * 2;

        if (audio.bit_depth == 16) {
            audio.buf[base+0] = buf[oft + 0];
            audio.buf[base+1] = buf[oft + 1];
            audio.buf[base+2] = buf[oft + 2];
            audio.buf[base+3] = buf[oft + 3];
        } else if (audio.bit_depth == 24) {
            audio.buf[base+0] = buf[oft + 0];
            audio.buf[base+1] = buf[oft + 1];
            audio.buf[base+2] = buf[oft + 2];
            audio.buf[base+3] = buf[oft + 3];
            audio.buf[base+4] = buf[oft + 4];
            audio.buf[base+5] = buf[oft + 5];
        } else if (audio.bit_depth == 32) {
            audio.buf[base+0] = buf[oft + 0];
            audio.buf[base+1] = buf[oft + 1];
            audio.buf[base+2] = buf[oft + 2];
            audio.buf[base+3] = buf[oft + 3];
            audio.buf[base+4] = buf[oft + 4];
            audio.buf[base+5] = buf[oft + 5];
            audio.buf[base+6] = buf[oft + 6];
            audio.buf[base+7] = buf[oft + 7];
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

    audio.state = AUDIO_STATE_RUN;
}

void audio_setVolume(uint8_t vol)
{
    printf("volume: %d\n", vol);
    audio.volume = vol;
    tas6424_volume(vol);
}

void audio_setMute(bool flag)
{
    printf("mute %d\n", flag);
    audio.mute = flag;
    tas6424_mute(flag);
}

static inline uint16_t audio_buf_size(void)
{
    return (int32_t)AUDIO_BUF_SAMPLE_NUM * audio.sample_size * 2;
}

static int16_t audio_remaining_writable_samples(void)
{
    if (audio.state != AUDIO_STATE_RUN) {
        return AUDIO_BUF_SAMPLE_NUM/2;
    }

    uint16_t rd_ptr = (LL_DMA_ReadReg(hsai.hdmatx->Instance, NDTR) & 0xFFFF);
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

static void audio_sai_clockConfig(uint32_t AudioFreq)
{
    RCC_PeriphCLKInitTypeDef periphCLK;

    HAL_RCCEx_GetPeriphCLKConfig(&periphCLK);
    periphCLK.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    periphCLK.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLI2S;
    /* periphCLK.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI; */

    /* Set the PLL configuration according to the audio frequency */
    if (
            (AudioFreq == SAI_AUDIO_FREQUENCY_11K) ||
            (AudioFreq == SAI_AUDIO_FREQUENCY_22K) ||
            (AudioFreq == SAI_AUDIO_FREQUENCY_44K))
    {
        /* Configure PLLSAI prescalers */
        /* PLLSAI_VCO: VCO_429M
           SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
           SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 214.5/19 = 11.289 Mhz */
        periphCLK.PLLI2S.PLLI2SM = 8;
        periphCLK.PLLI2S.PLLI2SN = 429;
        periphCLK.PLLI2S.PLLI2SQ = 2;
        periphCLK.PLLI2S.PLLI2SR = 38;
        periphCLK.PLLI2S.PLLI2SP = 2;
        periphCLK.PLLI2SDivQ = 19;

        /* periphCLK.PLLSAI.PLLSAIM = 8; */
        /* periphCLK.PLLSAI.PLLSAIN = 429; */
        /* periphCLK.PLLSAI.PLLSAIQ = 2; */
        /* periphCLK.PLLSAI.PLLSAIP = 2; */
        /* periphCLK.PLLSAIDivQ = 19; */
    } else {
        /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_48K), AUDIO_FREQUENCY_96K */
        /* SAI clock config
           PLLSAI_VCO: VCO_344M
           SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 344/7 = 49.142 Mhz
           SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 49.142/1 = 49.142 Mhz */
        periphCLK.PLLI2S.PLLI2SM = 8;
        periphCLK.PLLI2S.PLLI2SN = 344;
        periphCLK.PLLI2S.PLLI2SQ = 7;
        periphCLK.PLLI2S.PLLI2SP = 2;
        periphCLK.PLLI2S.PLLI2SR = 7;
        periphCLK.PLLI2SDivQ = 1;

        /* periphCLK.PLLSAI.PLLSAIM = 8; */
        /* periphCLK.PLLSAI.PLLSAIN = 344; */
        /* periphCLK.PLLSAI.PLLSAIQ = 7; */
        /* periphCLK.PLLSAI.PLLSAIP = 7; */
        /* periphCLK.PLLSAIDivQ = 1; */
    }

    if (HAL_RCCEx_PeriphCLKConfig(&periphCLK) != HAL_OK)
    {
        printf("Periph clock error\n");
    }

    /* 384/192 */
    periphCLK.PeriphClockSelection = RCC_PERIPHCLK_I2S_APB2;
    /* periphCLK.PeriphClockSelection = RCC_PERIPHCLK_PLLI2S; */
    periphCLK.I2sApb2ClockSelection = RCC_I2SAPB2CLKSOURCE_PLLI2S;

    if (HAL_RCCEx_PeriphCLKConfig(&periphCLK) != HAL_OK)
    {
        printf("Periph clock error\n");
    }
}

