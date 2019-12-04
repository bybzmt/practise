#include "spi.h"
#include "debug.h"

/* transfer state */
static __IO uint32_t wTransferState;

/* SPI */
enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};

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

HAL_StatusTypeDef SPI_Init(SPIx *spi)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = spi->CE_PIN;

    HAL_GPIO_Init(spi->CE_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return HAL_OK;
}


HAL_StatusTypeDef SPI_Transmit(SPIx *spi, uint8_t *tx, size_t len)
{
    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_RESET);

    wTransferState = TRANSFER_WAIT;

    HAL_StatusTypeDef re = HAL_SPI_Transmit_DMA(spi->hspi, tx, len);

    if (re == HAL_OK) {
        while(wTransferState != TRANSFER_WAIT);
    }

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return re != HAL_OK ? re : wTransferState == TRANSFER_COMPLETE ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef SPI_Receive(SPIx *spi, uint8_t *rx, size_t len)
{
    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_RESET);

    wTransferState = TRANSFER_WAIT;
    HAL_StatusTypeDef re = HAL_SPI_Receive_DMA(spi->hspi, rx, len);

    if (re == HAL_OK) {
        while(wTransferState != TRANSFER_WAIT);
    }

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return re != HAL_OK ? re : wTransferState == TRANSFER_COMPLETE ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef SPI_TransmitReceive(SPIx *spi, uint8_t *tx, uint8_t *rx, size_t len)
{
    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_RESET);

    wTransferState = TRANSFER_WAIT;
    HAL_StatusTypeDef re = HAL_SPI_TransmitReceive_DMA(spi->hspi, tx, rx, len);

    if (re == HAL_OK) {
        while(wTransferState != TRANSFER_WAIT);
    }

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return re != HAL_OK ? re : wTransferState == TRANSFER_COMPLETE ? HAL_OK : HAL_ERROR;
}

