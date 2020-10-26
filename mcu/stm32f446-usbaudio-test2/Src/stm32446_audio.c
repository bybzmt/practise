
#include <string.h>
#include "stm32446_audio.h"

#include "stm32f4xx_ll_dma.h"


/*### PLAY ###*/
/* SCK(kHz) = SAI_CK_x/(SAIClockDivider*2*256) */
#define SAIClockDivider(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? 12 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? 2 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? 6 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? 1 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? 3 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 0 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 2 : 1  \



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
void BSP_AUDIO_OUT_ChangeAudioConfig(uint32_t AudioOutOption);


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
  uint8_t ret = AUDIO_OK;

  //todo: 解码芯片启动播放

  /* Initiate a DMA transfer of PCM samples towards the serial audio interface */
  if(ret == AUDIO_OK)
  {
    if (HAL_SAI_Transmit_DMA(&haudio_out_sai, (uint8_t*) pBuffer, DMA_MAX(Size / AUDIODATA_SIZE))!= HAL_OK)
    {
      ret = AUDIO_ERROR;
    }
  }

  return ret;
}

/**
  * @brief  Sends n-Bytes on the SAI interface.
  * @param  pData: pointer on PCM samples buffer
  * @param  Size: number of data to be written
  */
void BSP_AUDIO_OUT_ChangeBuffer(uint16_t *pData, uint16_t Size)
{
   HAL_SAI_Transmit_DMA(&haudio_out_sai, (uint8_t*) pData, Size);
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
  uint8_t ret = AUDIO_OK;

  //todo: 解码芯片暂停

  /* Pause DMA transfer of PCM samples towards the serial audio interface */
  if(ret == AUDIO_OK)
  {
    if (HAL_SAI_DMAPause(&haudio_out_sai)!= HAL_OK)
    {
      ret =  AUDIO_ERROR;
    }
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
  uint8_t ret = AUDIO_OK;

  //todo: 解码芯片暂停

  /* Resume DMA transfer of PCM samples towards the serial audio interface */
  if(ret == AUDIO_OK)
  {
    if (HAL_SAI_DMAResume(&haudio_out_sai)!= HAL_OK)
    {
      ret =  AUDIO_ERROR;
    }
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
  uint8_t ret = AUDIO_OK;

  //todo: 解码芯片停止

  if(ret == AUDIO_OK)
  {
      /* Wait at least 100us */
      //HAL_Delay(2);

    /* Stop DMA transfer of PCM samples towards the serial audio interface */
    if (HAL_SAI_DMAStop(&haudio_out_sai)!= HAL_OK)
    {
      ret = AUDIO_ERROR;
    }
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
  uint8_t ret = AUDIO_OK;

  //todo: 解码芯片静音

  return ret;
}

/**
  * @brief  Switch dynamically (while audio file is being played) the output
  *          target (speaker or headphone).
  * @param  Output: The audio output target: OUTPUT_DEVICE_SPEAKER,
  *         OUTPUT_DEVICE_HEADPHONE or OUTPUT_DEVICE_BOTH
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t BSP_AUDIO_OUT_SetOutputMode(uint8_t Output)
{
  uint8_t ret = AUDIO_OK;

  //todo: 解码芯片设置模式

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
  * @brief  Changes the Audio Out Configuration.
  * @param  AudioOutOption: specifies the audio out new configuration
  *         This parameter can be any value of @ref BSP_Audio_Out_Option
  * @note   This API should be called after the BSP_AUDIO_OUT_Init() to adjust the
  *         audio out configuration.
  */
void BSP_AUDIO_OUT_ChangeAudioConfig(uint32_t AudioOutOption)
{
  /********** Playback Buffer circular/normal mode **********/
  if(AudioOutOption & BSP_AUDIO_OUT_CIRCULARMODE)
  {
    /* Deinitialize the Stream to update DMA mode */
    HAL_DMA_DeInit(haudio_out_sai.hdmatx);

    /* Update the SAI audio Transfer DMA mode */
    haudio_out_sai.hdmatx->Init.Mode = DMA_CIRCULAR;

    /* Configure the DMA Stream with new Transfer DMA mode */
    HAL_DMA_Init(haudio_out_sai.hdmatx);
  }
  else /* BSP_AUDIO_OUT_NORMALMODE */
  {
    /* Deinitialize the Stream to update DMA mode */
    HAL_DMA_DeInit(haudio_out_sai.hdmatx);

    /* Update the SAI audio Transfer DMA mode */
    haudio_out_sai.hdmatx->Init.Mode = DMA_NORMAL;

    /* Configure the DMA Stream with new Transfer DMA mode */
    HAL_DMA_Init(haudio_out_sai.hdmatx);
  }

  /********** Playback Buffer stereo/mono mode **********/
  if(AudioOutOption & BSP_AUDIO_OUT_STEREOMODE)
  {
    /* Disable SAI peripheral to allow access to SAI internal registers */
    __HAL_SAI_DISABLE(&haudio_out_sai);

    /* Update the SAI audio frame slot configuration */
    haudio_out_sai.Init.MonoStereoMode = SAI_STEREOMODE;
    HAL_SAI_Init(&haudio_out_sai);

    /* Enable SAI peripheral to generate MCLK */
    __HAL_SAI_ENABLE(&haudio_out_sai);
  }
  else /* BSP_AUDIO_OUT_MONOMODE */
  {
    /* Disable SAI peripheral to allow access to SAI internal registers */
    __HAL_SAI_DISABLE(&haudio_out_sai);

    /* Update the SAI audio frame slot configuration */
    haudio_out_sai.Init.MonoStereoMode = SAI_MONOMODE;
    HAL_SAI_Init(&haudio_out_sai);

    /* Enable SAI peripheral to generate MCLK */
    __HAL_SAI_ENABLE(&haudio_out_sai);
  }
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
  BSP_AUDIO_OUT_TransferComplete_CallBack();
}

/**
  * @brief  Tx Half Transfer completed callbacks.
  * @param  hsai: SAI handle
  */
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Manage the remaining file size and new address offset: This function
     should be coded by user (its prototype is already declared in stm32469i_discovery_audio.h) */
  BSP_AUDIO_OUT_HalfTransfer_CallBack();
}

/**
  * @brief  SAI error callbacks.
  * @param  hsai: SAI handle
  */
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
  BSP_AUDIO_OUT_Error_CallBack();
}

/**
  * @brief  Manages the DMA full Transfer complete event.
  */
__weak void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
}

/**
  * @brief  Manages the DMA Half Transfer complete event.
  */
__weak void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
}

/**
  * @brief  Manages the DMA FIFO error event.
  */
__weak void BSP_AUDIO_OUT_Error_CallBack(void)
{
}

/**
  * @brief  Initializes BSP_AUDIO_OUT MSP.
  * @param  hsai: SAI handle
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
__weak void BSP_AUDIO_OUT_MspInit(SAI_HandleTypeDef *hsai, void *Params)
{
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
  gpio_init_structure.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOB, &gpio_init_structure);
  //FS
  gpio_init_structure.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOA, &gpio_init_structure);
  //SCK
  gpio_init_structure.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOB, &gpio_init_structure);

  /* Enable the DMA clock */
  AUDIO_SAIx_DMAx_CLK_ENABLE();

  if(hsai->Instance == AUDIO_SAIx)
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
__weak void BSP_AUDIO_OUT_MspDeInit(SAI_HandleTypeDef *hsai, void *Params)
{
  /* SAI DMA IRQ Channel deactivation */
  HAL_NVIC_DisableIRQ(AUDIO_SAIx_DMAx_IRQ);

  if(hsai->Instance == AUDIO_SAIx)
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
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);
  //FS
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
  //SCK
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
  /* Disable SAI clock */
  __HAL_RCC_SAI1_CLK_DISABLE();


  /* GPIO pins clock and DMA clock can be shut down in the applic
     by surcgarging this __weak function */
}

/**
  * @brief  Clock Config.
  * @param  hsai: might be required to set audio peripheral predivider if any.
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @note   This API is called by BSP_AUDIO_OUT_Init() and BSP_AUDIO_OUT_SetFrequency()
  *         Being __weak it can be overwritten by the application
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
__weak void BSP_AUDIO_OUT_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t AudioFreq, void *Params)
{
  RCC_PeriphCLKInitTypeDef rcc_ex_clk_init_struct;

  HAL_RCCEx_GetPeriphCLKConfig(&rcc_ex_clk_init_struct);

  /* Set the PLL configuration according to the audio frequency */
  if((AudioFreq == AUDIO_FREQUENCY_11K) || (AudioFreq == AUDIO_FREQUENCY_22K) || (AudioFreq == AUDIO_FREQUENCY_44K))
  {
    /* Configure PLLI2S prescalers */
    /* PLLI2S_VCO: VCO_429M
    I2S_CLK(first level) = PLLI2S_VCO/PLLI2SQ = 429/2 = 214.5 Mhz
    I2S_CLK_x = I2S_CLK(first level)/PLLI2SDIVQ = 214.5/19 = 11.289 Mhz */
    rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 429;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 2;
    rcc_ex_clk_init_struct.PLLI2SDivQ = 19;

    HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);

  }
  else /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_48K), AUDIO_FREQUENCY_96K */
  {
    /* SAI clock config
    PLLSAI_VCO: VCO_344M
    I2S_CLK(first level) = PLLI2S_VCO/PLLI2SQ = 344/7 = 49.142 Mhz
    I2S_CLK_x = SAI_CLK(first level)/PLLI2SDIVQ = 49.142/1 = 49.142 Mhz */
    rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 344;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 7;
    rcc_ex_clk_init_struct.PLLI2SDivQ = 1;

    HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
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
  /* Initialize the hAudioOutSai Instance parameter */
  haudio_out_sai.Instance = AUDIO_SAIx;

  /* Disable SAI peripheral */
  __HAL_SAI_DISABLE(&haudio_out_sai);

  HAL_SAI_DeInit(&haudio_out_sai);
}

void BSP_AUDIO_OUT_LED_ON(void){};
void BSP_AUDIO_OUT_LED_OFF(void){};






/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
