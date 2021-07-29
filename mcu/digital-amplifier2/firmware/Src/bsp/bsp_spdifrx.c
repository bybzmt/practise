#include "base.h"
#include "bsp_spdifrx.h"

static void my_spdif_cplt(SPDIFRX_HandleTypeDef *hspdif);
static void my_spdif_half_cplt(SPDIFRX_HandleTypeDef *hspdif);

void bsp_spdifrx_init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_5;
    GPIO_InitStructure.Mode      = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
}

bool bsp_spdifrx_wait_signal(void)
{
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5);
}

void bsp_spdifrx_stop(void)
{
    audio_stop();

    HAL_SPDIFRX_DMAStop(&SpdifrxHandle);
    HAL_SPDIFRX_DeInit(&SpdifrxHandle);
}

void bsp_spdifrx_start(void)
{
    printf("spdif start\n");

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_5);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_5;
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStructure.Alternate = GPIO_AF8_SPDIFRX,
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    SpdifrxHandle.Init.InputSelection = SPDIFRX_INPUT_IN3;

    bsp_spdifrx_real_start();
}

void bsp_spdifrx_real_start(void)
{
    if (HAL_SPDIFRX_Init(&SpdifrxHandle) != HAL_OK)
    {
        printf("spdif init err\n");
        return;
    }

    HAL_SPDIFRX_RegisterCallback(&SpdifrxHandle, HAL_SPDIFRX_RX_HALF_CB_ID, my_spdif_half_cplt);
    HAL_SPDIFRX_RegisterCallback(&SpdifrxHandle, HAL_SPDIFRX_RX_CPLT_CB_ID, my_spdif_cplt);

    audio_init(SAI_AUDIO_FREQUENCY_48K, 16);

    HAL_StatusTypeDef ret;
    ret = HAL_SPDIFRX_ReceiveDataFlow_DMA(&SpdifrxHandle, (uint32_t*)(&audio.input_buf[0]), AUDIO_INPUT_BUF_SIZE/4);
    if (ret != 0 ) {
         printf("ret: %d\n", ret);
    }

    audio_play();
}


static void my_spdif_half_cplt(SPDIFRX_HandleTypeDef *hspdif)
{
    audio_append(&audio.input_buf[0], AUDIO_INPUT_BUF_SIZE/2);
    audio_clock_sync();
}

static void my_spdif_cplt(SPDIFRX_HandleTypeDef *hspdif)
{
    audio_append(&audio.input_buf[AUDIO_INPUT_BUF_SIZE/2], AUDIO_INPUT_BUF_SIZE/2);
    audio_clock_sync();
}

