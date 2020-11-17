
#include <string.h>
#include <stdio.h>

#include "bsp_audio.h"
#include "stm32f4xx_ll_dma.h"

static void BSP_SAI_ClockConfig(uint32_t AudioFreq);
static void BSP_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_ErrorCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_OUT_MspDeInit_Callback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_OUT_MspInit_Callback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_IN_MspInit_Callback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_IN_MspDeInit_Callback(SAI_HandleTypeDef *hsai);

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
        .FSOffset          = SAI_FS_BEFOREFIRSTBIT,
    },
    .SlotInit = {
        .FirstBitOffset = 0,
        .SlotSize       = SAI_SLOTSIZE_DATASIZE,
        .SlotNumber     = 2,
        .SlotActive     = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1,
    },
    .TxCpltCallback     = BSP_SAI_TxCpltCallback,
    .TxHalfCpltCallback = BSP_SAI_TxHalfCpltCallback,
    .ErrorCallback      = BSP_SAI_ErrorCallback,
    .MspInitCallback    = BSP_SAI_OUT_MspInit_Callback,
    .MspDeInitCallback  = BSP_SAI_OUT_MspDeInit_Callback,
};

SAI_HandleTypeDef hsai_in = {
    .Instance = SAI1_Block_A,
    .Init = {
        .ClockSource   = SAI_CLKSOURCE_PLLI2S,
        /* .ClockSource   = SAI_CLKSOURCE_NA, */
        /* .AudioMode     = SAI_MODEMASTER_RX, */
        .AudioMode     = SAI_MODESLAVE_RX,
        .NoDivider     = SAI_MASTERDIVIDER_ENABLE,
        /* .NoDivider     = SAI_MASTERDIVIDER_DISABLE, */
        .Protocol      = SAI_FREE_PROTOCOL,
        .DataSize      = SAI_DATASIZE_24,
        .FirstBit      = SAI_FIRSTBIT_MSB,
        .ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE,
        .Synchro       = SAI_ASYNCHRONOUS,
        /* .Synchro       = SAI_SYNCHRONOUS, */
        /* .OutputDrive   = SAI_OUTPUTDRIVE_ENABLE, */
        .OutputDrive   = SAI_OUTPUTDRIVE_DISABLE,
        /* .FIFOThreshold = SAI_FIFOTHRESHOLD_1QF, */
        .FIFOThreshold = SAI_FIFOTHRESHOLD_HF,
    },
    .FrameInit = {
        .FrameLength       = 64,
        .ActiveFrameLength = 32,
        .FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION,
        .FSPolarity        = SAI_FS_ACTIVE_LOW,
        .FSOffset          = SAI_FS_BEFOREFIRSTBIT,
    },
    .SlotInit = {
        .FirstBitOffset = 0,
        .SlotSize       = SAI_SLOTSIZE_DATASIZE,
        .SlotNumber     = 2,
        .SlotActive     = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1,
    },
    .RxCpltCallback     = BSP_SAI_RxCpltCallback,
    .RxHalfCpltCallback = BSP_SAI_RxHalfCpltCallback,
    .ErrorCallback      = BSP_SAI_ErrorCallback,
    .MspInitCallback    = BSP_SAI_IN_MspInit_Callback,
    .MspDeInitCallback  = BSP_SAI_IN_MspDeInit_Callback,
};

static DMA_HandleTypeDef hdma_sai_tx = {
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

static DMA_HandleTypeDef hdma_sai_rx = {
    .Instance = DMA2_Stream3,
    .Init = {
        .Channel             = DMA_CHANNEL_0,
        .Direction           = DMA_PERIPH_TO_MEMORY,
        .PeriphInc           = DMA_PINC_DISABLE,
        .MemInc              = DMA_MINC_ENABLE,
        .PeriphDataAlignment = DMA_MDATAALIGN_WORD,
        .MemDataAlignment    = DMA_MDATAALIGN_WORD,
        .Mode                = DMA_CIRCULAR,
        .Priority            = DMA_PRIORITY_HIGH,
        /* .FIFOMode            = DMA_FIFOMODE_ENABLE, */
        .FIFOMode            = DMA_FIFOMODE_DISABLE,
        /* .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL, */
        .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
        .MemBurst            = DMA_MBURST_SINGLE,
        .PeriphBurst         = DMA_PBURST_SINGLE,
    },
};

/* mode: 1:usb 2:adc */
static uint8_t audio_mode;

uint8_t BSP_AUDIO_Init(uint8_t mode, uint8_t Volume, uint32_t AudioFreq)
{
    BSP_AUDIO_DeInit();

    audio_mode = mode;

    BSP_SAI_ClockConfig(AudioFreq);

    hsai_in.Init.AudioFrequency = AudioFreq;
    hsai_out.Init.AudioFrequency = AudioFreq;

    if (HAL_SAI_Init(&hsai_out) != HAL_OK)
    {
        return AUDIO_ERROR;
    }

    if (HAL_SAI_Init(&hsai_in) != HAL_OK)
    {
        return AUDIO_ERROR;
    }

    __HAL_SAI_ENABLE(&hsai_out);
    __HAL_SAI_ENABLE(&hsai_in);

    return AUDIO_OK;
}

uint8_t BSP_AUDIO_Play(uint8_t* pBuffer, uint32_t Size)
{
    uint8_t ret = HAL_SAI_Transmit_DMA(&hsai_out, pBuffer, Size);
    if (ret!= HAL_OK) {
        printf("play err:%d\n", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

static uint8_t record_buf[1024] = {0};

uint8_t BSP_AUDIO_Record(void)
{
    printf("record\n");

    BSP_AUDIO_Init(2, 100, SAI_AUDIO_FREQUENCY_44K);

    uint8_t ret = HAL_SAI_Receive_DMA(&hsai_in, record_buf, 1024);
    if (ret!= HAL_OK) {
        printf("record err:%d\n", ret);
        /* return AUDIO_ERROR; */
    }

    HAL_Delay(10);

    BSP_AUDIO_Play(record_buf, 1024);

    printf("record ok\n");
    return AUDIO_OK;
}

uint8_t BSP_AUDIO_OUT_Pause(void)
{
    if (HAL_SAI_DMAPause(&hsai_out)!= HAL_OK)
    {
        return  AUDIO_ERROR;
    }

    return AUDIO_OK;
}

uint8_t BSP_AUDIO_OUT_Resume(void)
{
    if (HAL_SAI_DMAResume(&hsai_out)!= HAL_OK)
    {
        return AUDIO_ERROR;
    }

    return AUDIO_OK;
}

uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t Volume)
{
    return AUDIO_OK;
}

uint8_t BSP_AUDIO_OUT_SetMute(uint32_t Cmd)
{
    return AUDIO_OK;
}

void BSP_AUDIO_DeInit(void)
{
    __HAL_SAI_DISABLE(&hsai_out);
    __HAL_SAI_DISABLE(&hsai_in);
    HAL_SAI_DeInit(&hsai_out);
    HAL_SAI_DeInit(&hsai_in);
}

uint32_t BSP_AUDIO_OUT_GetRemainingDataSize(void)
{
    return LL_DMA_ReadReg(AUDIO_SAIx_DMAx_STREAM, NDTR) & 0xFFFF;
}

static void BSP_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (audio_mode == 1) {
        BSP_AUDIO_OUT_TransferComplete_CallBack();
    }
}

static void BSP_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (audio_mode == 1) {
        BSP_AUDIO_OUT_HalfTransfer_CallBack();
    }
}

static void BSP_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    printf("in\n");
}

static void BSP_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    printf("half\n");
}

static void BSP_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    uint32_t num = HAL_SAI_GetError(hsai);
    printf("sai err:%ld\n", num);
}

static void BSP_SAI_OUT_MspInit_Callback(SAI_HandleTypeDef *hsai)
{
    __HAL_RCC_SAI1_CLK_ENABLE();

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
    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, AUDIO_OUT_IRQ_PREPRIO, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
}

static void BSP_SAI_OUT_MspDeInit_Callback(SAI_HandleTypeDef *hsai)
{
    HAL_NVIC_DisableIRQ(DMA2_Stream5_IRQn);

    HAL_DMA_DeInit(hsai->hdmatx);

    __HAL_SAI_DISABLE(hsai);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12);

    __HAL_RCC_SAI1_CLK_DISABLE();
    //__HAL_RCC_DMA2_CLK_DISABLE();
}

static void BSP_SAI_IN_MspInit_Callback(SAI_HandleTypeDef *hsai)
{
    __HAL_RCC_SAI1_CLK_ENABLE();

    GPIO_InitTypeDef  gpio_init_structure = {
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_HIGH,
        .Alternate = GPIO_AF6_SAI1,
    };

    //MCK stm32f446ret6的SAI1_Block_A无MCK
    //SD
    gpio_init_structure.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOB, &gpio_init_structure);
    //FS
    gpio_init_structure.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &gpio_init_structure);
    //SCK
    gpio_init_structure.Pin = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOB, &gpio_init_structure);

    /* Enable the DMA clock */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmarx, hdma_sai_rx);

    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_sai_rx);

    /* Configure the DMA Stream */
    HAL_DMA_Init(&hdma_sai_rx);

    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, AUDIO_OUT_IRQ_PREPRIO, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
}

static void BSP_SAI_IN_MspDeInit_Callback(SAI_HandleTypeDef *hsai)
{
    HAL_NVIC_DisableIRQ(DMA2_Stream3_IRQn);

    HAL_DMA_DeInit(hsai->hdmarx);

    __HAL_SAI_DISABLE(hsai);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);

    __HAL_RCC_SAI1_CLK_DISABLE();
    //__HAL_RCC_DMA2_CLK_DISABLE();
}

static void BSP_SAI_ClockConfig(uint32_t AudioFreq)
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
        RCC_ExCLKInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLI2S;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SM = 8;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 344;
        RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 7;
        RCC_ExCLKInitStruct.PLLI2SDivQ = 1;

        HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
    }
}






