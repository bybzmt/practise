#include "base.h"
#include "msp_spdifrx.h"
#include "msp_clock.h"

static void msp_spdifrx_mspInit(SPDIFRX_HandleTypeDef *hspdif);
static void msp_spdifrx_mspDeInit(SPDIFRX_HandleTypeDef *hspdif);
static void msp_dma1_stream1_irq(void);
static void msp_spdif_rx_irq(void);

SPDIFRX_HandleTypeDef SpdifrxHandle = {
    .Instance = SPDIFRX,
    .Init = {
        .InputSelection = SPDIFRX_INPUT_IN3,
        .Retries = SPDIFRX_MAXRETRIES_NONE,
        .WaitForActivity = SPDIFRX_WAITFORACTIVITY_OFF,
        .ChannelSelection = SPDIFRX_CHANNEL_A,
        .DataFormat = SPDIFRX_DATAFORMAT_32BITS,
        .StereoMode = SPDIFRX_STEREOMODE_ENABLE,
        .PreambleTypeMask = SPDIFRX_PREAMBLETYPEMASK_OFF,
        .ChannelStatusMask = SPDIFRX_CHANNELSTATUS_OFF,
        .ValidityBitMask = SPDIFRX_VALIDITYMASK_OFF,
        .ParityErrorMask = SPDIFRX_PARITYERRORMASK_OFF,
    },
    .MspInitCallback = msp_spdifrx_mspInit,
    .MspDeInitCallback = msp_spdifrx_mspDeInit,
};

static DMA_HandleTypeDef SpdifrxDmaHandle = {
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


static void msp_spdifrx_mspInit(SPDIFRX_HandleTypeDef *hspdif)
{
    printf("spdifrx init\n");

    msp_clock_spdif_config();

    __HAL_RCC_SPDIFRX_CLK_ENABLE();

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
    NVIC_SetVector(DMA1_Stream1_IRQn, (uint32_t)&msp_dma1_stream1_irq);
    NVIC_SetVector(SPDIF_RX_IRQn, (uint32_t)&msp_spdif_rx_irq);
    NVIC_EnableIRQ(DMA1_Stream1_IRQn);
    NVIC_EnableIRQ(SPDIF_RX_IRQn);
}

static void msp_spdifrx_mspDeInit(SPDIFRX_HandleTypeDef *hspdif)
{
    printf("spdifrx deInit\n");

    __HAL_RCC_SPDIFRX_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(DMA1_Stream1_IRQn);
    HAL_NVIC_DisableIRQ(SPDIF_RX_IRQn);
}

static void msp_dma1_stream1_irq(void)
{
    HAL_DMA_IRQHandler(&SpdifrxDmaHandle);
}

static void msp_spdif_rx_irq(void)
{
    HAL_SPDIFRX_IRQHandler(&SpdifrxHandle);
}
