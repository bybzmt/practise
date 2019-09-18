#include <stdint.h>
#include "main.h"

void SystemClock_Config(void);
void sensor_config();

void gpio_init();

SPI_HandleTypeDef SpiHandle;

MAX31865 top_sensor;
MAX31865 bottom_sensor;

uint8_t mode;
int16_t setting_top_temperature;
int16_t setting_bottom_temperature;
int16_t top_temperature;
int16_t bottom_temperature;

static uint16_t temp_tick;
static uint16_t relay_tick;
static uint16_t sensor_reset_tick;

extern ADC_HandleTypeDef AdcHandle;

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
        top_temperature = MAX31865_Read(&top_sensor) * 10;
        bottom_temperature = MAX31865_Read(&bottom_sensor) * 10;
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

    //每10秒重置一下传感器
    sensor_reset_tick += MY_TICK_BASE;
    if (sensor_reset_tick > 10000) {
        sensor_reset_tick = 0;

        MAX31856_AutoConvert(&top_sensor);
        MAX31856_AutoConvert(&bottom_sensor);
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

    sensor_config();

    printf("book ok\n");

    while (1)
    {
        //数码管挡描显示
        nixie_tube_display();
    }
}

void sensor_config()
{
    top_sensor.spi.CE_PIN = GPIO_PIN_3;
    top_sensor.spi.CE_PORT = GPIOA;
    top_sensor.Is3Wire = true;

    bottom_sensor.spi.CE_PIN = GPIO_PIN_4;
    bottom_sensor.spi.CE_PORT = GPIOA;
    bottom_sensor.Is3Wire = true;

#ifdef SPI_GPIO
    top_sensor->spi.CLK_PIN = GPIO_PIN_5;
    top_sensor->spi.CLK_PORT = GPIOA;
    top_sensor->spi.MISO_PIN = GPIO_PIN_6;
    top_sensor->spi.MISO_PORT = GPIOA;
    top_sensor->spi.MOSI_PIN = GPIO_PIN_7;
    top_sensor->spi.MOSI_PORT = GPIOA;

    bottom_sensor->spi.CLK_PIN = GPIO_PIN_5;
    bottom_sensor->spi.CLK_PORT = GPIOA;
    bottom_sensor->spi.MISO_PIN = GPIO_PIN_6;
    bottom_sensor->spi.MISO_PORT = GPIOA;
    bottom_sensor->spi.MOSI_PIN = GPIO_PIN_7;
    bottom_sensor->spi.MOSI_PORT = GPIOA;
#else
    SpiHandle.Instance = SPI1;
    SpiHandle.Init.Mode = SPI_MODE_MASTER;
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
    SpiHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
    SpiHandle.Init.NSS = SPI_NSS_SOFT;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial = 7;

    top_sensor.spi.hspi = &SpiHandle;
    bottom_sensor.spi.hspi = &SpiHandle;
#endif

    //初始化spi
    SPI_Init(&top_sensor.spi);
    SPI_Init(&bottom_sensor.spi);

    //启动传感器
    MAX31856_AutoConvert(&top_sensor);
    MAX31856_AutoConvert(&bottom_sensor);
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

