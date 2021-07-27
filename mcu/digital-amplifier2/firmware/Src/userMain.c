#include "base.h"
#include "msp_i2c.h"
#include "msp_iwdg.h"

void bkp_sram_init(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_BKPSRAM_CLK_ENABLE();
	HAL_PWREx_EnableBkUpReg();
}

void UserMain()
{
    bkp_sram_init();
    msp_iwdg_init();
    msp_i2c_init();

    if (!settings._saved) {
        settings._saved = 1;
        settings.input_mode = 0;
        settings.vol = VOLUME_0dB - 40;
        settings.mute = 0;
        settings.headphone_on = 1;
        settings.speakers_on = 1;
        settings.auto_switch = 1;
        settings.auto_off = 1;
    }

    xTaskCreate(task_dev_setter, "dev", 1024, NULL, 0, &audio.out_task_hd);

    xTaskNotify(audio.out_task_hd, 0x0f00, eSetValueWithOverwrite);

    vTaskDelay(1);

    xTaskCreate(task_btn_service, "btn", 1024, NULL, 2, NULL);

    printf("runing\n");

    for (;;) {
        vTaskDelay(2000);
        msp_iwdg_feed();
    }
}


