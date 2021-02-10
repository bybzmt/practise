#include "base.h"
#include <string.h>
#include <stdio.h>
#include "bsp_audio.h"
#include "stm32f4xx_ll_dma.h"

static void BSP_SAI_ClockConfig(uint32_t AudioFreq);
static void BSP_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void BSP_SAI_ErrorCallback(SAI_HandleTypeDef *hsai);

static void spdifClock_Config(void);
static void my_spdif_rx_cplt(SPDIFRX_HandleTypeDef *hspdif);

uint8_t BSP_AUDIO_Init(uint8_t mode, uint8_t Volume, uint32_t AudioFreq)
{
    printf("audio init %ld\n", AudioFreq);

    /* BSP_AUDIO_DeInit(); */

    BSP_SAI_ClockConfig(AudioFreq);

    __HAL_SAI_DISABLE(&hsai_out);
    HAL_SAI_DeInit(&hsai_out);

    hsai_out.Init.AudioFrequency = AudioFreq;

    if (HAL_SAI_Init(&hsai_out) != HAL_OK)
    {
        return AUDIO_ERROR;
    }

    HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_TX_COMPLETE_CB_ID, BSP_SAI_TxCpltCallback);
    HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_TX_HALFCOMPLETE_CB_ID, BSP_SAI_TxHalfCpltCallback);
    HAL_SAI_RegisterCallback(&hsai_out, HAL_SAI_ERROR_CB_ID, BSP_SAI_ErrorCallback);

    __HAL_SAI_ENABLE(&hsai_out);

    bsp_tas6424_play(AudioFreq);

    return AUDIO_OK;
}

uint8_t BSP_AUDIO_Play(uint8_t* pBuffer, uint32_t Size)
{
    HAL_SAI_DMAStop(&hsai_out);

    HAL_StatusTypeDef ret = HAL_SAI_Transmit_DMA(&hsai_out, pBuffer, Size);
    if (ret!= HAL_OK) {
        printf("play err:%d\n", ret);
        return AUDIO_ERROR;
    }

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
    printf("volume %d\n", Volume);
    bsp_tas6424_mute(Volume);
    return AUDIO_OK;
}

uint8_t BSP_AUDIO_OUT_SetMute(uint32_t Cmd)
{
    printf("mute %d\n", (uint8_t)Cmd);
    bsp_tas6424_mute((uint8_t)Cmd);
    return AUDIO_OK;
}

void BSP_AUDIO_DeInit(void)
{
    printf("audio deInit\n");

    __HAL_SAI_DISABLE(&hsai_out);
    HAL_SAI_DeInit(&hsai_out);
}

uint32_t BSP_AUDIO_OUT_GetRemainingDataSize(void)
{
    return LL_DMA_ReadReg(DMA2_Stream5, NDTR) & 0xFFFF;
}

static void BSP_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    BSP_AUDIO_OUT_TransferComplete_CallBack();
}

static void BSP_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    BSP_AUDIO_OUT_HalfTransfer_CallBack();
}

static void BSP_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    uint32_t num = HAL_SAI_GetError(hsai);
    printf("sai err:%ld\n", num);
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

void my_spdifrx_stop(void)
{
    HAL_SPDIFRX_DeInit(&SpdifrxHandle);
}

void my_spdifrx_start(void)
{
    printf("spdifrx start\n");

    size_t size = 1024 * 8;
    uint32_t *buf = (uint32_t*)malloc(size);

    HAL_SPDIFRX_DeInit(&SpdifrxHandle);
    __HAL_SPDIFRX_IDLE(&SpdifrxHandle);

    spdifClock_Config();

    if (HAL_SPDIFRX_Init(&SpdifrxHandle) != HAL_OK)
    {
        printf("spdifrx init err\n");
        return;
    }

    HAL_SPDIFRX_RegisterCallback(&SpdifrxHandle, HAL_SPDIFRX_RX_CPLT_CB_ID, my_spdif_rx_cplt);
    HAL_SPDIFRX_RegisterCallback(&SpdifrxHandle, HAL_SPDIFRX_CX_CPLT_CB_ID, my_spdif_rx_cplt);
    HAL_SPDIFRX_RegisterCallback(&SpdifrxHandle, HAL_SPDIFRX_ERROR_CB_ID, my_spdif_rx_cplt);

    /* 配置SAI */
    /* __HAL_SAI_DISABLE(&hsai_out); */
    /* HAL_SAI_DeInit(&hsai_out); */

    /* hsai_out.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K; */

    /* HAL_SAI_Init(&hsai_out); */
    /* __HAL_SAI_ENABLE(&hsai_out); */

    /* HAL_SAI_Transmit_DMA(&hsai_out, buf, size); */
    /* HAL_SAI_DMAPause(&hsai_out); */
    /* bsp_tas6424_play(SAI_AUDIO_FREQUENCY_48K); */

    /* 启动spdif */
    HAL_SPDIFRX_ReceiveDataFlow_DMA(&SpdifrxHandle, buf, size/4);

    HAL_StatusTypeDef ret;
    ret = HAL_SPDIFRX_ReceiveDataFlow(&SpdifrxHandle, buf, size/4, 2000);
    printf("ret: %d\n", ret);


    if (SpdifrxHandle.ErrorCode != HAL_SPDIFRX_ERROR_NONE)
    {
        printf("spdifrx start err\n");
        return;
    }

    printf("spdifrx over\n");
}

static void spdifClock_Config(void)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPDIFRX|RCC_PERIPHCLK_SAI1;
    /* PeriphClkInitStruct.SpdifClockSelection = RCC_SPDIFRXCLKSOURCE_PLLI2SP; */
    PeriphClkInitStruct.SpdifClockSelection = RCC_SPDIFRXCLKSOURCE_PLLR;
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLI2S;
    PeriphClkInitStruct.PLLI2S.PLLI2SM = 8;
    PeriphClkInitStruct.PLLI2S.PLLI2SN = 344;
    PeriphClkInitStruct.PLLI2S.PLLI2SQ = 7;
    PeriphClkInitStruct.PLLI2S.PLLI2SP = RCC_PLLI2SP_DIV2;
    PeriphClkInitStruct.PLLI2SDivQ = 1;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        printf("spdif clock error\n");
    }
}

static void my_spdif_rx_cplt(SPDIFRX_HandleTypeDef *hspdif)
{
    static bool once = true;
    if (once) {
        printf("once\n");
        /* HAL_SAI_DMAResume(&hsai_out); */
        once = false;
    }
}
