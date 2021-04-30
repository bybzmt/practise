#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "base.h"
#include <string.h>
#include <stdio.h>

#define AUDIO_BUF_SAMPLE_NUM 1000
#define AUDIO_SAMPLE_SIZE (4*4)
#define AUDIO_BUF_MAX_SIZE (AUDIO_SAMPLE_SIZE * AUDIO_BUF_SAMPLE_NUM)

#define AUDIO_STATE_INIT   0
#define AUDIO_STATE_START  1
#define AUDIO_STATE_RUN    2
#define AUDIO_STATE_SET    3
#define AUDIO_STATE_STOP   4
#define AUDIO_STATE_ERROR  5

 /*
  * Volume
  * 0xFF: 24 dB
  * 0xCF: 0 dB
  * 0x01: –103 dB
  * 0x00: MUTE
*/

typedef struct {
    uint8_t buf[AUDIO_BUF_MAX_SIZE];
    uint32_t freq;
    uint8_t state;
    volatile int16_t sync;
    uint8_t bit_depth;
    uint8_t volume;
    bool mute;
} Audio;

extern Audio audio;

void audio_init(uint32_t AudioFreq, uint8_t bit_depth);
void audio_play(void);
void audio_stop(void);
void audio_tick(void);
void audio_append(uint8_t* buf, uint16_t buf_len);
void audio_clock_sync(void);
void audio_setVolume(uint8_t volume);
void audio_setMute(bool flag);

int16_t audio_clock_samples_delta(void);

void audio_out1_init(uint32_t AudioFreq, uint8_t bit_depth);
void audio_out1_reset(void);
void audio_out1_check(void);
void audio_out1_append(uint8_t* buf, uint16_t buf_len);
void audio_out1_clock_sync(void);
int16_t audio_out1_clock_delta(void);

void audio_out2_init(uint32_t AudioFreq, uint8_t bit_depth);
void audio_out2_reset(void);
void audio_out2_check(void);
void audio_out2_append(uint8_t* buf, uint16_t buf_len);
void audio_out2_clock_sync(void);
int16_t audio_out2_clock_delta(void);

#ifdef __cplusplus
}
#endif

#endif
