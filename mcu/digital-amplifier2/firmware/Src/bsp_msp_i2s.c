#include "base.h"
#include <string.h>
#include <stdio.h>

static void my_MspDeInit(I2S_HandleTypeDef *hi2s);
static void my_MspInit(I2S_HandleTypeDef *hi2s);

I2S_HandleTypeDef hi2s1 = {
    .Instance = SPI1,
    .Init = {
        .AudioFreq = I2S_AUDIOFREQ_44K,
        .ClockSource = I2S_CLOCK_PLL,
        .CPOL = I2S_CPOL_LOW,
        .DataFormat = I2S_DATAFORMAT_16B,
        .MCLKOutput = I2S_MCLKOUTPUT_ENABLE,
        .Mode = I2S_MODE_MASTER_TX,
        .Standard = I2S_STANDARD_PHILIPS,
    },
    .MspInitCallback    = my_MspInit,
    .MspDeInitCallback  = my_MspDeInit,
};

static DMA_HandleTypeDef hdma_tx = {
    .Instance = DMA2_Stream3,
    .Init = {
        .Channel             = DMA_CHANNEL_3,
        .Direction           = DMA_MEMORY_TO_PERIPH,
        .PeriphInc           = DMA_PINC_DISABLE,
        .MemInc              = DMA_MINC_ENABLE,
        .PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD,
        .MemDataAlignment    = DMA_PDATAALIGN_HALFWORD,
        .Mode                = DMA_CIRCULAR,
        .Priority            = DMA_PRIORITY_HIGH,
        .FIFOMode            = DMA_FIFOMODE_DISABLE,
        .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
        .MemBurst            = DMA_MBURST_SINGLE,
        .PeriphBurst         = DMA_MBURST_SINGLE,
    },
};

static void my_MspInit(I2S_HandleTypeDef *hi2s)
{
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef  gpio_init_structure = {
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_HIGH,
        .Alternate = GPIO_AF5_SPI1,
    };

    //MCK
    gpio_init_structure.Pin =  GPIO_PIN_4;
    HAL_GPIO_Init(GPIOC, &gpio_init_structure);
    //fs, sck, sd
    gpio_init_structure.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &gpio_init_structure);

    __HAL_I2S_ENABLE(hi2s);

    /* Enable the DMA clock */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Associate the DMA handle */
    __HAL_LINKDMA(hi2s, hdmatx, hdma_tx);

    /* Configure the DMA Stream */
    HAL_DMA_DeInit(&hdma_tx);
    if (HAL_DMA_Init(&hdma_tx) != HAL_OK) {
        printf("i2s dma init err\n");
    }

    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
}

static void my_MspDeInit(I2S_HandleTypeDef *hi2s)
{
    __HAL_I2S_DISABLE(hi2s);

    HAL_NVIC_DisableIRQ(DMA2_Stream3_IRQn);

    HAL_DMA_DeInit(hi2s->hdmatx);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7);

    __HAL_RCC_SPI1_CLK_DISABLE();
}

/***************** 中断 *********************/

void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_tx);
}


