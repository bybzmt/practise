#include "base.h"

USBD_HandleTypeDef USBD_Device;

void usb_init(void)
{
    USBD_Init(&USBD_Device, &AUDIO_Desc, 0);

    USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);

    USBD_AUDIO_RegisterInterface(&USBD_Device, &USBD_AUDIO_fops);
}

void usb_start(void)
{
    RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
    HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
    RCC_ExCLKInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLSAIP;
    RCC_ExCLKInitStruct.PLLSAI.PLLSAIM = 8;
    RCC_ExCLKInitStruct.PLLSAI.PLLSAIN = 384;
    RCC_ExCLKInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
    HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);

    bsp_tas6424_init();

    USBD_Start(&USBD_Device);
}

void usb_stop(void)
{
    printf("sub stop\n");

    bsp_tas6424_deInit();

    USBD_Stop(&USBD_Device);
}

static bool usb_runing = false;
static bool spdif_running = false;

void UserMain()
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    HAL_StatusTypeDef flag = HAL_I2C_Init(&hi2c1);
    if (flag != HAL_OK) {
        printf("i2c init err\n");
    }

    usb_init();

    for (;;) {
        if (my_spdif_has()) {
            if (usb_runing) {
                usb_stop();
                usb_runing = false;
            }

            if (!spdif_running) {
                my_spdif_start();
                spdif_running = true;
            }
        } else {
            if (spdif_running) {
                my_spdif_stop();
                spdif_running = false;
            }

            if (!usb_runing) {
                usb_start();
                usb_runing = true;
            }
        }

        vTaskDelay(1000);
    }

    printf("runing.\n");
}

