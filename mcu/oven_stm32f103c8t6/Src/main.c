#include <stdint.h>
#include <string.h>
#include "main.h"

void SystemClock_Config(void);
void sensor_config();
void hspi_init();
void gpio_init();

SPI_HandleTypeDef SpiHandle;

MAX31865 top_sensor;
MAX31865 bottom_sensor;
NRF24L01 nrf;

static uint8_t display[6] = {0, 0, 0, 0, 0, 0};
static uint8_t display_bits[10] = {
    0B11111100U,
    0B01100000U,
    0B11011010U,
    0B11110010U,
    0B01100110U,
    0B10110110U,
    0B10111110U,
    0B11100000U,
    0B11111110U,
    0B11110110U
};

uint8_t mode; //模式
uint8_t force; //强制开关
uint8_t setting_top_temperature; //设定上管温度
uint8_t setting_bottom_temperature; //设定下管温度
float top_temperature = 0; //当前上管温度
float bottom_temperature = 0; //当前下管温度

static uint16_t adc_change_top_tick = 0;
static uint16_t adc_change_bottom_tick = 0;
static uint16_t relay_tick;
static uint16_t sensor_reset_tick;

extern ADC_HandleTypeDef AdcHandle;

void my_tick()
{
    relay_tick++;
    sensor_reset_tick++;

    //adc显示倒计时
    if (adc_change_top_tick > 0) {
        adc_change_top_tick--;
    }
    if (adc_change_bottom_tick > 0) {
        adc_change_bottom_tick--;
    }

    //继电器控制
    if (relay_tick > 1000) {
        relay_tick = 0;

        //主板闪灯
        /* HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); */

        //温度检查
        /* top_temperature = MAX31865_Read(&top_sensor); */
        /* bottom_temperature = MAX31865_Read(&bottom_sensor); */

        //整理当前数据
        /* uint8_t * buf = nrf.txbuf; */
        /* buf[0] = 1; */
        /* buf[1] = mode; */
        /* buf[2] = setting_top_temperature; */
        /* buf[3] = setting_bottom_temperature; */
        /* buf+=4; */
        /* memcpy(buf, (void*)(&top_temperature), sizeof(float)); */
        /* buf += sizeof(float); */
        /* memcpy(buf, (void*)(&bottom_temperature), sizeof(float)); */

        //发送数据
        /* NRF24L01_TxPacket(&nrf); */

        /* display_format(top_temperature, true, false); */
        /* display_format(bottom_temperature, false, false); */

        /* printf("set %d/%d\n", setting_top_temperature, setting_bottom_temperature); */
        /* printf("temperature %d/%d\n", (uint16_t)(top_temperature*10), (uint16_t)(bottom_temperature*10)); */

        //上加热管
        /* HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, (force&1) || (top_temperature < setting_top_temperature)); */
        //下加热管
        /* HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, (force&2) || (bottom_temperature < setting_bottom_temperature)); */
        //热风烤
        /* HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, (force&4) || ((mode & MODE_FAN) > 0)); */
        //旋转烤架
        /* HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, (force&&8) || ((mode & MODE_ROTATE) > 0)); */
    }

    //每10秒重置一下传感器
    if (sensor_reset_tick > 10000) {
        sensor_reset_tick = 0;

        MAX31856_AutoConvert(&top_sensor);
        MAX31856_AutoConvert(&bottom_sensor);
    }
}

void display_format(uint16_t number, bool top, bool dp)
{
    uint8_t *buf = top ? display : display + 3;

    if (dp) {
        if (top) {
            adc_change_top_tick = 2000;
        } else {
            adc_change_bottom_tick = 2000;
        }
    } else if (top && adc_change_top_tick > 0) {
        //adc显示保护期上
        return;
    } else if (!top && adc_change_bottom_tick > 0) {
        //adc显示保护期下
        return;
    }

    buf[0] = number < 100 ? 0 : display_bits[(number / 100) % 10];

    buf[1] = number < 10 ? 0 : display_bits[(number / 10) % 10];

    buf[2] = display_bits[number % 10];

    if (dp) {
        buf[0] |= 1;
        buf[1] |= 1;
        buf[2] |= 1;
    }
}

void gpio_init() {
    //启动GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    __HAL_RCC_AFIO_CLK_ENABLE();
    /* __HAL_AFIO_REMAP_SWJ_NONJTRST(); */
    __HAL_AFIO_REMAP_SWJ_NOJTAG();
    /* __HAL_AFIO_REMAP_SWJ_DISABLE(); */

    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

    //共阴角
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //输出继电器
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //显示角
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //显示角
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //板子灯
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    //ADC检测
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

#define PIN_CHECK(data, n, pin) (((data) & (n)) ? (pin) : (uint32_t)(pin) << 16U)

//数码管挡描显示
void nixie_tube_display()
{
    uint32_t gpa, gpb;

    uint16_t chars[6] = {GPIO_PIN_11, GPIO_PIN_9, GPIO_PIN_8, GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5};
    uint16_t charN = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;

    for (uint8_t i=0; i<sizeof(chars); i++) {
        gpa = 0;
        gpb = 0;

        gpa |= PIN_CHECK(display[i], 128, GPIO_PIN_8);
        gpa |= PIN_CHECK(display[i], 8, GPIO_PIN_9);
        gpa |= PIN_CHECK(display[i], 16, GPIO_PIN_10);
        gpa |= PIN_CHECK(display[i], 1, GPIO_PIN_11);
        gpa |= PIN_CHECK(display[i], 32, GPIO_PIN_12);
        gpa |= PIN_CHECK(display[i], 2, GPIO_PIN_15);

        gpb |= PIN_CHECK(display[i], 4, GPIO_PIN_3);
        gpb |= PIN_CHECK(display[i], 64, GPIO_PIN_4);

        //扫描字符(共阴)(置0开)
        gpb |= charN & (~chars[i]);
        gpb |= (uint32_t)chars[i] << 16U;

        GPIOA->BSRR = gpa;
        GPIOB->BSRR = gpb;

        /* HAL_Delay(1); */
    }
}

void nrfTxOk(){
    //发送完成后总时切换到接收模式
    NRF24L01_RxPacket(&nrf);
}

void nrfRxMsg(uint8_t *buf) {
    //消息类型1
    if (buf[0] != 1) {
        return;
    }

    //只能在此模下进行遥控
    if (mode != MODE_IDLE) {
        return;
    }

    setting_top_temperature = buf[1];
    setting_bottom_temperature = buf[2];
    force = buf[3];
}

int main(void)
{
    SystemClock_Config();

    HAL_Init();

    gpio_init();

    hspi_init();
    /* nrf.spi.CE_PIN = GPIO_PIN_10; */
    /* nrf.spi.CE_PORT = GPIOB; */
    /* nrf.spi.hspi = &SpiHandle; */
    /* nrf.onTxOk = nrfTxOk; */
    /* nrf.onTxFail = NULL; */
    /* nrf.onRxMsg = nrfRxMsg; */
    /* SPI_Init(&nrf.spi); */
    /* NRF24L01_Init(&nrf); */

    /* UART_init(); */

    /* printf("boot\n"); */

    ADC_init();
    sensor_config();

    while (1)
    {
        //数码管挡描显示
        nixie_tube_display();
    }
}

void hspi_init()
{
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

    HAL_StatusTypeDef re = HAL_SPI_Init(&SpiHandle);

    if (re == HAL_OK) {
        /* SPI block is enabled prior calling SPI transmit/receive functions,
         * in order to get CLK signal properly pulled down. Otherwise,
         * SPI CLK signal is not clean on this board and leads to errors during transfer */
        __HAL_SPI_ENABLE(&SpiHandle);
        __HAL_SPI_DISABLE(&SpiHandle);
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

/* #ifdef SPI_GPIO */
    /* top_sensor->spi.CLK_PIN = GPIO_PIN_5; */
    /* top_sensor->spi.CLK_PORT = GPIOA; */
    /* top_sensor->spi.MISO_PIN = GPIO_PIN_6; */
    /* top_sensor->spi.MISO_PORT = GPIOA; */
    /* top_sensor->spi.MOSI_PIN = GPIO_PIN_7; */
    /* top_sensor->spi.MOSI_PORT = GPIOA; */

    /* bottom_sensor->spi.CLK_PIN = GPIO_PIN_5; */
    /* bottom_sensor->spi.CLK_PORT = GPIOA; */
    /* bottom_sensor->spi.MISO_PIN = GPIO_PIN_6; */
    /* bottom_sensor->spi.MISO_PORT = GPIOA; */
    /* bottom_sensor->spi.MOSI_PIN = GPIO_PIN_7; */
    /* bottom_sensor->spi.MOSI_PORT = GPIOA; */
/* #else */
    top_sensor.spi.hspi = &SpiHandle;
    bottom_sensor.spi.hspi = &SpiHandle;
/* #endif */

    //初始化spi
    SPI_Init(&top_sensor.spi);
    SPI_Init(&bottom_sensor.spi);

    //启动传感器
    MAX31856_AutoConvert(&top_sensor);
    MAX31856_AutoConvert(&bottom_sensor);
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

