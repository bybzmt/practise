#include "spi.h"


static uint8_t spixfer(MAX31865 *gpio, uint8_t x) {
    uint8_t reply = 0;

    for (int i=7; i>=0; i--) {
        reply <<= 1;

        HAL_GPIO_TogglePin(gpio->CLK_PORT, gpio->CLK_PIN);

        HAL_GPIO_WritePin(gpio->MOSI_PORT, gpio->MOSI_PIN, x & (1<<i));

        HAL_GPIO_TogglePin(gpio->CLK_PORT, gpio->CLK_PIN);

        if (HAL_GPIO_ReadPin(gpio->MISO_PORT, gpio->MISO_PIN) == GPIO_PIN_SET) {
            reply |= 1;
        }
    }

    return reply;
}

HAL_StatusTypeDef SPI_Init(SPIx *spi)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin = gpio->CE_PIN;
    HAL_GPIO_Init(gpio->CE_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = gpio->CLK_PIN;
    HAL_GPIO_Init(gpio->CLK_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(gpio->CLK_PORT, gpio->CLK_PIN, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = gpio->MOSI_PIN;
    HAL_GPIO_Init(gpio->MOSI_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(gpio->MOSI_PORT, gpio->MOSI_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin       = gpio->MISO_PIN;
    HAL_GPIO_Init(gpio->MISO_PORT, &GPIO_InitStruct);
}

HAL_StatusTypeDef SPI_Transmit(SPIx *spi, uint8_t *tx, size_t len)
{
    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_RESET);

    for (size_t i=0; i<len; i++) {
        spixfer(spi, tx[i]);
    }

    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_SET);

    return HAL_OK;
}

HAL_StatusTypeDef SPI_Receive(SPIx *spi, uint8_t *rx, size_t len)
{
    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_RESET);

    for (size_t i=0; i<len; i++) {
        rx[i] = spixfer(spi, 0xff);
    }

    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_SET);

    return HAL_OK;
}

HAL_StatusTypeDef SPI_TransmitReceive(SPIx *spi, uint8_t *tx, uint8_t *rx, size_t len)
{
    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_RESET);

    for (size_t i=0; i<len; i++) {
        rx[i] = spixfer(spi, tx[i]);
    }

    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_SET);

    return HAL_OK;
}

