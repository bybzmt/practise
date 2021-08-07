#include "base.h"
#include "audio.h"
#include "msp_sai_out.h"

static uint16_t audio_rd_idx(void);
static void audio_sai_init(uint32_t AudioFreq, uint8_t bit_depth);
static void audio_dma_cplt_cb(SAI_HandleTypeDef *hsai);

audio_t audio = {
    .power_from_usb = 1,
};

void audio_init(uint32_t audioFreq, uint8_t bit_depth)
{
    printf("audio init: %ld %d\n", audioFreq, bit_depth);

    audio.freq = audioFreq;
    audio.bit_depth = bit_depth;
    audio.sample_size = bit_depth/8*2;

    audio.w_idx = 0;
    audio.is_play = 0;
    audio.all_zero = 65535;

    audio_sai_init(audioFreq, bit_depth);
}

void audio_play(void)
{
    audio.state = AUDIO_STATE_INIT;

    audio.is_play = true;
    audio.all_zero = 65535;

    audio_notify_dev();
}

void audio_stop(void)
{
    memset(audio.out_buf, 0, sizeof(audio.out_buf));

    audio.is_play = false;

    audio_notify_dev();
}

void audio_notify_dev(void)
{
    uint32_t data = 0;
    data |= settings.input_mode << 24;

    if (audio.is_play) {
        /* 非自动关闭 或 自动关闭下有数据 */
        if (!settings.auto_off || (settings.auto_off && audio.all_zero != 65535)) {
            if (settings.headphone_on) {
                data |= 1 << 20;
            }
            if (settings.speakers_on) {
                data |= 1 << 16;
            }
        }
    }

    switch (audio.freq)
    {
        case SAI_AUDIO_FREQUENCY_44K:  data |= 0<<12; break;
        case SAI_AUDIO_FREQUENCY_48K:  data |= 1<<12; break;
        case SAI_AUDIO_FREQUENCY_96K:  data |= 2<<12; break;
        case SAI_AUDIO_FREQUENCY_192K: data |= 3<<12; break;
    }

    switch (audio.bit_depth)
    {
        case 16:  data |= 0<<8; break;
        case 24:  data |= 1<<8; break;
        case 32:  data |= 2<<8; break;
    }

    if (!settings.mute) {
        if (audio.power_from_usb && settings.vol > VOLUME_ON_USB_POWER) {
            data |= VOLUME_ON_USB_POWER;
        } else {
            data |= settings.vol;
        }
    }

    if (__get_IPSR()) {
        xTaskNotifyFromISR(audio.out_task_hd, data, eSetValueWithOverwrite, NULL);
    } else {
        xTaskNotify(audio.out_task_hd, data, eSetValueWithOverwrite);
    }
}

void audio_append(uint8_t* buf, uint16_t buf_len)
{
    if (audio.state == AUDIO_STATE_INIT) {
        audio.w_idx = audio_rd_idx();
        audio.w_idx += AUDIO_BUF_SAMPLE_NUM / 2;
        audio.w_idx %= AUDIO_BUF_SAMPLE_NUM;
        audio.state = AUDIO_STATE_RUN;
    } else if (audio.state == AUDIO_STATE_SYNC) {
        audio.state = AUDIO_STATE_RUN;
    }

    bool all_zero = true;
    for (int i=0; i<buf_len; i++) {
        if (buf[i]) {
            all_zero = false;
            break;
        }
    }

    if (all_zero) {
        if (audio.all_zero < 3000) {
            audio.all_zero++;
        } else if (audio.all_zero < 65535) {
            audio.all_zero = 65535;
            audio_notify_dev();
        }
    } else {
        if (audio.all_zero == 65535) {
            audio.all_zero = 0;
            audio_notify_dev();
        } else {
            audio.all_zero = 0;
        }
    }

    uint16_t sample_num = buf_len / audio.sample_size;

    audio_copy(audio.w_idx, buf, sample_num);

    audio.w_idx += sample_num;
    audio.w_idx %= AUDIO_BUF_SAMPLE_NUM;
}

void audio_clock_sync(void)
{
    int16_t delta = audio_clock_delta();

    if (delta > 1) {
        audio.w_idx++;
        audio.w_idx %= AUDIO_BUF_SAMPLE_NUM;
    }
    else if (delta < -1) {
        if (audio.w_idx == 0) {
            audio.w_idx = AUDIO_BUF_SAMPLE_NUM -1;
        } else {
            audio.w_idx--;
        }
    }
}

/* 当前读取指针的位置 */
static uint16_t audio_rd_idx(void)
{
    uint16_t remaining = msp_sai_out_dma_remaining();
    uint16_t rd_idx = AUDIO_BUF_SAMPLE_NUM - (remaining / 2);
    return rd_idx;
}

int16_t audio_clock_delta(void)
{
    uint16_t rd_idx = audio_rd_idx();

    /* 剩余可写空间 */
    int16_t cap;

    if (audio.w_idx <= rd_idx) {
        cap = rd_idx - audio.w_idx;
    } else {
        cap = AUDIO_BUF_SAMPLE_NUM - (audio.w_idx - rd_idx);
    }

    /* 大于0则写入慢了 小于0则 写入快了 */
    int16_t delta = cap - (int16_t)(AUDIO_BUF_SAMPLE_NUM / 2);

    return delta;
}

void audio_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num)
{
    uint16_t oft, oft2;

    if (audio.bit_depth==16) {
        for (int i=0; i<sample_num; i++) {
            oft2 = ((idx+i) % AUDIO_BUF_SAMPLE_NUM) * AUDIO_SAMPLE_SIZE;
            oft = 4 * i;
            audio.out_buf[oft2+0] = buf[oft];
            audio.out_buf[oft2+1] = buf[oft+1];
            audio.out_buf[oft2+2] = 0;
            audio.out_buf[oft2+3] = 0;
            audio.out_buf[oft2+4] = buf[oft+2];
            audio.out_buf[oft2+5] = buf[oft+3];
            audio.out_buf[oft2+6] = 0;
            audio.out_buf[oft2+7] = 0;
        }
    } else if (audio.bit_depth==24) {
        for (int i=0; i<sample_num; i++) {
            oft2 = ((idx+i) % AUDIO_BUF_SAMPLE_NUM) * AUDIO_SAMPLE_SIZE;
            oft = 6 * i;
            audio.out_buf[oft2+0] = buf[oft+0];
            audio.out_buf[oft2+1] = buf[oft+1];
            audio.out_buf[oft2+2] = buf[oft+2];
            audio.out_buf[oft2+3] = 0;
            audio.out_buf[oft2+4] = buf[oft+3];
            audio.out_buf[oft2+5] = buf[oft+4];
            audio.out_buf[oft2+6] = buf[oft+5];
            audio.out_buf[oft2+7] = 0;
        }
    } else {
        for (int i=0; i<sample_num; i++) {
            oft2 = ((idx+i) % AUDIO_BUF_SAMPLE_NUM) * AUDIO_SAMPLE_SIZE;
            oft = 8 * i;
            audio.out_buf[oft2+0] = buf[oft];
            audio.out_buf[oft2+1] = buf[oft+1];
            audio.out_buf[oft2+2] = buf[oft+2];
            audio.out_buf[oft2+3] = buf[oft+3];
            audio.out_buf[oft2+4] = buf[oft+4];
            audio.out_buf[oft2+5] = buf[oft+5];
            audio.out_buf[oft2+6] = buf[oft+6];
            audio.out_buf[oft2+7] = buf[oft+7];
        }
    }
}

static void audio_dma_cplt_cb(SAI_HandleTypeDef *hsai)
{
    if (audio.state == AUDIO_STATE_SYNC) {
        memset(audio.out_buf, 0, sizeof(audio.out_buf));
        audio.state = AUDIO_STATE_INIT;
        return;
    }

    if (audio.state == AUDIO_STATE_RUN) {
        audio.state = AUDIO_STATE_SYNC;
    }
}

static void audio_sai_init(uint32_t AudioFreq, uint8_t bit_depth)
{
    msp_sai_out_deInit();

    msp_sai_out_init(AudioFreq, bit_depth);

    HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_TX_COMPLETE_CB_ID, audio_dma_cplt_cb);

    HAL_StatusTypeDef ret;
    ret = HAL_SAI_Transmit_DMA(&hsai_out, &audio.out_buf[0], AUDIO_OUT_BUF_SIZE/4);
    if (ret!= HAL_OK) {
        printf("sai out dma err\n");
        return;
    }
}

