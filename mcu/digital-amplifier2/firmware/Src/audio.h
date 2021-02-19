#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "base.h"
#include <string.h>
#include <stdio.h>

#define AUDIO_BUF_SAMPLE_NUM 1000
#define AUDIO_32BIT_SAMPLE_SIZE (4*2)
#define AUDIO_BUF_SIZE (AUDIO_32BIT_SAMPLE_SIZE * AUDIO_BUF_SAMPLE_NUM)

#define AUDIO_STATE_INIT   0
#define AUDIO_STATE_SYNC   1
#define AUDIO_STATE_RUN    2
#define AUDIO_STATE_ERROR  4

typedef struct {
    uint8_t buf[AUDIO_BUF_SIZE];
    uint32_t freq;
    volatile int16_t sample_count;
    volatile int16_t sample_diff;
    volatile uint16_t wr_idx;
    volatile uint8_t state;
    uint8_t bit_depth;
    uint8_t sample_size;
    uint8_t channel_num;
    uint8_t vol;
    bool mute;
    SAI_HandleTypeDef hsai;
    DMA_HandleTypeDef hdma_tx;
} Audio;

extern Audio audio;

void audio_init(uint32_t AudioFreq, uint8_t sample_size);
void audio_deInit();
void audio_append(uint8_t* buf, uint16_t buf_len);
void audio_append_adapt(uint8_t* buf, uint16_t buf_len);
void audio_setVolume(uint8_t vol);
void audio_setMute(bool flag);

uint16_t audio_buf_size(void);
uint16_t audio_remaining_writable_buffer(void);

#ifdef __cplusplus
}
#endif

#endif
