#include "base.h"
#include "msp_i2c.h"
#include "msp_iwdg.h"

void UserMain()
{
    msp_iwdg_init();
    msp_i2c_init();

    audio.vol = VOLUME_0dB - 40;
    audio.set_out1 = 0;
    audio.set_out2 = 0;
    audio.set_auto_switch = 1;
    audio.set_auto_off = 1;

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


