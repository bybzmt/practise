#include "base.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_ll_dma.h"

/* 16bit/8*2 * (48000khz/1000)*4 */
#define SPDIF_BUF_SIZE (2*2*48*4)

extern SPDIFRX_HandleTypeDef SpdifrxHandle;

static void my_spdif_cplt(SPDIFRX_HandleTypeDef *hspdif);
static void my_spdif_half_cplt(SPDIFRX_HandleTypeDef *hspdif);
static void my_spdif_listen(void);
static void my_spdif_listen_stop(void);
static void spdif_start_real(void);

static uint8_t spdif_buf[SPDIF_BUF_SIZE];

void spdif_stop(void)
{
    printf("spdif_stop\n");

    audio_stop();

    my_spdif_listen_stop();

    HAL_SPDIFRX_DeInit(&SpdifrxHandle);
}

void spdif_start(void)
{
    my_spdif_listen();
}

static void spdif_start_real(void)
{
    printf("spdif start\n");

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

    audio_init(SAI_AUDIO_FREQUENCY_48K, 16);
}

static void my_spdif_half_cplt(SPDIFRX_HandleTypeDef *hspdif)
{
    audio_append(&spdif_buf[0], SPDIF_BUF_SIZE/2);
    audio_clock_sync();
}

static void my_spdif_cplt(SPDIFRX_HandleTypeDef *hspdif)
{
    audio_append(&spdif_buf[SPDIF_BUF_SIZE/2], SPDIF_BUF_SIZE/2);
    audio_clock_sync();
}

static void my_spdif_listen(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_5;
    GPIO_InitStructure.Mode      = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

static void my_spdif_listen_stop(void)
{
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_5);
}

void EXTI9_5_IRQHandler(void)
{
    my_spdif_listen_stop();
    spdif_start_real();

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}
