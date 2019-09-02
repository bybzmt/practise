#include "main.h"

static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    UNUSED(huart);

    GPIO_InitTypeDef  GPIO_InitStruct;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Enable USARTx clock */
    __HAL_RCC_USART1_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/**
  * @brief SPI MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - DMA configuration for transmission request by peripheral
  *           - NVIC configuration for DMA interrupt request enable
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    if (hspi->Instance == SPIx)
    {
        printf("HAL_SPI_MspInit\n");

        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        SPIx_SCK_GPIO_CLK_ENABLE();
        SPIx_MISO_GPIO_CLK_ENABLE();
        SPIx_MOSI_GPIO_CLK_ENABLE();
        /* Enable SPI1 clock */
        SPIx_CLK_ENABLE();
        /* Enable DMA clock */
        DMAx_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* GPIO_InitStruct.Pull      = GPIO_NOPULL; */
        GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
        /* GPIO_InitStruct.Mode      = GPIO_MODE_AF_INPUT; */
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;

        GPIO_InitStruct.Pin       = SPIx_MISO_PIN | GPIO_PIN_4;
        HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
        HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin       = SPIx_MOSI_PIN;
        HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);


        /*##-3- Configure the DMA ##################################################*/
        /* Configure the DMA handler for Transmission process */
        hdma_tx.Instance                 = SPIx_TX_DMA_CHANNEL;
        hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_tx.Init.Mode                = DMA_NORMAL;
        hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

        HAL_DMA_Init(&hdma_tx);

        /* Associate the initialized DMA handle to the the SPI handle */
        __HAL_LINKDMA(hspi, hdmatx, hdma_tx);

        /* Configure the DMA handler for Transmission process */
        hdma_rx.Instance                 = SPIx_RX_DMA_CHANNEL;

        hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_rx.Init.Mode                = DMA_NORMAL;
        hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;

        HAL_DMA_Init(&hdma_rx);

        /* Associate the initialized DMA handle to the the SPI handle */
        __HAL_LINKDMA(hspi, hdmarx, hdma_rx);

        /*##-4- Configure the NVIC for DMA #########################################*/
        /* NVIC configuration for DMA transfer complete interrupt (SPI1_TX) */
        HAL_NVIC_SetPriority(SPIx_DMA_TX_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(SPIx_DMA_TX_IRQn);

        /* NVIC configuration for DMA transfer complete interrupt (SPI1_RX) */
        HAL_NVIC_SetPriority(SPIx_DMA_RX_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(SPIx_DMA_RX_IRQn);
    }
}

/**
  * @brief SPI MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPIx)
    {
        /*##-1- Disable peripherals and GPIO Clocks ################################*/
        /* Configure SPI SCK as alternate function  */
        HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
        /* Configure SPI MISO as alternate function  */
        HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
        /* Configure SPI MOSI as alternate function  */
        HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);

        /*##-2- Disable the DMA ####################################################*/
        /* De-Initialize the DMA associated to transmission process */
        HAL_DMA_DeInit(&hdma_tx);
        /* De-Initialize the DMA associated to reception process */
        HAL_DMA_DeInit(&hdma_rx);

        /*##-3- Disable the NVIC for DMA ###########################################*/
        HAL_NVIC_DisableIRQ(SPIx_DMA_TX_IRQn);
        HAL_NVIC_DisableIRQ(SPIx_DMA_RX_IRQn);
    }
}

/**
  * @brief ADC MSP initialization
  *        This function configures the hardware resources used in this example:
  *          - Enable clock of ADC peripheral
  *          - Configure the GPIO associated to the peripheral channels
  *          - Configure the DMA associated to the peripheral
  *          - Configure the NVIC associated to the peripheral interruptions
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    static DMA_HandleTypeDef  DmaHandle;
    RCC_PeriphCLKInitTypeDef  PeriphClkInit;

    /* Enable clock of ADCx peripheral */
    __HAL_RCC_ADC1_CLK_ENABLE();

    /* Configure ADCx clock prescaler */
    /* Caution: On STM32F1, ADC clock frequency max is 14MHz (refer to device   */
    /*          datasheet).                                                     */
    /*          Therefore, ADC clock prescaler must be configured in function   */
    /*          of ADC clock source frequency to remain below this maximum      */
    /*          frequency.                                                      */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* Enable clock of DMA associated to the peripheral */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /*##-3- Configure the DMA ##################################################*/
    /* Configure DMA parameters */
    DmaHandle.Instance = ADCx_DMA;

    DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
    DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
    DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* Transfer from ADC by half-word to match with ADC configuration: ADC resolution 10 or 12 bits */
    DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;   /* Transfer to memory by half-word to match with buffer variable type: half-word */
    DmaHandle.Init.Mode                = DMA_CIRCULAR;              /* DMA in circular mode to match with ADC configuration: DMA continuous requests */
    DmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

    /* Deinitialize  & Initialize the DMA for new transfer */
    HAL_DMA_DeInit(&DmaHandle);
    HAL_DMA_Init(&DmaHandle);

    /* Associate the initialized DMA handle to the ADC handle */
    __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle);

    /*##-4- Configure the NVIC #################################################*/

    /* NVIC configuration for DMA interrupt (transfer completion or error) */
    /* Priority: high-priority */
    HAL_NVIC_SetPriority(ADCx_DMA_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(ADCx_DMA_IRQn);

    /* NVIC configuration for ADC interrupt */
    /* Priority: high-priority */
    HAL_NVIC_SetPriority(ADCx_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADCx_IRQn);
}

/**
  * @brief ADC MSP de-initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable clock of ADC peripheral
  *          - Revert GPIO associated to the peripheral channels to their default state
  *          - Revert DMA associated to the peripheral to its default state
  *          - Revert NVIC associated to the peripheral interruptions to its default state
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
    /*##-1- Reset peripherals ##################################################*/
    ADCx_FORCE_RESET();
    ADCx_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* De-initialize GPIO pin of the selected ADC channel */
    HAL_GPIO_DeInit(ADCx_CHANNELa_GPIO_PORT, ADCx_CHANNELa_PIN);

    /*##-3- Disable the DMA ####################################################*/
    /* De-Initialize the DMA associated to the peripheral */
    if(hadc->DMA_Handle != NULL)
    {
        HAL_DMA_DeInit(hadc->DMA_Handle);
    }

    /*##-4- Disable the NVIC ###################################################*/
    /* Disable the NVIC configuration for DMA interrupt */
    HAL_NVIC_DisableIRQ(ADCx_DMA_IRQn);

    /* Disable the NVIC configuration for ADC interrupt */
    HAL_NVIC_DisableIRQ(ADCx_IRQn);
}
