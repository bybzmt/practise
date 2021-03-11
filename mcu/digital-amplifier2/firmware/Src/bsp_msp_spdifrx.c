#include "base.h"

static void bsp_spdif_clock_config(void);
static void my_spdif_MspInit(SPDIFRX_HandleTypeDef *hspdif);
static void my_spdif_MspDeInit(SPDIFRX_HandleTypeDef *hspdif);

SPDIFRX_HandleTypeDef SpdifrxHandle = {
    .Instance = SPDIFRX,
    .Init = {
        .InputSelection = SPDIFRX_INPUT_IN3,
        .Retries = SPDIFRX_MAXRETRIES_NONE,
        /* .Retries = SPDIFRX_MAXRETRIES_15, */
        /* .WaitForActivity = SPDIFRX_WAITFORACTIVITY_ON, */
        .WaitForActivity = SPDIFRX_WAITFORACTIVITY_OFF,
        .ChannelSelection = SPDIFRX_CHANNEL_A,
        /* .DataFormat = SPDIFRX_DATAFORMAT_LSB, */
        .DataFormat = SPDIFRX_DATAFORMAT_32BITS,
        .StereoMode = SPDIFRX_STEREOMODE_ENABLE,
        .PreambleTypeMask = SPDIFRX_PREAMBLETYPEMASK_OFF,
        .ChannelStatusMask = SPDIFRX_CHANNELSTATUS_OFF,
        .ValidityBitMask = SPDIFRX_VALIDITYMASK_OFF,
        .ParityErrorMask = SPDIFRX_PARITYERRORMASK_OFF,
    },
    .MspInitCallback = my_spdif_MspInit,
    .MspDeInitCallback = my_spdif_MspDeInit,
};

DMA_HandleTypeDef SpdifrxDmaHandle = {
    .Instance = DMA1_Stream1,
    .Init = {
        .Channel             = DMA_CHANNEL_0,
        .Direction           = DMA_PERIPH_TO_MEMORY,
        .PeriphInc           = DMA_PINC_DISABLE,
        .MemInc              = DMA_MINC_ENABLE,
        .PeriphDataAlignment = DMA_PDATAALIGN_WORD,
        .MemDataAlignment    = DMA_PDATAALIGN_WORD,
        .Mode                = DMA_CIRCULAR,
        .Priority            = DMA_PRIORITY_HIGH,
        .FIFOMode            = DMA_FIFOMODE_DISABLE,
        .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
        .MemBurst            = DMA_MBURST_SINGLE,
        .PeriphBurst         = DMA_MBURST_SINGLE,
    },
};

static void bsp_spdif_clock_config(void)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    HAL_RCCEx_GetPeriphCLKConfig(&PeriphClkInitStruct);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPDIFRX;
    PeriphClkInitStruct.SpdifClockSelection = RCC_SPDIFRXCLKSOURCE_PLLR;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        printf("spdif clock error\n");
    }
}

static void my_spdif_MspInit(SPDIFRX_HandleTypeDef *hspdif)
{
    printf("spdifrx init\n");

    bsp_spdif_clock_config();

    __HAL_RCC_SPDIFRX_CLK_ENABLE();

    /* Configure SPDIFRX_IN pin */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_5;
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStructure.Alternate = GPIO_AF8_SPDIFRX;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Enable DMA1 clock */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Associate the initialized hdma_rx handle to the the hspdif handle*/
    __HAL_LINKDMA(hspdif, hdmaDrRx, SpdifrxDmaHandle);

    /* Configure the DMA Stream */
    if (HAL_OK != HAL_DMA_Init(&SpdifrxDmaHandle))
    {
        printf("spdifrx dma err\n");
    }

    /* NVIC configuration for DMA transfer complete interrupt (SPDIFRX) */
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn,   0, 0);
    HAL_NVIC_SetPriority(SPDIF_RX_IRQn,   0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
    HAL_NVIC_EnableIRQ(SPDIF_RX_IRQn);
}

static void my_spdif_MspDeInit(SPDIFRX_HandleTypeDef *hspdif)
{
    printf("spdifrx deInit\n");

    __HAL_RCC_SPDIFRX_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(DMA1_Stream1_IRQn);
    HAL_NVIC_DisableIRQ(SPDIF_RX_IRQn);
}

void DMA1_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&SpdifrxDmaHandle);
}

void SPDIF_RX_IRQHandler(void)
{
    HAL_SPDIFRX_IRQHandler(&SpdifrxHandle);
}
