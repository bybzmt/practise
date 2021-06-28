#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "stm32f4xx_hal.h"

static void debug_UART_InitOnce(void);
static void debug_UART_MspInit(UART_HandleTypeDef *huart);
static void debug_UART_MspDeInit(UART_HandleTypeDef *huart);

int _write(int fd, uint8_t *buf, size_t count);

static UART_HandleTypeDef debug_UartHandle = {
    .Instance = USART3,
    .Init = {
        .BaudRate   = 9600,
        .WordLength = UART_WORDLENGTH_8B,
        .StopBits   = UART_STOPBITS_1,
        .Parity     = UART_PARITY_NONE,
        .HwFlowCtl  = UART_HWCONTROL_NONE,
        .Mode       = UART_MODE_TX,
        .OverSampling = UART_OVERSAMPLING_16,
    },
    .MspInitCallback = debug_UART_MspInit,
    .MspDeInitCallback = debug_UART_MspDeInit,
};

static void debug_UART_InitOnce(void)
{
    static uint8_t init_once = true;

    if (init_once) {
        init_once = false;

        HAL_UART_DeInit(&debug_UartHandle);

        if (HAL_UART_Init(&debug_UartHandle) != HAL_OK)
        {
            while(1);
        }
    }
}

int _write (int fd, uint8_t *buf, size_t count)
{
    UNUSED(fd);

    debug_UART_InitOnce();

    HAL_UART_Transmit(&debug_UartHandle, buf, count, 0xFFFF);

    return count;
}

static void debug_UART_MspInit(UART_HandleTypeDef *huart)
{
    UNUSED(huart);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_USART3_CLK_ENABLE();

    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void debug_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_USART1_RELEASE_RESET();
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10);
}

