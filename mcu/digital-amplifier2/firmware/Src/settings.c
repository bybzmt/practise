#include "base.h"
#include "settings.h"
#include "audio.h"
#include "fnv.h"

__attribute__((section(".BKP_SRAM"))) settings_t settings={0};

void audio_setVolume(volume_t vol)
{
    settings.vol = vol;
    settings_fnv_set();
    audio_notify_dev();
}

void audio_setMute(bool flag)
{
    settings.mute = flag;
    settings_fnv_set();
    audio_notify_dev();
}

void settings_fnv_set(void)
{
	Fnv32_t hash_val;

    settings._fnv1a = 0;
	hash_val = fnv_32a_buf(&settings, sizeof(settings), FNV1_32_INIT);

    settings._fnv1a = hash_val;
}

void settings_fnv_check(void)
{
    if (settings._fnv1a) {
        uint32_t old = settings._fnv1a;

        Fnv32_t hash_val;

        settings._fnv1a = 0;

        hash_val = fnv_32a_buf(&settings, sizeof(settings), FNV1_32_INIT);

        if (old == hash_val) {
            settings._fnv1a = old;
        }
    }

    if (settings._fnv1a == 0) {
        settings.input_mode = 0;
        settings.vol = VOLUME_0dB - 40;
        settings.bak_vol = VOLUME_0dB - 40;
        settings.mute = 0;
        settings.headphone_on = 1;
        settings.speakers_on = 1;
        settings.auto_switch = 1;
        settings.auto_off = 1;
    }
}

void bkp_sram_init(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_BKPSRAM_CLK_ENABLE();
	HAL_PWREx_EnableBkUpReg();

    vTaskDelay(1);
}

void settings_init(void)
{
    bkp_sram_init();
    settings_fnv_check();
}
