#include "base.h"

USBD_HandleTypeDef USBD_Device;

static void gpio_init()
{


    GPIO_InitTypeDef  gpio_init_structure;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;

    //usb上拉电阻
    gpio_init_structure.Pin =  GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &gpio_init_structure);

    //关
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1);
    HAL_Delay(100);
}

void usb_to_sai_init(void)
{
    gpio_init();

    USBD_Init(&USBD_Device, &AUDIO_Desc, 0);

    USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);

    USBD_AUDIO_RegisterInterface(&USBD_Device, &USBD_AUDIO_fops);

    USBD_Start(&USBD_Device);

    //usb上拉电阻
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 0);
}

void UserMain()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    usb_to_sai_init();
    /* BSP_AUDIO_Record(); */

    printf("run!\n");

    /* int num = 0; */
    while (1) {
        /* printf("run:%d\n", num++); */
        vTaskDelay(1000);
    }
}

