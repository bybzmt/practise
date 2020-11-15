
#include <string.h>
#include <stdio.h>

#include "bsp_audio.h"
#include "stm32f4xx_ll_dma.h"


/*### PLAY ###*/
/* SCK(kHz) = SAI_CK_x/(SAIClockDivider*2*256) */
#define SAIClockDivider(__FREQUENCY__) \
        (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_8K)  ? 12 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_11K) ? 2 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_16K) ? 6 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_22K) ? 1 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_32K) ? 3 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_44K) ? 0 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_48K) ? 2 : 1  \



/*
Note:
  these global variables are not compliant to naming rules (upper case without "_" ),
  but we keep this naming for compatibility, in fact these variables (not static)
  could have been used by the application, example the stm32f4xx_it.c:
    void DMA2_Stream6_IRQHandler(void)
     {  HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);  }
*/
SAI_HandleTypeDef         haudio_out_sai;


static uint8_t SAIx_Init(uint32_t AudioFreq);
static void SAIx_DeInit(void);


/**
  * @brief  Configures the audio peripherals.
  * @param  OutputDevice: OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
  *                       or OUTPUT_DEVICE_BOTH.
  * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @note   The SAI PLL input clock must be done in the user application.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_OUT_Init(uint16_t OutputDevice,
                           uint8_t Volume,
                           uint32_t AudioFreq)
{
    /* printf("bsp Init %ld\n", AudioFreq); */

  uint8_t ret = AUDIO_OK;

  SAIx_DeInit();

  /* PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups) */
  BSP_AUDIO_OUT_ClockConfig(&haudio_out_sai, AudioFreq, NULL);

  /* SAI data transfer preparation:
  Prepare the Media to be used for the audio transfer from memory to SAI peripheral */
  haudio_out_sai.Instance = AUDIO_SAIx;
  if(HAL_SAI_GetState(&haudio_out_sai) == HAL_SAI_STATE_RESET)
  {
    /* Init the SAI MSP: this __weak function can be redefined by the application*/
    BSP_AUDIO_OUT_MspInit(&haudio_out_sai, NULL);
  }

  if (SAIx_Init(AudioFreq) != AUDIO_OK)
  {
    ret = AUDIO_ERROR;
  }

  if(ret == AUDIO_OK)
  {
      //todo: 初始化解码芯片
  }

  return ret;
}

/**
  * @brief  Starts playing audio stream from a data buffer for a determined size.
  * @param  pBuffer: Pointer to the buffer
  * @param  Size: Number of audio data BYTES.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_OUT_Play(uint16_t* pBuffer, uint32_t Size)
{
    /* printf("bsp Play\n"); */
    uint8_t ret = AUDIO_OK;

    /* Initiate a DMA transfer of PCM samples towards the serial audio interface */
    if (HAL_SAI_Transmit_DMA(&haudio_out_sai, (uint8_t*) pBuffer, DMA_MAX(Size / AUDIODATA_SIZE))!= HAL_OK)
    {
        ret = AUDIO_ERROR;
    }

    return ret;
}

/**
  * @brief   This function Pauses the audio file stream. In case
  *          of using DMA, the DMA Pause feature is used.
  * @warning When calling BSP_AUDIO_OUT_Pause() function for pause, only
  *          BSP_AUDIO_OUT_Resume() function should be called for resume (use of BSP_AUDIO_OUT_Play()
  *          function for resume could lead to unexpected behavior).
  * @retval  AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_OUT_Pause(void)
{
    /* printf("bsp Pause\n"); */
    uint8_t ret = AUDIO_OK;

    //todo: 解码芯片暂停

    /* Pause DMA transfer of PCM samples towards the serial audio interface */
    if (HAL_SAI_DMAPause(&haudio_out_sai)!= HAL_OK)
    {
        ret =  AUDIO_ERROR;
    }

    /* Return AUDIO_OK when all operations are correctly done */
    return ret;
}

/**
  * @brief  This function  Resumes the audio file stream.
  * WARNING: When calling BSP_AUDIO_OUT_Pause() function for pause, only
  *          BSP_AUDIO_OUT_Resume() function should be called for resume
  *          (use of BSP_AUDIO_OUT_Play() function for resume could lead to
  *           unexpected behavior).
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_OUT_Resume(void)
{
    /* printf("bsp Resume\n"); */
    uint8_t ret = AUDIO_OK;

    //todo: 解码芯片暂停

    /* Resume DMA transfer of PCM samples towards the serial audio interface */
    if (HAL_SAI_DMAResume(&haudio_out_sai)!= HAL_OK)
    {
        ret =  AUDIO_ERROR;
    }

    /* Return AUDIO_OK when all operations are correctly done */
    return ret;
}

/**
  * @brief  Stops audio playing and Power down the Audio Codec.
  * @param  Option: could be one of the following parameters
  *           - CODEC_PDWN_SW: for software power off (by writing registers).
  *                            Then no need to reconfigure the Codec after power on.
  *           - CODEC_PDWN_HW: completely shut down the codec (physically).
  *                            Then need to reconfigure the Codec after power on.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_OUT_Stop(uint32_t Option)
{
    /* printf("bsp Stop\n"); */

    uint8_t ret = AUDIO_OK;

    /* Stop DMA transfer of PCM samples towards the serial audio interface */
    if (HAL_SAI_DMAStop(&haudio_out_sai)!= HAL_OK)
    {
        ret = AUDIO_ERROR;
    }

    /* Return AUDIO_OK when all operations are correctly done */
    return ret;
}

/**
  * @brief  Controls the current audio volume level.
  * @param  Volume: Volume level to be set in percentage from 0% to 100% (0 for
  *         Mute and 100 for Max volume level).
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t Volume)
{
    /* printf("bsp SetVolume\n"); */
    uint8_t ret = AUDIO_OK;

    //todo: 解码芯片调节音量

    return ret;
}

/**
  * @brief  Enables or disables the MUTE mode by software
  * @param  Cmd: Could be AUDIO_MUTE_ON to mute sound or AUDIO_MUTE_OFF to
  *         unmute the codec and restore previous volume level.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_OUT_SetMute(uint32_t Cmd)
{
    /* printf("bsp SetMute\n"); */

    uint8_t ret = AUDIO_OK;

    //todo: 解码芯片静音

    return ret;
}



/**
  * @brief  Updates the audio frequency.
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @note   This API should be called after the BSP_AUDIO_OUT_Init() to adjust the
  *         audio frequency.
  */
void BSP_AUDIO_OUT_SetFrequency(uint32_t AudioFreq)
{
    /* printf("bsp SetFrequency\n"); */

  /* PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups) */
  BSP_AUDIO_OUT_ClockConfig(&haudio_out_sai, AudioFreq, NULL);

  /* Disable SAI peripheral to allow access to SAI internal registers */
  __HAL_SAI_DISABLE(&haudio_out_sai);

  /* Update the SAI audio frequency configuration */
  haudio_out_sai.Init.AudioFrequency = AudioFreq;
  HAL_SAI_Init(&haudio_out_sai);

  /* Enable SAI peripheral to generate MCLK */
  __HAL_SAI_ENABLE(&haudio_out_sai);
}

/**
  * @brief  Updates the Audio frame slot configuration.
  * @param  AudioFrameSlot: specifies the audio Frame slot
  *         This parameter can be any value of @ref CODEC_AudioFrame_SLOT_TDMMode
  * @note   This API should be called after the BSP_AUDIO_OUT_Init() to adjust the
  *         audio frame slot.
  */
void BSP_AUDIO_OUT_SetAudioFrameSlot(uint32_t AudioFrameSlot)
{
    /* printf("bsp SetAudioFrameSlot\n"); */

  /* Disable SAI peripheral to allow access to SAI internal registers */
  __HAL_SAI_DISABLE(&haudio_out_sai);

  /* Update the SAI audio frame slot configuration */
  haudio_out_sai.SlotInit.SlotActive = AudioFrameSlot;
  HAL_SAI_Init(&haudio_out_sai);

  /* Enable SAI peripheral to generate MCLK */
  __HAL_SAI_ENABLE(&haudio_out_sai);
}

/**
  * @brief  Deinit the audio peripherals.
  */
void BSP_AUDIO_OUT_DeInit(void)
{
    /* printf("bsp DeInit\n"); */

  SAIx_DeInit();
  /* DeInit the SAI MSP : this __weak function can be rewritten by the applic */
  BSP_AUDIO_OUT_MspDeInit(&haudio_out_sai, NULL);

  //todo: 解码清理工作
}

/**
 * @brief  Get size of remaining audio data to be transmitted.
 * @see    STM32F469xx Reference Manual 9.5.6 (p.248)
 */
uint32_t BSP_AUDIO_OUT_GetRemainingDataSize(void)
{
    /* printf("bsp dataSize\n"); */
  return LL_DMA_ReadReg(AUDIO_SAIx_DMAx_STREAM, NDTR) & 0xFFFF;
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  hsai: SAI handle
  */
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    /* Manage the remaining file size and new address offset: This function
       should be coded by user (its prototype is already declared in stm32469i_discovery_audio.h) */
    /* BSP_AUDIO_OUT_TransferComplete_CallBack(); */
}

/**
  * @brief  Tx Half Transfer completed callbacks.
  * @param  hsai: SAI handle
  */
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    /* Manage the remaining file size and new address offset: This function
       should be coded by user (its prototype is already declared in stm32469i_discovery_audio.h) */
    /* BSP_AUDIO_OUT_HalfTransfer_CallBack(); */
}

/**
  * @brief  SAI error callbacks.
  * @param  hsai: SAI handle
  */
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    /* BSP_AUDIO_OUT_Error_CallBack(); */
}

/**
  * @brief  Initializes BSP_AUDIO_OUT MSP.
  * @param  hsai: SAI handle
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
void BSP_AUDIO_OUT_MspInit(SAI_HandleTypeDef *hsai, void *Params)
{
    /* printf("bsp MspInit\n"); */

  static DMA_HandleTypeDef hdma_sai_tx;
  GPIO_InitTypeDef  gpio_init_structure;

  //todo: 解码芯片设置

  /* Enable clock */
  __HAL_RCC_SAI1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = GPIO_AF6_SAI1;

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

  if (hsai->Instance == AUDIO_SAIx)
  {
    /* Configure the hdma_saiTx handle parameters */
    hdma_sai_tx.Init.Channel             = AUDIO_SAIx_DMAx_CHANNEL;
    hdma_sai_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_sai_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_sai_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_sai_tx.Init.PeriphDataAlignment = AUDIO_SAIx_DMAx_PERIPH_DATA_SIZE;
    hdma_sai_tx.Init.MemDataAlignment    = AUDIO_SAIx_DMAx_MEM_DATA_SIZE;
    hdma_sai_tx.Init.Mode                = DMA_CIRCULAR;
    hdma_sai_tx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_sai_tx.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    hdma_sai_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
    hdma_sai_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

    hdma_sai_tx.Instance = AUDIO_SAIx_DMAx_STREAM;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmatx, hdma_sai_tx);

    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_sai_tx);

    /* Configure the DMA Stream */
    HAL_DMA_Init(&hdma_sai_tx);
  }

  /* SAI DMA IRQ Channel configuration */
  HAL_NVIC_SetPriority(AUDIO_SAIx_DMAx_IRQ, AUDIO_OUT_IRQ_PREPRIO, 0);
  HAL_NVIC_EnableIRQ(AUDIO_SAIx_DMAx_IRQ);
}

/**
  * @brief  Deinitializes BSP_AUDIO_OUT MSP.
  * @param  hsai: SAI handle
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
void BSP_AUDIO_OUT_MspDeInit(SAI_HandleTypeDef *hsai, void *Params)
{
    /* printf("bsp MspDeInit\n"); */

  /* SAI DMA IRQ Channel deactivation */
  HAL_NVIC_DisableIRQ(AUDIO_SAIx_DMAx_IRQ);

  if (hsai->Instance == AUDIO_SAIx)
  {
    /* Deinitialize the DMA stream */
    HAL_DMA_DeInit(hsai->hdmatx);
  }

  /* Disable SAI peripheral */
  __HAL_SAI_DISABLE(hsai);

  //todo: 启用解码芯片

  //MCK
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
  //SD
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
  //FS
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
  //SCK
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12);

  /* Disable SAI clock */
  __HAL_RCC_SAI1_CLK_DISABLE();

  /* GPIO pins clock and DMA clock can be shut down in the applic
     by surcgarging this __weak function */
  //__HAL_RCC_DMA2_CLK_DISABLE();
}

/**
  * @brief  Clock Config.
  * @param  hsai: might be required to set audio peripheral predivider if any.
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @note   This API is called by BSP_AUDIO_OUT_Init() and BSP_AUDIO_OUT_SetFrequency()
  *         Being __weak it can be overwritten by the application
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
void BSP_AUDIO_OUT_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t AudioFreq, void *Params)
{
    /* printf("bsp ClockConfig\n"); */

    RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;

    HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);

    /* Set the PLL configuration according to the audio frequency */
    if((AudioFreq == SAI_AUDIO_FREQUENCY_11K) || (AudioFreq == SAI_AUDIO_FREQUENCY_22K) || (AudioFreq == SAI_AUDIO_FREQUENCY_44K))
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

/*******************************************************************************
                            Static Functions
*******************************************************************************/
/**
  * @brief  Initializes the Audio Codec audio interface (SAI).
  * @param  AudioFreq: Audio frequency to be configured for the SAI peripheral.
  * @note   The default SlotActive configuration is set to CODEC_AUDIOFRAME_SLOT_0123
  *         and user can update this configuration using
  */
static uint8_t SAIx_Init(uint32_t AudioFreq)
{
    /* printf("sai Init\n"); */

    uint8_t ret = AUDIO_OK;

    /* Initialize the haudio_out_sai Instance parameter */
    haudio_out_sai.Instance = AUDIO_SAIx;

    /* Disable SAI peripheral to allow access to SAI internal registers */
    __HAL_SAI_DISABLE(&haudio_out_sai);

    /* Configure SAI_Block_x
LSBFirst: Disabled
DataSize: 16 */
    haudio_out_sai.Init.AudioFrequency = AudioFreq;
    haudio_out_sai.Init.ClockSource = SAI_CLKSOURCE_PLLI2S;
    haudio_out_sai.Init.AudioMode = SAI_MODEMASTER_TX;
    haudio_out_sai.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
    haudio_out_sai.Init.Protocol = SAI_FREE_PROTOCOL;
    haudio_out_sai.Init.DataSize = SAI_DATASIZE_24;
    haudio_out_sai.Init.FirstBit = SAI_FIRSTBIT_MSB;
    haudio_out_sai.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
    haudio_out_sai.Init.Synchro = SAI_ASYNCHRONOUS;
    haudio_out_sai.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;
    haudio_out_sai.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;
    /*
       haudio_out_sai.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_MCKDIV;
       haudio_out_sai.Init.SynchroExt     = SAI_SYNCEXT_DISABLE;
       haudio_out_sai.Init.Mckdiv         = SAIClockDivider(AudioFreq);
       haudio_out_sai.Init.MonoStereoMode = SAI_STEREOMODE;
       haudio_out_sai.Init.CompandingMode = SAI_NOCOMPANDING;
       haudio_out_sai.Init.TriState       = SAI_OUTPUT_NOTRELEASED;
       */

    /* Configure SAI_Block_x Frame
       Frame Length: 64
       Frame active Length: 32
       FS Definition: Start frame + Channel Side identification
       FS Polarity: FS active Low
       FS Offset: FS asserted one bit before the first bit of slot 0 */
    haudio_out_sai.FrameInit.FrameLength = 128;
    haudio_out_sai.FrameInit.ActiveFrameLength = 64;
    haudio_out_sai.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
    haudio_out_sai.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
    haudio_out_sai.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;

    /* Configure SAI Block_x Slot
       Slot First Bit Offset: 0
       Slot Size  : 16
       Slot Number: 4
       Slot Active: All slot actives */
    haudio_out_sai.SlotInit.FirstBitOffset = 0;
    haudio_out_sai.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
    haudio_out_sai.SlotInit.SlotNumber = 4;
    haudio_out_sai.SlotInit.SlotActive = CODEC_AUDIOFRAME_SLOT_0123;
    /* haudio_out_sai.SlotInit.SlotActive = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1; */

    /* Initializes the SAI peripheral*/
    if (HAL_SAI_Init(&haudio_out_sai) != HAL_OK)
    {
        ret = AUDIO_ERROR;
    }

    /* Enable SAI peripheral to generate MCLK */
    __HAL_SAI_ENABLE(&haudio_out_sai);

    return ret;

}

/**
  * @brief  Deinitializes the Audio Codec audio interface (SAI).
  */
static void SAIx_DeInit(void)
{
    /* printf("sai DeInit\n"); */

    /* Initialize the hAudioOutSai Instance parameter */
    haudio_out_sai.Instance = AUDIO_SAIx;

    /* Disable SAI peripheral */
    __HAL_SAI_DISABLE(&haudio_out_sai);

    HAL_SAI_DeInit(&haudio_out_sai);
}





