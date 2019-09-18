#include <stdint.h>
#include "main.h"

void SystemClock_Config(void);
void temps_config();

void gpio_init();

MAX31865 temp1;
MAX31865 temp2;

uint8_t mode;
int16_t setting_top_temperature;
int16_t setting_bottom_temperature;
int16_t top_temperature;
int16_t bottom_temperature;

static uint16_t temp_tick;
static uint16_t relay_tick;
extern AdcHandle;

void my_tick()
{
    //adc扫描
    HAL_ADC_Start(&AdcHandle);

    temp_tick += MY_TICK_BASE;
    if (temp_tick > 500) {
        temp_tick = 0;

        //主板闪灯
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        //温度检查
        top_temperature = MAX31865_readTemp(&temp1) * 10;
        bottom_temperature = MAX31865_readTemp(&temp2) * 10;
    }

    //继电器控制
    relay_tick += MY_TICK_BASE;
    if (relay_tick > 1000) {
        relay_tick = 0;

        //上加热管
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, top_temperature < setting_top_temperature);
        //下加热管
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, bottom_temperature < setting_bottom_temperature);
        //热风烤
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (mode & MODE_FAN) > 0);
        //旋转烤架
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, (mode & MODE_ROTATE) > 0);
    }
}

//数码管挡描显示
void nixie_tube_display()
{
    HAL_Delay(1);
}

int main(void)
{
    SystemClock_Config();

    HAL_Init();

    printf("boot\n");

    gpio_init();
    ADC_init();

    temps_config();
    MAX31865_init(&temp1);
    MAX31865_init(&temp2);

    printf("book ok\n");

    while (1)
    {
        //数码管挡描显示
        nixie_tube_display();
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

    //输出继电器
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //板子灯
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Configure GPIO pin of the selected ADC channel */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
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

    //启动GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
}






