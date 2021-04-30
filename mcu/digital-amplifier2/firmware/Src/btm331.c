#include "base.h"
#include <string.h>
#include <stdio.h>

/* 16bit/8 * 2 * 500 */
#define BUF_SIZE 2000

static void _dma_half_cplt(SAI_HandleTypeDef *hsai);
static void _dma_cplt(SAI_HandleTypeDef *hsai);
static void _mspDeInit(SAI_HandleTypeDef *hsai);
static void _mspInit(SAI_HandleTypeDef *hsai);

static uint8_t buf[BUF_SIZE];

SAI_HandleTypeDef hsai_in = {
    .Instance = SAI1_Block_A,
    .Init = {
        .AudioFrequency = SAI_AUDIO_FREQUENCY_48K,
        .ClockSource   = SAI_CLKSOURCE_PLLI2S,
        .AudioMode     = SAI_MODESLAVE_RX,
        .NoDivider     = SAI_MASTERDIVIDER_ENABLE,
        .Protocol      = SAI_FREE_PROTOCOL,
        .DataSize      = SAI_DATASIZE_16,
        .FirstBit      = SAI_FIRSTBIT_MSB,
        .ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE,
        .Synchro       = SAI_ASYNCHRONOUS,
        .OutputDrive   = SAI_OUTPUTDRIVE_DISABLE,
        .FIFOThreshold = SAI_FIFOTHRESHOLD_1QF,
    },
    .FrameInit = {
        .FrameLength       = 64,
        .ActiveFrameLength = 32,
        .FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION,
        .FSPolarity        = SAI_FS_ACTIVE_HIGH,
        .FSOffset          = SAI_FS_FIRSTBIT,
    },
    .SlotInit = {
        .FirstBitOffset = 1,
        .SlotSize       = SAI_SLOTSIZE_32B,
        .SlotNumber     = 2,
        .SlotActive     = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1,
    },
    .MspInitCallback    = _mspInit,
    .MspDeInitCallback  = _mspDeInit,
};

static DMA_HandleTypeDef hdma_rx = {
        .Instance = DMA2_Stream1,
        .Init = {
            .Channel             = DMA_CHANNEL_0,
            .Direction           = DMA_PERIPH_TO_MEMORY,
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

static void _mspInit(SAI_HandleTypeDef *hsai)
{
    __HAL_RCC_SAI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef  gpio_init = {
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_HIGH,
        .Alternate = GPIO_AF6_SAI1,
    };

    //SD
    gpio_init.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOC, &gpio_init);
    //FS
    gpio_init.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &gpio_init);
    //SCK
    gpio_init.Pin = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOB, &gpio_init);

    /* Enable the DMA clock */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmarx, hdma_rx);

    /* Configure the DMA Stream */
    HAL_DMA_DeInit(&hdma_rx);
    HAL_DMA_Init(&hdma_rx);

    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
}

static void _mspDeInit(SAI_HandleTypeDef *hsai)
{
    __HAL_SAI_DISABLE(hsai);

    HAL_NVIC_DisableIRQ(DMA2_Stream1_IRQn);

    HAL_DMA_DeInit(hsai->hdmarx);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
}

/***************** 中断 *********************/

void DMA2_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_rx);
}


/*********************************/

void btm331_init(void)
{
    GPIO_InitTypeDef  gpio_init = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW,
    };

    //button: bt en
    gpio_init.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    //state: 1 2
    gpio_init.Mode = GPIO_MODE_INPUT,
    gpio_init.Pull = GPIO_PULLDOWN,
    gpio_init.Pin = GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11 | GPIO_PIN_12, 0);
}

void btm331_start(void)
{
    btm331_init();

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 1);

    if (HAL_SAI_Init(&hsai_in) != HAL_OK) {
        printf("sai init error\n");
        return;
    }

    HAL_SAI_RegisterCallback(&hsai_in, HAL_SAI_RX_COMPLETE_CB_ID, _dma_cplt);
    HAL_SAI_RegisterCallback(&hsai_in, HAL_SAI_RX_HALFCOMPLETE_CB_ID, _dma_half_cplt);

    HAL_StatusTypeDef ret;
    ret = HAL_SAI_Receive_DMA(&hsai_in, (uint8_t *)&buf[0], BUF_SIZE/2);
    if (ret!= HAL_OK) {
        printf("btm331 play err:%d\n", ret);
        return;
    }

    audio_init(SAI_AUDIO_FREQUENCY_48K, 16);
}

void btm331_reset(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1);
    //长按3秒进入配对
    vTaskDelay(3500);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 0);
}

void btm331_stop(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0);

    audio_stop();
    HAL_SAI_DeInit(&hsai_in);
}

static void _dma_half_cplt(SAI_HandleTypeDef *hsai)
{
    audio_append(&buf[0], BUF_SIZE/2);
    audio_clock_sync();
}

static void _dma_cplt(SAI_HandleTypeDef *hsai)
{
    audio_append(&buf[BUF_SIZE/2], BUF_SIZE/2);
    audio_clock_sync();
}
