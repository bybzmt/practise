#include "spi.h"
#include "debug.h"

/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

/* transfer state */
__IO uint32_t wTransferState;

/* SPI1 init function */
void MX_SPI1_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin       = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    SpiHandle.Instance = SPI1;
    SpiHandle.Init.Mode = SPI_MODE_MASTER;
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
    SpiHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
    SpiHandle.Init.NSS = SPI_NSS_SOFT;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial = 7;

    if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        Error_Handler("HAL_SPI_Init");
    }

    /* SPI block is enabled prior calling SPI transmit/receive functions, in order to get CLK signal properly pulled down.
       Otherwise, SPI CLK signal is not clean on this board and leads to errors during transfer */
    /* __HAL_SPI_ENABLE(&SpiHandle); */
    /* __HAL_SPI_DISABLE(&SpiHandle); */
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    UNUSED(hspi);

    wTransferState = TRANSFER_COMPLETE;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    UNUSED(hspi);

    wTransferState = TRANSFER_COMPLETE;
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    UNUSED(hspi);

    wTransferState = TRANSFER_COMPLETE;
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    UNUSED(hspi);

    Error_Handler("HAL_SPI_ErrorCallback");

    wTransferState = TRANSFER_ERROR;
}
