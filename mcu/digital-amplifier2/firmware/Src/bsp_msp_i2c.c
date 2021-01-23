#include "base.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


static void MY_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);
static void MY_I2C_MspInit(I2C_HandleTypeDef *hi2c);

I2C_HandleTypeDef hi2c1 = {
    .Instance              = I2C1,
    .Init = {
        .ClockSpeed       = 400000,
        .DutyCycle        = I2C_DUTYCYCLE_2,
        .AddressingMode   = I2C_ADDRESSINGMODE_7BIT,
        .DualAddressMode  = I2C_DUALADDRESS_DISABLE,
        .GeneralCallMode  = I2C_GENERALCALL_DISABLE,
        .NoStretchMode    = I2C_NOSTRETCH_DISABLE,
        .OwnAddress1      = 0xFF,
        .OwnAddress2      = 0xFE,
    },
    .MspInitCallback = MY_I2C_MspInit,
    .MspDeInitCallback = MY_I2C_MspDeInit,
};

static DMA_HandleTypeDef hdma_tx = {
    .Instance                 = DMA1_Stream6,
    .Init = {
        .Channel             = DMA_CHANNEL_1,
        .Direction           = DMA_MEMORY_TO_PERIPH,
        .PeriphInc           = DMA_PINC_DISABLE,
        .MemInc              = DMA_MINC_ENABLE,
        .PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
        .MemDataAlignment    = DMA_MDATAALIGN_BYTE,
        .Mode                = DMA_NORMAL,
        .Priority            = DMA_PRIORITY_LOW,
        .FIFOMode            = DMA_FIFOMODE_DISABLE,
        .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
        .MemBurst            = DMA_MBURST_SINGLE,
        .PeriphBurst         = DMA_MBURST_INC4,
    },
};

static DMA_HandleTypeDef hdma_rx = {
    .Instance                 = DMA1_Stream5,
    .Init = {
        .Channel             = DMA_CHANNEL_1,
        .Direction           = DMA_PERIPH_TO_MEMORY,
        .PeriphInc           = DMA_PINC_DISABLE,
        .MemInc              = DMA_MINC_ENABLE,
        .PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
        .MemDataAlignment    = DMA_MDATAALIGN_BYTE,
        .Mode                = DMA_NORMAL,
        .Priority            = DMA_PRIORITY_HIGH,
        .FIFOMode            = DMA_FIFOMODE_DISABLE,
        .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
        .MemBurst            = DMA_MBURST_INC4,
        .PeriphBurst         = DMA_MBURST_INC4,
    },
};

static void MY_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef  gpio;
    gpio.Pin       = GPIO_PIN_6|GPIO_PIN_7;
    gpio.Mode      = GPIO_MODE_AF_OD;
    gpio.Pull      = GPIO_PULLUP;
    gpio.Speed     = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &gpio);

    HAL_DMA_Init(&hdma_tx);
    __HAL_LINKDMA(hi2c, hdmatx, hdma_tx);

    HAL_DMA_Init(&hdma_rx);
    __HAL_LINKDMA(hi2c, hdmarx, hdma_rx);

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    //系统中断优先级
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    HAL_NVIC_SetPriority(PendSV_IRQn, 1, 0);
    HAL_NVIC_SetPriority(SysTick_IRQn, 1, 0);

    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 6, 1);
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 6, 0);
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 6, 2);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 6, 3);

    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
}

static void MY_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    HAL_NVIC_DisableIRQ(DMA1_Stream5_IRQn);
    HAL_NVIC_DisableIRQ(DMA1_Stream6_IRQn);
    HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);

    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
    __HAL_RCC_I2C1_CLK_DISABLE();

    HAL_DMA_DeInit(hi2c->hdmatx);
    HAL_DMA_DeInit(hi2c->hdmarx);
}

/****************** 中断 ***********************/

void I2C1_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c1);
}

void I2C1_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c1);
}

void DMA1_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(hi2c1.hdmatx);
}

void DMA1_Stream5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(hi2c1.hdmarx);
}
