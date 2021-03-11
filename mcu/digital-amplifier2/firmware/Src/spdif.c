#include "base.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_ll_dma.h"

/* 16bit/8*2 * (48000khz/1000)*4 */
#define SPDIF_BUF_SIZE (2*2*48*4)

extern SPDIFRX_HandleTypeDef SpdifrxHandle;

static void my_spdif_cplt(SPDIFRX_HandleTypeDef *hspdif);
static void my_spdif_half_cplt(SPDIFRX_HandleTypeDef *hspdif);

static uint8_t spdif_state = 0;

static uint8_t spdif_buf[SPDIF_BUF_SIZE];

void spdif_stop(void)
{
    printf("spdif_stop\n");

    audio_deInit();
    HAL_SPDIFRX_DeInit(&SpdifrxHandle);
}

void spdif_start(void)
{
    printf("spdif start\n");

    spdif_state = 0;

    if (HAL_SPDIFRX_Init(&SpdifrxHandle) != HAL_OK)
    {
        printf("spdif init err\n");
        return;
    }

    HAL_SPDIFRX_RegisterCallback(&SpdifrxHandle, HAL_SPDIFRX_RX_HALF_CB_ID, my_spdif_half_cplt);
    HAL_SPDIFRX_RegisterCallback(&SpdifrxHandle, HAL_SPDIFRX_RX_CPLT_CB_ID, my_spdif_cplt);

    HAL_StatusTypeDef ret;
    ret = HAL_SPDIFRX_ReceiveDataFlow_DMA(&SpdifrxHandle, (uint32_t*)(&spdif_buf[0]), SPDIF_BUF_SIZE/4);
    if (ret != 0 ) {
         printf("ret: %d\n", ret);
    }
}

static void my_spdif_half_cplt(SPDIFRX_HandleTypeDef *hspdif)
{
    if (spdif_state == 0) {
        audio_init(SAI_AUDIO_FREQUENCY_48K, 16);
    }
    spdif_state = 1;

    audio_append_adapt(&spdif_buf[0], SPDIF_BUF_SIZE/2);
}

static void my_spdif_cplt(SPDIFRX_HandleTypeDef *hspdif)
{
    spdif_state = 1;
    audio_append_adapt(&spdif_buf[SPDIF_BUF_SIZE/2], SPDIF_BUF_SIZE/2);
}

void spdif_check(void)
{
    if (spdif_state == 2) {
        spdif_stop();
        spdif_start();
    }

    spdif_state = 2;
}
