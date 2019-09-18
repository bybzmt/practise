#include <stdint.h>
#include "main.h"

void SystemClock_Config(void);
void temps_config();

void gpio_init();

MAX31865_GPIO temp1;
MAX31865_GPIO temp2;

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();


    printf("boot\n");

    gpio_init();
    ADC_init();

    temps_config();
    MAX31865_init(&temp1);
    MAX31865_init(&temp2);

    printf("book ok\n");

    while (1)
    {
        //主板闪灯
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        bool input1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
        bool input2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);

        uint16_t max_temp = 0;

        if (input2) {
            max_temp = 250 * 10;
        }

        if (input1) {
            max_temp = 30 * 10;
        }

        printf("max_temp:%d\n", max_temp);

        uint16_t now_a=0, now_b=0;
        ADC_Read(&now_a, &now_b);

        printf("adc top:%d down:%d\n", now_a, now_b);

        //3700是调温时250度的adc值
        uint16_t need_t1 = (uint32_t)now_a * 2500 / 3700;
        uint16_t need_t2 = (uint32_t)now_b * 2500 / 3700;

        if (need_t1 > max_temp) {
            need_t1 = max_temp;
        }
        if (need_t2 > max_temp) {
            need_t2 = max_temp;
        }

        uint16_t now_t1 = MAX31865_readTemp(&temp1) * 10;
        uint16_t now_t2 = MAX31865_readTemp(&temp2) * 10;

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, now_t1 < need_t1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, now_t2 < need_t2);

        printf("temp top:%d/%d down:%d/%d\n", now_t1, need_t1, now_t2, need_t2);

        HAL_Delay(1000);
    }
}

void temps_config() {
    temp1.Is3Wire = true;
    temp1.CE_PIN = GPIO_PIN_3;
    temp1.CE_PORT = GPIOA;

    temp2.Is3Wire = true;
    temp2.CE_PIN = GPIO_PIN_4;
    temp2.CE_PORT = GPIOA;

    temp1.CLK_PIN = GPIO_PIN_5;
    temp1.CLK_PORT = GPIOA;
    temp1.MISO_PIN = GPIO_PIN_6;
    temp1.MISO_PORT = GPIOA;
    temp1.MOSI_PIN = GPIO_PIN_7;
    temp1.MOSI_PORT = GPIOA;

    temp2.CLK_PIN = GPIO_PIN_5;
    temp2.CLK_PORT = GPIOA;
    temp2.MISO_PIN = GPIO_PIN_6;
    temp2.MISO_PORT = GPIOA;
    temp2.MOSI_PIN = GPIO_PIN_7;
    temp2.MOSI_PORT = GPIOA;
}

void gpio_init() {
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    //输入 模式选择
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //输出继电器
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //板子灯
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Configure GPIO pin of the selected ADC channel */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 24000000
  *            HCLK(Hz)                       = 24000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 2
  *            PLLMUL                         = 6
  *            Flash Latency(WS)              = 0
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef clkinitstruct = {0};
    RCC_OscInitTypeDef oscinitstruct = {0};

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
    oscinitstruct.HSEState        = RCC_HSE_ON;
    oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV2;
    oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
    oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
    oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL6;
    if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
    {
        /* Initialization Error */
        while(1);
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
       clocks dividers */
    clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
    clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_0)!= HAL_OK)
    {
        /* Initialization Error */
        while(1);
    }
}






