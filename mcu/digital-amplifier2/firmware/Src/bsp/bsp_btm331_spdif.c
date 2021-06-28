#include <string.h>
#include <stdio.h>
#include "base.h"
#include "bsp_spdifrx.h"
#include "bsp_btm331_spdif.h"

void bsp_btm331_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef  gpio_init = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW,
    };

    //button: en/play/next
    gpio_init.Pin = GPIO_PIN_12 | GPIO_PIN_11 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOC, &gpio_init);
    //button: prev
    gpio_init.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOB, &gpio_init);

    //state: Connected
    gpio_init.Mode = GPIO_MODE_INPUT,
    gpio_init.Pull = GPIO_NOPULL,
    gpio_init.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOA, &gpio_init);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 1);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_7;
    GPIO_InitStructure.Mode      = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
}

bool bsp_btm331_wait_signal(void)
{
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);
}

void bsp_btm331_spdif_start(void)
{
    /* Configure SPDIFRX_IN pin */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_7;
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStructure.Alternate = GPIO_AF7_SPDIFRX;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    SpdifrxHandle.Init.InputSelection = SPDIFRX_INPUT_IN1;
    bsp_spdifrx_start();
}

void bsp_btm331_reset(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1);
    //长按3秒进入配对
    vTaskDelay(3500);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 0);
}

void bsp_btm331_deInit(void)
{
    bsp_spdif_stop();

    HAL_GPIO_deInit(GPIOC, GPIO_PIN_7);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0);
}

