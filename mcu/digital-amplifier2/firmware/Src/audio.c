#include "audio.h"
#include <string.h>
#include <stdio.h>

static void audio_sai_clockConfig(uint32_t AudioFreq);
int16_t audio_clock_samples_delta(void);

Audio audio = {0};

void audio_init(uint32_t audioFreq, uint8_t bit_depth)
{
    printf("audio init: %ld %d\n", audioFreq, bit_depth);

    audio.freq = audioFreq;
    audio.bit_depth = bit_depth;

    audio_sai_clockConfig(audioFreq);

    audio_out1_init(audioFreq, bit_depth);
    /* audio_out2_init(audioFreq, bit_depth); */

    audio.state = AUDIO_STATE_START;
    audio.sync++;
}

void audio_stop(void)
{
    /* printf("audio deinit\n"); */

    audio_out1_reset();
    /* audio_out2_reset(); */

    audio.state = AUDIO_STATE_STOP;
    audio.sync++;
}

static void device_init(void)
{
    /* tas6424_init(); */
    /* tas6424_en(true); */
    /* tas6424_volume(audio.volume); */
    /* tas6424_mute(audio.mute); */
    /* tas6424_play(audio.freq, audio.bit_depth); */

    pcm1792_init();
    pcm1792_volume(audio.volume);
    pcm1792_mute(audio.mute);
    pcm1792_play(audio.freq, audio.bit_depth);
}

static void device_set(void)
{
    /* tas6424_volume(audio.volume); */
    /* tas6424_mute(audio.mute); */
    pcm1792_volume(audio.volume);
    pcm1792_mute(audio.mute);
}

static void device_stop(void)
{
    /* tas6424_stop(); */
    pcm1792_stop();
}

void audio_tick(void)
{
    if (audio.sync > 0) {
        audio.sync--;
        return;
    }

    if (audio.state == AUDIO_STATE_START) {
        audio.state = AUDIO_STATE_RUN;
        device_init();
    }

    if (audio.state == AUDIO_STATE_SET) {
        audio.state = AUDIO_STATE_RUN;
        device_set();
    }

    if (audio.state == AUDIO_STATE_STOP) {
        audio.state = AUDIO_STATE_INIT;
        device_stop();
    }
}

void audio_append(uint8_t* buf, uint16_t buf_len)
{
    audio_out1_append(buf, buf_len);
    /* audio_out2_append(buf, buf_len); */
}

void audio_clock_sync(void)
{
    audio_out1_clock_sync();
    /* audio_out2_clock_sync(); */
}

void audio_setVolume(uint8_t vol)
{
    audio.volume = vol;
    audio.state = AUDIO_STATE_SET;
    audio.sync++;
}

void audio_setMute(bool flag)
{
    audio.mute = flag;
    audio.state = AUDIO_STATE_SET;
    audio.sync++;
}

int16_t audio_clock_samples_delta(void)
{
    return audio_out1_clock_delta();
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
        periphCLK.PLLI2S.PLLI2SR = 18;
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

    return;

    /* 384/192 */
    periphCLK.PeriphClockSelection = RCC_PERIPHCLK_I2S_APB2;
    /* periphCLK.PeriphClockSelection = RCC_PERIPHCLK_PLLI2S; */
    periphCLK.I2sApb2ClockSelection = RCC_I2SAPB2CLKSOURCE_PLLI2S;

    if (HAL_RCCEx_PeriphCLKConfig(&periphCLK) != HAL_OK)
    {
        printf("Periph clock error\n");
    }
}

