#include <string.h>
#include "adc.h"
#include "debug.h"
#include "main.h"

/* ADC handler declaration */
ADC_HandleTypeDef    AdcHandle;

/* Variable containing ADC conversions results */
__IO uint16_t   aADCxConvertedValues[ADCCONVERTEDVALUES_BUFFER_SIZE];
uint8_t aAdc_i = 0;

/* Variable to report ADC sequencer status */
uint8_t         ubSequenceCompleted = RESET;

/**
  * @brief  ADC configuration
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
    ADC_ChannelConfTypeDef   sConfig;

    /* Configuration of ADCx init structure: ADC parameters and regular group */
    AdcHandle.Instance = ADCx;

    AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    AdcHandle.Init.ScanConvMode          = ADC_SCAN_ENABLE;               /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
    AdcHandle.Init.ContinuousConvMode    = ENABLE;                       /* Continuous mode disabled to have only 1 rank converted at each conversion trig, and because discontinuous mode is enabled */
    AdcHandle.Init.NbrOfConversion       = 3;                             /* Sequencer of regular group will convert the 3 first ranks: rank1, rank2, rank3 */
    AdcHandle.Init.DiscontinuousConvMode = ENABLE;                        /* Sequencer of regular group will convert the sequence in several sub-divided sequences */
    AdcHandle.Init.NbrOfDiscConversion   = 1;                             /* Sequencer of regular group will convert ranks one by one, at each conversion trig */
    AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Trig of conversion start done manually by software, without external event */

    if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
    {
        Error_Handler("HAL_ADC_Init");
    }

    /* Configuration of channel on ADCx regular group on sequencer rank 1 */
    /* Note: Considering IT occurring after each ADC conversion (IT by DMA end  */
    /*       of transfer), select sampling time and ADC clock with sufficient   */
    /*       duration to not create an overhead situation in IRQHandler.        */
    /* Note: Set long sampling time due to internal channels (VrefInt,          */
    /*       temperature sensor) constraints. Refer to device datasheet for     */
    /*       min/typ/max values.                                                */
    sConfig.Channel      = ADC_CHANNEL_0;
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;

    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        Error_Handler("HAL_ADC_ConfigChannel");
    }

    /* Configuration of channel on ADCx regular group on sequencer rank 2 */
    /* Replicate previous rank settings, change only channel and rank */
    sConfig.Channel      = ADC_CHANNEL_1;
    sConfig.Rank         = ADC_REGULAR_RANK_2;

    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        Error_Handler("HAL_ADC_ConfigChannel");
    }

    sConfig.Channel      = ADC_CHANNEL_2;
    sConfig.Rank         = ADC_REGULAR_RANK_3;

    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        Error_Handler("HAL_ADC_ConfigChannel");
    }
}

void ADC_init(void)
{
    ADC_Config();

    /* Run the ADC calibration */
    if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
    {
        Error_Handler("HAL_ADCEx_Calibration_Start");
    }

    /* Start ADC conversion on regular group with transfer by DMA */
    if (HAL_ADC_Start_DMA(&AdcHandle,
                        (uint32_t *)aADCxConvertedValues,
                        ADCCONVERTEDVALUES_BUFFER_SIZE
                       ) != HAL_OK)
    {
        Error_Handler("HAL_ADC_Start_DMA");
    }
}

void ADC_Read(uint16_t *a, uint16_t *b)
{
    HAL_ADC_Start(&AdcHandle);
    HAL_ADC_PollForConversion(&AdcHandle, 10);

    *a = aADCxConvertedValues[0];
    *b = aADCxConvertedValues[1];
}

/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  AdcHandle : AdcHandle handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle)
{
    UNUSED(AdcHandle);

    adc3shot[0] = aADCxConvertedValues[0];
    adc3shot[1] = aADCxConvertedValues[1];
    adc3shot[2] = aADCxConvertedValues[2];
}


void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    UNUSED(hadc);

    ubSequenceCompleted = SET;
}

