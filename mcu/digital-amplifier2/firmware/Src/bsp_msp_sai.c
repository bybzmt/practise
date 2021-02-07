
#include <string.h>
#include <stdio.h>

#include "bsp_audio.h"
#include "stm32f4xx_ll_dma.h"

static void MY_SAI_OUT_MspDeInit(SAI_HandleTypeDef *hsai);
static void MY_SAI_OUT_MspInit(SAI_HandleTypeDef *hsai);

SAI_HandleTypeDef hsai_out = {
    .Instance = SAI1_Block_B,
    .Init = {
        .ClockSource   = SAI_CLKSOURCE_PLLI2S,
        .AudioMode     = SAI_MODEMASTER_TX,
        .NoDivider     = SAI_MASTERDIVIDER_ENABLE,
        .Protocol      = SAI_FREE_PROTOCOL,
        .DataSize      = SAI_DATASIZE_16,
        .FirstBit      = SAI_FIRSTBIT_MSB,
        .ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE,
        .Synchro       = SAI_ASYNCHRONOUS,
        .OutputDrive   = SAI_OUTPUTDRIVE_ENABLE,
        .FIFOThreshold = SAI_FIFOTHRESHOLD_1QF,
    },
    .FrameInit = {
        .FrameLength       = 32,
        .ActiveFrameLength = 16,
        .FSDefinition      = SAI_FS_STARTFRAME,
        .FSPolarity        = SAI_FS_ACTIVE_LOW,
        .FSOffset          = SAI_FS_FIRSTBIT,
    },
    .SlotInit = {
        .FirstBitOffset = 1,
        .SlotSize       = SAI_SLOTSIZE_DATASIZE,
        .SlotNumber     = 2,
        .SlotActive     = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1,
    },
    .MspInitCallback    = MY_SAI_OUT_MspInit,
    .MspDeInitCallback  = MY_SAI_OUT_MspDeInit,
};

static DMA_HandleTypeDef hdma_sai_tx = {
    .Instance = DMA2_Stream5,
    .Init = {
        .Channel             = DMA_CHANNEL_0,
        .Direction           = DMA_MEMORY_TO_PERIPH,
        .PeriphInc           = DMA_PINC_DISABLE,
        .MemInc              = DMA_MINC_ENABLE,
        .PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD,
        .MemDataAlignment    = DMA_MDATAALIGN_HALFWORD,
        .Mode                = DMA_CIRCULAR,
        .Priority            = DMA_PRIORITY_HIGH,
        .FIFOMode            = DMA_FIFOMODE_ENABLE,
        .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
        .MemBurst            = DMA_MBURST_SINGLE,
        .PeriphBurst         = DMA_PBURST_SINGLE,
    },
};

static void MY_SAI_OUT_MspInit(SAI_HandleTypeDef *hsai)
{
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

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmatx, hdma_sai_tx);

    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_sai_tx);

    /* Configure the DMA Stream */
    HAL_DMA_Init(&hdma_sai_tx);

    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
}

static void MY_SAI_OUT_MspDeInit(SAI_HandleTypeDef *hsai)
{
    __HAL_SAI_DISABLE(hsai);

    HAL_NVIC_DisableIRQ(DMA2_Stream5_IRQn);

    HAL_DMA_DeInit(hsai->hdmatx);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12);

    __HAL_RCC_SAI1_CLK_DISABLE();
    //__HAL_RCC_DMA2_CLK_DISABLE();
}

void BSP_SAI_ClockConfig(uint32_t AudioFreq)
{
    RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;

    HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);

    /* Set the PLL configuration according to the audio frequency */
    if ((AudioFreq == SAI_AUDIO_FREQUENCY_11K) || (AudioFreq == SAI_AUDIO_FREQUENCY_22K) || (AudioFreq == SAI_AUDIO_FREQUENCY_44K))
    {
        /* Configure PLLSAI prescalers */
        /* PLLSAI_VCO: VCO_429M
           SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
           SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 214.5/19 = 11.289 Mhz */
        RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
        RCC_ExCLKInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLI2S;
        RCC_ExCLKInitStruct.SpdifClockSelection = RCC_SPDIFRXCLKSOURCE_PLLI2SP;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SM = 8;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 429;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 2;
        RCC_ExCLKInitStruct.PLLI2SDivQ = 19;

        HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
    }
    else /* SAI_AUDIO_FREQUENCY_8K, SAI_AUDIO_FREQUENCY_16K, SAI_AUDIO_FREQUENCY_48K), SAI_AUDIO_FREQUENCY_96K */
    {
        /* SAI clock config
           PLLSAI_VCO: VCO_344M
           SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 344/7 = 49.142 Mhz
           SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 49.142/1 = 49.142 Mhz */
        RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
        RCC_ExCLKInitStruct.SpdifClockSelection = RCC_SPDIFRXCLKSOURCE_PLLI2SP;
        RCC_ExCLKInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLI2S;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SM = 8;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 344;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 7;
        RCC_ExCLKInitStruct.PLLI2SDivQ = 1;

        HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
    }
}


/***************** 中断 *********************/

void DMA2_Stream5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(hsai_out.hdmatx);
}

/* void SAI1_IRQHandler(void) {} */



