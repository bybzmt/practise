#include "spi.h"


static uint8_t spixfer(SPIx *spi, uint8_t x) {
    uint8_t reply = 0;

    __disable_irq();

    for (int i=7; i>=0; i--) {
        reply <<= 1;

        HAL_GPIO_TogglePin(spi->CLK_PORT, spi->CLK_PIN);

        HAL_GPIO_WritePin(spi->MOSI_PORT, spi->MOSI_PIN, x & (1<<i));

        HAL_GPIO_TogglePin(spi->CLK_PORT, spi->CLK_PIN);

        if (HAL_GPIO_ReadPin(spi->MISO_PORT, spi->MISO_PIN) == GPIO_PIN_SET) {
            reply |= 1;
        }
    }

    __enable_irq();

    return reply;
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

    GPIO_InitStruct.Pin = spi->CLK_PIN;
    HAL_GPIO_Init(spi->CLK_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(spi->CLK_PORT, spi->CLK_PIN, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = spi->MOSI_PIN;
    HAL_GPIO_Init(spi->MOSI_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(spi->MOSI_PORT, spi->MOSI_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin       = spi->MISO_PIN;
    HAL_GPIO_Init(spi->MISO_PORT, &GPIO_InitStruct);

    return HAL_OK;
}

HAL_StatusTypeDef SPI_Transmit(SPIx *spi, uint8_t *tx, size_t len)
{
    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_RESET);

    for (size_t i=0; i<len; i++) {
        spixfer(spi, tx[i]);
    }

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return HAL_OK;
}

HAL_StatusTypeDef SPI_Receive(SPIx *spi, uint8_t *rx, size_t len)
{
    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_RESET);

    for (size_t i=0; i<len; i++) {
        rx[i] = spixfer(spi, 0xff);
    }

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return HAL_OK;
}

HAL_StatusTypeDef SPI_TransmitReceive(SPIx *spi, uint8_t *tx, uint8_t *rx, size_t len)
{
    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_RESET);

    for (size_t i=0; i<len; i++) {
        rx[i] = spixfer(spi, tx[i]);
    }

    HAL_GPIO_WritePin(spi->CE_PORT, spi->CE_PIN, GPIO_PIN_SET);

    return HAL_OK;
}

