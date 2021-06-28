#include "base.h"
#include <string.h>
#include <stdio.h>
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "bsp_btm331_sai.h"
#include "bsp_spdifrx.h"

void task_bt_input()
{
    bsp_btm331_init();
    bsp_btm331_sai_start();

    uint32_t interrupt;

    for (;;) {
        if (xTaskNotifyWait(0, ~0ul, &interrupt, 10) == pdTRUE) {
            bsp_btm331_deInit();
            break;
        }
    }

    vTaskDelete(NULL);
}

void task_spdif_input()
{
    bsp_spdifrx_init();

    uint32_t interrupt;

    bool waiting = true;

    for (;;) {
        if (xTaskNotifyWait(0, ~0ul, &interrupt, 10) == pdTRUE) {
            bsp_spdifrx_stop();
            break;
        }

        if (waiting) {
            if (bsp_spdifrx_wait_signal()) {
                bsp_spdifrx_start();
                waiting = false;
            }
        }
    }

    vTaskDelete(NULL);
}

void task_usb_input()
{
    static USBD_HandleTypeDef USBD_Device;
    USBD_Init(&USBD_Device, &AUDIO_Desc, 0);
    USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);
    USBD_Start(&USBD_Device);

    uint32_t interrupt;

    for (;;) {
        if (xTaskNotifyWait(0, ~0ul, &interrupt, 10) == pdTRUE) {
            USBD_Stop(&USBD_Device);
            USBD_DeInit(&USBD_Device);
            break;
        }
    }

    vTaskDelete(NULL);
}
