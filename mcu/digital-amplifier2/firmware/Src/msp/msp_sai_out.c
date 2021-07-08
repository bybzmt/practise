#include <stdint.h>
#include "base.h"
#include "msp_sai_out.h"
#include "msp_clock.h"
#include "stm32f4xx_ll_dma.h"

static void msp_sai_dma_irq(void);
static void msp_sai_mspDeInit(SAI_HandleTypeDef *hsai);
static void msp_sai_mspInit(SAI_HandleTypeDef *hsai);

SAI_HandleTypeDef hsai_out = {
    .Instance = SAI1_Block_B,
    .Init = {
        .ClockSource   = SAI_CLKSOURCE_PLLI2S,
        .AudioMode     = SAI_MODEMASTER_TX,
        .NoDivider     = SAI_MASTERDIVIDER_ENABLE,
        .Protocol      = SAI_FREE_PROTOCOL,
        .DataSize      = SAI_DATASIZE_24,
        .FirstBit      = SAI_FIRSTBIT_MSB,
        .ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE,
        .Synchro       = SAI_ASYNCHRONOUS,
        .OutputDrive   = SAI_OUTPUTDRIVE_ENABLE,
        .FIFOThreshold = SAI_FIFOTHRESHOLD_1QF,
    },
    .FrameInit = {
        .FrameLength       = 64,
        .ActiveFrameLength = 32,
        .FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION,
        .FSPolarity        = SAI_FS_ACTIVE_LOW,
        .FSOffset          = SAI_FS_FIRSTBIT,
    },
    .SlotInit = {
        .FirstBitOffset = 1,
        .SlotSize       = SAI_SLOTSIZE_32B,
        .SlotNumber     = 2,
        .SlotActive     = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1,
    },
    .MspInitCallback    = msp_sai_mspInit,
    .MspDeInitCallback  = msp_sai_mspDeInit,
};

static DMA_HandleTypeDef hdma_tx = {
    .Instance = DMA2_Stream5,
    .Init = {
        .Channel             = DMA_CHANNEL_0,
        .Direction           = DMA_MEMORY_TO_PERIPH,
        .PeriphInc           = DMA_PINC_DISABLE,
        .MemInc              = DMA_MINC_ENABLE,
        .PeriphDataAlignment = DMA_PDATAALIGN_WORD,
        .MemDataAlignment    = DMA_MDATAALIGN_WORD,
        .Mode                = DMA_CIRCULAR,
        .Priority            = DMA_PRIORITY_HIGH,
        .FIFOMode            = DMA_FIFOMODE_ENABLE,
        .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
        .MemBurst            = DMA_MBURST_SINGLE,
        .PeriphBurst         = DMA_PBURST_SINGLE,
    },
};

static void msp_sai_mspInit(SAI_HandleTypeDef *hsai)
{
    msp_clock_sai_config(hsai->Init.AudioFrequency);

    __HAL_RCC_SAI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef  gpio_init_structure = {
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_HIGH,
        .Alternate = GPIO_AF6_SAI1,
    };

    //MCK
    gpio_init_structure.Pin =  GPIO_PIN_0;
    HAL_GPIO_Init(GPIOC, &gpio_init_structure);
    //SD
    gpio_init_structure.Pin = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOA, &gpio_init_structure);
    //FS
    gpio_init_structure.Pin = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOB, &gpio_init_structure);
    //SCK
    gpio_init_structure.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOB, &gpio_init_structure);

    /* Enable the DMA clock */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_tx);
    /* Configure the DMA Stream */
    HAL_DMA_Init(&hdma_tx);

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmatx, hdma_tx);

    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 6, 0);

    NVIC_SetVector(DMA2_Stream5_IRQn, (uint32_t)&msp_sai_dma_irq);

    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
}

static void msp_sai_mspDeInit(SAI_HandleTypeDef *hsai)
{
    __HAL_SAI_DISABLE(hsai);

    HAL_NVIC_DisableIRQ(DMA2_Stream5_IRQn);

    HAL_DMA_DeInit(hsai->hdmatx);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12);

    __HAL_RCC_SAI1_CLK_DISABLE();
}

uint16_t msp_sai_out_dma_remaining(void)
{
    uint16_t ptr = (LL_DMA_ReadReg(hsai_out.hdmatx->Instance, NDTR) & 0xFFFF);
    return ptr;
}

/***************** 中断 *********************/

static void msp_sai_dma_irq(void)
{
    HAL_DMA_IRQHandler(&hdma_tx);
}

/* void DMA2_Stream5_IRQHandler(void) */
/* { */
    /* msp_sai_dma_irq(); */
/* } */

/* --------------------- */

void msp_sai_out_init(uint32_t AudioFreq, uint8_t bit_depth)
{
    hsai_out.Init.AudioFrequency = AudioFreq;

    if (bit_depth == 16U) {
        hsai_out.Init.DataSize = SAI_DATASIZE_16;
    } else {
        hsai_out.Init.DataSize = SAI_DATASIZE_24;
    }

    if (HAL_SAI_Init(&hsai_out) != HAL_OK) {
        printf("sai out init error\n");
        return;
    }
}

void msp_sai_out_deInit(void)
{
    HAL_SAI_DMAStop(&hsai_out);
    HAL_SAI_DeInit(&hsai_out);
}
