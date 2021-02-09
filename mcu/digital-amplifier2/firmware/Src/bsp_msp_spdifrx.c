#include "base.h"

static void my_spdif_MspInit(SPDIFRX_HandleTypeDef *hspdif);
static void my_spdif_MspDeInit(SPDIFRX_HandleTypeDef *hspdif);
static void my_spdif_rx_cplt(SPDIFRX_HandleTypeDef *hspdif);
static void my_spdif_rx_halfcplt(SPDIFRX_HandleTypeDef *hspdif);

SPDIFRX_HandleTypeDef SpdifrxHandle = {
    .Instance = SPDIFRX,
    .Init = {
        .InputSelection = SPDIFRX_INPUT_IN3,
        .Retries = SPDIFRX_MAXRETRIES_NONE,
        .WaitForActivity = SPDIFRX_WAITFORACTIVITY_ON,
        .ChannelSelection = SPDIFRX_CHANNEL_A,
        .DataFormat = SPDIFRX_DATAFORMAT_LSB,
        .StereoMode = SPDIFRX_STEREOMODE_ENABLE,
        .PreambleTypeMask = SPDIFRX_PREAMBLETYPEMASK_ON,
        .ChannelStatusMask = SPDIFRX_CHANNELSTATUS_ON,
        .ValidityBitMask = SPDIFRX_VALIDITYMASK_OFF,
        .ParityErrorMask = SPDIFRX_PARITYERRORMASK_OFF,
    },
    .MspInitCallback = my_spdif_MspInit,
    .MspDeInitCallback = my_spdif_MspDeInit,
};

DMA_HandleTypeDef SpdifrxDmaHandle = {
    .Instance = DMA1_Stream1;
    .Init = {
        .Request             = DMA_REQUEST_SPDIF_RX_DT;
        .Direction           = DMA_PERIPH_TO_MEMORY;
        .PeriphInc           = DMA_PINC_DISABLE;
        .MemInc              = DMA_MINC_ENABLE;
        .PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        .MemDataAlignment    = DMA_PDATAALIGN_WORD;
        .Mode                = DMA_CIRCULAR;
        .Priority            = DMA_PRIORITY_HIGH;
        .FIFOMode            = DMA_FIFOMODE_DISABLE;
        .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
        .MemBurst            = DMA_MBURST_SINGLE;
        .PeriphBurst         = DMA_MBURST_SINGLE;
    },
};

uint8_t my_spdifrx_stop(uint32_t *buf, size_t size)
{
    HAL_SPDIFRX_DeInit(&SpdifrxHandle);
}

uint8_t my_spdifrx_start()
{
    size_t size = 1024 * 8;
    uint8_t *buf = malloc(size);

    HAL_SPDIFRX_DeInit(&SpdifrxHandle);

    if (HAL_SPDIFRX_Init(&SpdifrxHandle) != HAL_OK)
    {
        printf("spdifrx init err\n");
        return 1;
    }

    HAL_SPDIFRX_ReceiveDataFlow_DMA(&SpdifrxHandle, buf, size);

    if (SpdifrxHandle.ErrorCode != HAL_SPDIFRX_ERROR_NONE)
    {
        printf("spdifrx start err\n");
        return 1;
    }

    return 0;
}


static void my_spdif_MspInit(SPDIFRX_HandleTypeDef *hspdif)
{
    hspdif->RxHalfCpltCallback = my_spdif_rx_halfcplt;
    hspdif->RxCpltCallback = my_spdif_rx_cplt;

    __HAL_RCC_SPDIFRX_CLK_ENABLE();

    /* Configure SPDIFRX_IN pin */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_5;
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF8_SPDIF;
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
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn,   6, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

static void my_spdif_MspDeInit(SPDIFRX_HandleTypeDef *hspdif)
{
}

static void my_spdif_rx_cplt(SPDIFRX_HandleTypeDef *hspdif)
{
}

static void my_spdif_rx_halfcplt(SPDIFRX_HandleTypeDef *hspdif)
{
}

void spdifClock_Config(void)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1|RCC_PERIPHCLK_SPDIFRX;
    PeriphClkInitStruct.PLLSAI.PLLSAIM = 5;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 96;
    PeriphClkInitStruct.PLLSAI.PLLSAIQ = 5;
    PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
    PeriphClkInitStruct.PLLSAIDivQ = 5;
    PeriphClkInitStruct.SpdifClockSelection = RCC_SPDIFRXCLKSOURCE_PLLR;
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}
