#ifndef __SPI_H
#define __SPI_H

#include "stm32f1xx_hal.h"

/* Definition for SPIx clock resources */
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT               GPIOA
#define SPIx_MISO_PIN                    GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT              GPIOA
#define SPIx_MOSI_PIN                    GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT              GPIOA

/* Definition for SPIx's DMA */
#define SPIx_TX_DMA_CHANNEL              DMA1_Channel3
#define SPIx_RX_DMA_CHANNEL              DMA1_Channel2

/* Definition for SPIx's NVIC */
#define SPIx_DMA_TX_IRQn                 DMA1_Channel3_IRQn
#define SPIx_DMA_RX_IRQn                 DMA1_Channel2_IRQn

#define SPIx_DMA_TX_IRQHandler           DMA1_Channel3_IRQHandler
#define SPIx_DMA_RX_IRQHandler           DMA1_Channel2_IRQHandler

#ifdef SPIx_GPIO
    typedef struct {
        GPIO_TypeDef *CE_PORT;
        uint16_t CE_PIN;

        GPIO_TypeDef *CLK_PORT;
        uint16_t CLK_PIN;

        GPIO_TypeDef *MOSI_PORT;
        uint16_t MOSI_PIN;

        GPIO_TypeDef *MISO_PORT;
        uint16_t MISO_PIN;
    } SPIx;
#else
    typedef struct {
        GPIO_TypeDef *CE_PORT;
        uint16_t CE_PIN;

        SPI_HandleTypeDef *hspi;
    } SPIx;
#endif

HAL_StatusTypeDef SPI_Init(SPIx *spi);
HAL_StatusTypeDef SPI_Transmit(SPIx *spi, uint8_t *tx, size_t len);
HAL_StatusTypeDef SPI_Receive(SPIx *spi, uint8_t *rx, size_t len);
HAL_StatusTypeDef SPI_TransmitReceive(SPIx *spi, uint8_t *tx, uint8_t *rx, size_t len);

#endif /* __MAIN_H */
