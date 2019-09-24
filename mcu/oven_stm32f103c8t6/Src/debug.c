#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "stm32f1xx_hal.h"
#include "debug.h"

int _read(int fd, uint8_t *buf, size_t count);
int _write(int fd, uint8_t *buf, size_t count);

static UART_HandleTypeDef UartHandle;

int _read (int fd, uint8_t *buf, size_t count)
{
    UNUSED(fd);

    HAL_UART_Receive(&UartHandle, buf, count, 0xFFFF);

    return count;
}

int _write (int fd, uint8_t *buf, size_t count)
{
    UNUSED(fd);

    HAL_UART_Transmit(&UartHandle, buf, count, 0xFFFF);

    return count;
}

void UART_init()
{
    UartHandle.Instance        = USART1;

    /* UartHandle.Init.BaudRate   = 9600; */
    UartHandle.Init.BaudRate   = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_ODD;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;

    if (HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        /* Initialization Error */
        while(1);
    }
}

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

void Error_Handler(char *msg)
{
    while(1)
    {
        printf("Error_Handler: %s\n", msg);
        HAL_Delay(1000);
    }
}

void printHex(const char *msg, const uint8_t *data, size_t len)
{
    uint8_t hex[40];
    size_t max = sizeof(hex)-1;

    max = len > (max/2) ? (max/2) : len;

    hex[max*2] = 0;

    byte2hex(data, hex, max);

    printf("%s: %s\n", msg, hex);
}

void byte2hex(const uint8_t *source, uint8_t *dest, size_t len)
{
    uint8_t highByte, lowByte;

    for (size_t i = 0; i < len; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;

        highByte += 0x30;

        if (highByte > 0x39)
                dest[i * 2] = highByte + 0x07;
        else
                dest[i * 2] = highByte;

        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
}

void hex2byte(const uint8_t *source, uint8_t *dest, size_t len)
{
    uint8_t highByte, lowByte;

    for (size_t i = 0; i < len; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);

        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;

        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;

        dest[i / 2] = (highByte << 4) | lowByte;
    }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif
