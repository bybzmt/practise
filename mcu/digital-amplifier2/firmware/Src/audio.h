#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "base.h"
#include <string.h>
#include <stdio.h>

#define AUDIO_BUF_SAMPLE_NUM 1000
#define AUDIO_SAMPLE_SIZE (4*2)
#define AUDIO_OUT_BUF_SIZE (AUDIO_SAMPLE_SIZE * AUDIO_BUF_SAMPLE_NUM)
#define AUDIO_INPUT_BUF_SIZE (AUDIO_SAMPLE_SIZE * 200)

#define AUDIO_STATE_INIT   0
#define AUDIO_STATE_RUN    1
#define AUDIO_STATE_SYNC   2
#define AUDIO_STATE_ERROR  3

#define VOLUME_0dB 0xCF

 /*
  * Volume: 0.5dB/step
  * 0xFF: +24 dB
  * 0xCF: 0 dB
  * 0x01: –103 dB
  * 0x00: MUTE
*/
typedef uint8_t volume_t;

typedef struct {
    uint8_t input_buf[AUDIO_INPUT_BUF_SIZE];
    uint8_t out_buf[AUDIO_OUT_BUF_SIZE];

    uint8_t input_mode;
    uint32_t freq;
    uint8_t bit_depth;
    volume_t vol;
    bool mute;

    uint8_t set_out1;
    uint8_t set_out2;

    uint16_t w_idx;
    uint8_t state;
    uint8_t sample_size;

    bool out_dev_en;
    bool zero_off;
    uint8_t zero_data;

    TaskHandle_t input_task_hd;
    TaskHandle_t out_task_hd;
} Audio;

extern Audio audio;

void audio_init(uint32_t AudioFreq, uint8_t bit_depth);
void audio_play(void);
void audio_stop(void);
void audio_append(uint8_t* buf, uint16_t buf_len);
void audio_setVolume(volume_t vol);
void audio_setMute(bool flag);
void audio_copy(uint16_t idx, uint8_t* buf, uint16_t sample_num);
void audio_clock_sync(void);
int16_t audio_clock_delta(void);

void audio_notify_dev(void);

#ifdef __cplusplus
}
#endif

#endif
