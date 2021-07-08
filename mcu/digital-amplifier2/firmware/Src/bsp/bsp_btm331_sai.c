#include <string.h>
#include <stdio.h>
#include "base.h"
#include "msp_sai_in.h"
#include "bsp_btm331_sai.h"

static void _dma_half_cplt(SAI_HandleTypeDef *hsai);
static void _dma_cplt(SAI_HandleTypeDef *hsai);

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
}

void bsp_btm331_sai_start(void)
{
    msp_sai_in_init(SAI_AUDIO_FREQUENCY_48K, 24);

    HAL_SAI_RegisterCallback(&hsai_in, HAL_SAI_RX_HALFCOMPLETE_CB_ID, _dma_half_cplt);
    HAL_SAI_RegisterCallback(&hsai_in, HAL_SAI_RX_COMPLETE_CB_ID, _dma_cplt);

    HAL_StatusTypeDef ret;
    ret = HAL_SAI_Receive_DMA(&hsai_in, &audio.input_buf[0], AUDIO_INPUT_BUF_SIZE/4);
    if (ret!= HAL_OK) {
        printf("sai in dma err\n");
        return;
    }

    audio_init(SAI_AUDIO_FREQUENCY_48K, 32);
    audio_play();
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
    msp_sai_in_deInit();

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0);
}

static void _dma_half_cplt(SAI_HandleTypeDef *hsai)
{
    audio_append(&audio.input_buf[0], AUDIO_INPUT_BUF_SIZE/2);
    audio_clock_sync();
}

static void _dma_cplt(SAI_HandleTypeDef *hsai)
{
    audio_append(&audio.input_buf[AUDIO_INPUT_BUF_SIZE/2], AUDIO_INPUT_BUF_SIZE/2);
    audio_clock_sync();
}

