#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "base.h"
#include <string.h>
#include <stdio.h>

#define AUDIO_BUF_SIZE (1024*8)

typedef struct {
    uint8_t buf[AUDIO_BUF_SIZE];
    uint32_t freq;
    volatile uint16_t offset;
    uint16_t wr_ptr;
    bool enable;
    bool sync;
    uint8_t bit_depth;
    uint8_t vol;
    bool mute;
    SAI_HandleTypeDef hsai;
    DMA_HandleTypeDef hdma_tx;
} Audio;

extern Audio audio;

void audio_init(uint32_t AudioFreq, uint8_t sample_size);
void audio_deInit();
void audio_append(uint8_t* buf, uint16_t buf_len);
void audio_mixer(uint16_t idx, uint8_t* buf, uint16_t buf_len);
void audio_setVolume(uint8_t vol);
void audio_setMute(bool flag);

uint16_t audio_play_idx(void);
uint16_t audio_remaining_writable_buffer(void);

#ifdef __cplusplus
}
#endif

#endif
