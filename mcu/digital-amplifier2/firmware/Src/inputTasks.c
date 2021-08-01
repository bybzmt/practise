#include "base.h"
#include <string.h>
#include <stdio.h>
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "bsp_btm331_sai.h"
#include "bsp_btm331_spdif.h"
#include "bsp_spdifrx.h"

static void task_init(void)
{
    static bool a=0;
    if (a) {
        return;
    }
    a=1;

    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin       = GPIO_PIN_0 | GPIO_PIN_1;
    gpio_init.Mode      = GPIO_MODE_OUTPUT_OD;
    gpio_init.Pull      = GPIO_NOPULL;
    gpio_init.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &gpio_init);
}

void task_bt_input()
{
    printf("bt runing\n");

    task_init();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);

    bsp_btm331_spdif_init();

    uint32_t interrupt;
    bool waiting = true;

    for (;;) {
        if (xTaskNotifyWait(0, ~0ul, &interrupt, 10) == pdTRUE) {
            bsp_btm331_spdif_deInit();
            break;
        }

        if (waiting) {
            if (bsp_btm331_spdif_wait_signal()) {
                bsp_btm331_spdif_start();
                waiting = false;
            }
        } else {
            vTaskDelay(500);

            if (audio.state == AUDIO_STATE_INIT) {
                bsp_spdifrx_stop();
                vTaskDelay(10);
                bsp_btm331_spdif_init();
                waiting = true;
            }
        }

        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
    }

    audio.input_task_hd = NULL;
    vTaskDelete(NULL);
}

void task_spdif_input()
{
    printf("spdif runing\n");

    task_init();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);

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
        } else {
            vTaskDelay(500);

            if (audio.state == AUDIO_STATE_INIT) {
                bsp_spdifrx_stop();
                vTaskDelay(10);
                bsp_spdifrx_init();
                waiting = true;
            }
        }

        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
    }

    audio.input_task_hd = NULL;
    vTaskDelete(NULL);
}

void task_usb_input()
{
    printf("usb runing\n");

    task_init();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);

    settings.bak_vol = settings.vol;

    static USBD_HandleTypeDef USBD_Device;
    USBD_Init(&USBD_Device, &AUDIO_Desc, 0);
    USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);
    USBD_Start(&USBD_Device);

    uint32_t interrupt;

    for (;;) {
        if (xTaskNotifyWait(0, ~0ul, &interrupt, 500) == pdTRUE) {
            USBD_Stop(&USBD_Device);
            USBD_DeInit(&USBD_Device);
            break;
        }

        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

        vTaskDelay(10);
    }

    settings.vol = settings.bak_vol;

    audio.input_task_hd = NULL;
    vTaskDelete(NULL);
}
