#include "spi.h"

HAL_StatusTypeDef SPI_Init(SPIx *spi)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = spi->CE_PIN;

    HAL_GPIO_Init(spi->CE_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return 0;
}

HAL_StatusTypeDef SPI_Transmit(SPIx *spi, uint8_t *tx, size_t len)
{
    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_RESET);

    HAL_StatusTypeDef re = HAL_SPI_Transmit(spi->hspi, tx, len, 200);

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return re;
}

HAL_StatusTypeDef SPI_Receive(SPIx *spi, uint8_t *rx, size_t len)
{
    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_RESET);

    HAL_StatusTypeDef re = HAL_SPI_Receive(spi->hspi, rx, len, 200);

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return re;
}

HAL_StatusTypeDef SPI_TransmitReceive(SPIx *spi, uint8_t *tx, uint8_t *rx, size_t len)
{
    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_RESET);

    HAL_StatusTypeDef re = HAL_SPI_TransmitReceive(spi->hspi, tx, rx, len, 200);

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return re;
}

