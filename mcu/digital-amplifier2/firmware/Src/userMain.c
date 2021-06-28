#include "base.h"
#include "msp_i2c.h"
#include "msp_iwdg.h"
#include "bsp_ec11.h"
#include "bsp_pcm1792.h"
#include "bsp_tas5805.h"

void task_dev_setter()
{
    uint32_t oldData = 0;

    uint8_t inputMode = 0;
    uint8_t out1_en = 0;
    uint8_t out2_en = 0;
    uint32_t AudioFreq = SAI_AUDIO_FREQUENCY_44K;
    int8_t bit_depth = 16;
    uint8_t vol = 0;

    uint32_t data;
    for (;;) {
        if (xTaskNotifyWait(~0, ~0, &data, 1000)) {

            inputMode = (data>>24) & 0xf;
            out1_en = (data>>20) & 0xf;
            out2_en = (data>>16) & 0xf;

            switch ((data>>12) & 0xf) {
                case 0: AudioFreq = SAI_AUDIO_FREQUENCY_44K; break;
                case 1: AudioFreq = SAI_AUDIO_FREQUENCY_48K; break;
                case 2: AudioFreq = SAI_AUDIO_FREQUENCY_96K; break;
                case 3: AudioFreq = SAI_AUDIO_FREQUENCY_192K; break;
            }

            switch ((data>>8) & 0xf) {
                case 0: bit_depth = 16; break;
                case 1: bit_depth = 24; break;
                case 2: bit_depth = 32; break;
            }

            vol = data & 0xff;

            if (oldData != (data &(~0xf))) {
                if (audio.input_task_hd) {
                    xTaskNotify(audio.input_task_hd, 1, eSetValueWithOverwrite);
                    vTaskDelay(100);
                }

                if (inputMode == 0) {
                    xTaskCreate(task_usb_input, "usb", 1024, NULL, 0, &audio.input_task_hd);
                } else if (inputMode == 1) {
                    xTaskCreate(task_bt_input, "bt", 1024, NULL, 0, &audio.input_task_hd);
                } else if (inputMode == 2) {
                    xTaskCreate(task_spdif_input, "spdif", 1024, NULL, 0, &audio.input_task_hd);
                }

                if (out1_en) {
                    bsp_pcm1792_init();
                    vTaskDelay(100);
                    bsp_pcm1792_play(AudioFreq, bit_depth, vol);
                } else {
                    bsp_pcm1792_deInit();
                }

                if (out2_en) {
                    bsp_tas5805_init();
                    vTaskDelay(100);
                    bsp_tas5805_play(AudioFreq, bit_depth, vol);
                } else {
                    bsp_tas5805_deInit();
                }
            } else {
                if (out1_en) {
                    bsp_pcm1792_volume(vol);
                }
                if (out2_en) {
                    bsp_tas5805_volume(vol);
                }
            }

            oldData = data & (~0xf);
        } else {
            msp_iwdg_feed();
        }
    }
}

void UserMain()
{
    msp_i2c_init();
    msp_iwdg_init();
    bsp_ec11_init();

    xTaskCreate(task_btn_service, "usb", 1024, NULL, 1, &audio.input_task_hd);
    xTaskCreate(task_dev_setter, "dev", 1024, NULL, 0, &audio.out_task_hd);

    xTaskNotify(audio.out_task_hd, 0x0f00, eSetValueWithOverwrite);

    printf("runing\n");
}


