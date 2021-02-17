#include "base.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_ll_dma.h"

#define SPDIF_BUF_SIZE 1024

extern SPDIFRX_HandleTypeDef SpdifrxHandle;

static void my_spdif_cplt(SPDIFRX_HandleTypeDef *hspdif);
static void my_spdif_half_cplt(SPDIFRX_HandleTypeDef *hspdif);

static bool spdif_sync = false;

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

    spdif_sync = false;

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
    if (spdif_sync == false) {
        spdif_sync = true;
        audio_init(SAI_AUDIO_FREQUENCY_48K, 16);
    }

    audio_append(&spdif_buf[0], SPDIF_BUF_SIZE/2);
}

static void my_spdif_cplt(SPDIFRX_HandleTypeDef *hspdif)
{
    audio_append(&spdif_buf[SPDIF_BUF_SIZE/2], SPDIF_BUF_SIZE/2);
}
