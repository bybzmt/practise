#include "base.h"


IWDG_HandleTypeDef IwdgHandle;

USBD_HandleTypeDef USBD_Device;

void usb_init(void)
{
    USBD_Init(&USBD_Device, &AUDIO_Desc, 0);
    USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);
}

void usb_start(void)
{
    usb_init();
    USBD_Start(&USBD_Device);
}

void usb_stop(void)
{
    USBD_Stop(&USBD_Device);
    USBD_DeInit(&USBD_Device);
}

static uint8_t volatile device_mode;
static uint8_t volatile device_mode_new;

void button_click()
{
    device_mode_new = 1;
}

void device_mode_change(uint8_t mode)
{
    switch (device_mode) {
        case MODE_SPDIF: spdif_stop(); break;
        case MODE_BT: btm331_stop(); break;
        case MODE_USB: usb_stop(); break;
    }

    device_mode = mode;

    switch (device_mode) {
        case MODE_SPDIF: spdif_start(); break;
        case MODE_BT: btm331_start(); break;
        case MODE_USB: usb_start(); break;
    }
}

void UserMain()
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_StatusTypeDef flag = HAL_I2C_Init(&hi2c1);
    if (flag != HAL_OK) {
        printf("i2c init err\n");
    }

    IwdgHandle.Instance = IWDG;
    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_128;
    IwdgHandle.Init.Reload = 40000/128*5;
    HAL_IWDG_Init(&IwdgHandle);

    audio.volume = 0xcf - 10;
    audio.mute = false;
    audio_init(SAI_AUDIO_FREQUENCY_44K, 16);

    btm331_init();
    button_init();

    device_mode_change(MODE_USB);

    printf("run\n");

    for (;;) {
        vTaskDelay(100);

        audio_tick();

        if (device_mode_new != 0) {
            if (device_mode == MODE_BT) {
                device_mode_change(MODE_USB);
                led_on(1);
            } else {
                device_mode_change(MODE_BT);
                led_on(0);
            }

            device_mode_new = 0;
        }

        HAL_IWDG_Refresh(&IwdgHandle);
    }
}

