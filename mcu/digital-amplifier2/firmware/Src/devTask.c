#include "base.h"
#include "msp_iwdg.h"
#include "bsp_pcm1792.h"
#include "bsp_tas5805.h"

void task_dev_setter()
{
    printf("task dev running\n");

    uint8_t old_mode = 255;
    bool old_headphone_on = 0;
    bool old_speakers_on = 0;
    uint32_t old_audioFreq = 0;
    uint8_t old_bit_depth = 0;
    uint8_t close_tick1 = 0;
    uint8_t close_tick2 = 0;
    uint8_t old_vol = 255;

    uint8_t input_mode = 0;
    bool headphone_on = 0;
    bool speakers_on = 0;
    uint32_t audioFreq = SAI_AUDIO_FREQUENCY_44K;
    uint8_t bit_depth = 16;
    uint8_t vol = 0;

    uint32_t data;
    for (;;) {
        if (xTaskNotifyWait(0, ~0ul, &data, 1000)) {
            input_mode = (data>>24) & 0xf;
            headphone_on = (data>>20) & 0xf;
            speakers_on = (data>>16) & 0xf;

            switch ((data>>12) & 0xf) {
                case 0: audioFreq = SAI_AUDIO_FREQUENCY_44K; break;
                case 1: audioFreq = SAI_AUDIO_FREQUENCY_48K; break;
                case 2: audioFreq = SAI_AUDIO_FREQUENCY_96K; break;
                case 3: audioFreq = SAI_AUDIO_FREQUENCY_192K; break;
            }

            switch ((data>>8) & 0xf) {
                case 0: bit_depth = 16; break;
                case 1: bit_depth = 24; break;
                case 2: bit_depth = 32; break;
            }

            vol = data & 0xff;

            if (old_mode != input_mode) {
                while (audio.input_task_hd != NULL) {
                    xTaskNotify(audio.input_task_hd, 1, eSetValueWithOverwrite);
                    vTaskDelay(100);
                }

                if (input_mode == 0) {
                    xTaskCreate(task_usb_input, "usb", 1024, NULL, 1, &audio.input_task_hd);
                } else if (input_mode == 1) {
                    xTaskCreate(task_bt_input, "bt", 1024, NULL, 1, &audio.input_task_hd);
                } else if (input_mode == 2) {
                    xTaskCreate(task_spdif_input, "spdif", 1024, NULL, 1, &audio.input_task_hd);
                }

                old_mode = input_mode;
            }

            if (!old_headphone_on && headphone_on) {
                bsp_pcm1792_init();
                bsp_pcm1792_play(audioFreq, bit_depth, vol);

                old_headphone_on = true;
                close_tick1 = 0;
            } else if (old_headphone_on && headphone_on) {
                if (old_audioFreq != audioFreq || old_bit_depth != bit_depth) {
                    bsp_pcm1792_play(audioFreq, bit_depth, vol);
                } else {
                    bsp_pcm1792_volume(vol);
                }
            } else if (old_headphone_on && !headphone_on) {
                bsp_pcm1792_stop();

                old_headphone_on = false;
                close_tick1 = 1;
            }

            if (!old_speakers_on && speakers_on) {
                bsp_tas5805_init();
                bsp_tas5805_play(audioFreq, bit_depth, vol);

                old_speakers_on = true;
                close_tick2 = 0;
            } else if (old_speakers_on && speakers_on) {
                if (old_audioFreq != audioFreq || old_bit_depth != bit_depth) {
                    bsp_tas5805_play(audioFreq, bit_depth, vol);
                } else {
                    bsp_tas5805_volume(vol);
                }
            } else if (old_speakers_on && !speakers_on) {
                bsp_tas5805_stop();

                old_speakers_on = false;
                close_tick2 = 1;
            }

            if ( old_audioFreq != audioFreq ||
                    old_bit_depth != bit_depth ||
                    old_vol != vol
                    ) {
                btn_oled_refresh();
            }

            old_audioFreq = audioFreq;
            old_bit_depth = bit_depth;
            old_vol = vol;
        } else {
            if (close_tick1 > 0) {
                close_tick1++;
                if (close_tick1 > 5) {
                    bsp_pcm1792_deInit();
                    close_tick1 = 0;
                }
            }

            if (close_tick2 > 0) {
                close_tick2++;
                if (close_tick2 > 5) {
                    bsp_tas5805_deInit();
                    close_tick2 = 0;
                }
            }
        }
    }
}




