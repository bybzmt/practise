#include "base.h"
#include <stdint.h>

void msp_clock_usb_config(void)
{
    RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
    HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
    RCC_ExCLKInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLSAIP;
    RCC_ExCLKInitStruct.PLLSAI.PLLSAIM = 8;
    RCC_ExCLKInitStruct.PLLSAI.PLLSAIN = 384;
    RCC_ExCLKInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
    HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
}

void msp_clock_spdif_config(void)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    HAL_RCCEx_GetPeriphCLKConfig(&PeriphClkInitStruct);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPDIFRX;
    /* PeriphClkInitStruct.SpdifClockSelection = RCC_SPDIFRXCLKSOURCE_PLLR; */
    PeriphClkInitStruct.SpdifClockSelection = RCC_SPDIFRXCLKSOURCE_PLLI2SP;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        printf("spdif clock error\n");
    }
}

void msp_clock_sai_config(uint32_t AudioFreq)
{
    RCC_PeriphCLKInitTypeDef periphCLK;

    HAL_RCCEx_GetPeriphCLKConfig(&periphCLK);
    periphCLK.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    periphCLK.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLI2S;

    /* Set the PLL configuration according to the audio frequency */
    if (
            (AudioFreq == SAI_AUDIO_FREQUENCY_11K) ||
            (AudioFreq == SAI_AUDIO_FREQUENCY_22K) ||
            (AudioFreq == SAI_AUDIO_FREQUENCY_44K))
    {
        /* Configure PLLSAI prescalers */
        /* PLLSAI_VCO: VCO_429M
           SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
           SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 214.5/19 = 11.289 Mhz */
        periphCLK.PLLI2S.PLLI2SM = 8;
        periphCLK.PLLI2S.PLLI2SN = 429;
        periphCLK.PLLI2S.PLLI2SQ = 2;
        periphCLK.PLLI2S.PLLI2SR = 18;
        periphCLK.PLLI2S.PLLI2SP = 2;
        periphCLK.PLLI2SDivQ = 19;

        /* periphCLK.PLLSAI.PLLSAIM = 8; */
        /* periphCLK.PLLSAI.PLLSAIN = 429; */
        /* periphCLK.PLLSAI.PLLSAIQ = 2; */
        /* periphCLK.PLLSAI.PLLSAIP = 2; */
        /* periphCLK.PLLSAIDivQ = 19; */
    } else {
        /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_48K), AUDIO_FREQUENCY_96K */
        /* SAI clock config
           PLLSAI_VCO: VCO_344M
           SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 344/7 = 49.142 Mhz
           SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 49.142/1 = 49.142 Mhz */
        periphCLK.PLLI2S.PLLI2SM = 8;
        periphCLK.PLLI2S.PLLI2SN = 344;
        periphCLK.PLLI2S.PLLI2SQ = 7;
        periphCLK.PLLI2S.PLLI2SP = 2;
        periphCLK.PLLI2S.PLLI2SR = 7;
        periphCLK.PLLI2SDivQ = 1;

        /* periphCLK.PLLSAI.PLLSAIM = 8; */
        /* periphCLK.PLLSAI.PLLSAIN = 344; */
        /* periphCLK.PLLSAI.PLLSAIQ = 7; */
        /* periphCLK.PLLSAI.PLLSAIP = 7; */
        /* periphCLK.PLLSAIDivQ = 1; */
    }

    if (HAL_RCCEx_PeriphCLKConfig(&periphCLK) != HAL_OK)
    {
        printf("Periph clock error\n");
    }

    return;

    /* 384/192 */
    periphCLK.PeriphClockSelection = RCC_PERIPHCLK_I2S_APB2;
    /* periphCLK.PeriphClockSelection = RCC_PERIPHCLK_PLLI2S; */
    periphCLK.I2sApb2ClockSelection = RCC_I2SAPB2CLKSOURCE_PLLI2S;

    if (HAL_RCCEx_PeriphCLKConfig(&periphCLK) != HAL_OK)
    {
        printf("Periph clock error\n");
    }
}

