#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

void SystemClock_Config(void);
void sensor_config();
void hspi_init();
void gpio_init();

SPI_HandleTypeDef SpiHandle;

MAX31865 top_sensor;
MAX31865 bottom_sensor;
max7219 disp;

uint16_t __IO adc3shot[3*(ADC_COUNT+1)];
static uint8_t adc_count_tick = 0;

static uint8_t switch_status = 0;
static __IO uint16_t tick = 0;


static uint8_t setting_mode; //模式
static uint8_t setting_top_temperature; //设定上管温度
static uint8_t setting_bottom_temperature; //设定下管温度
static float top_temperature = 0; //当前上管温度
static float bottom_temperature = 0; //当前下管温度

extern ADC_HandleTypeDef AdcHandle;

void my_tick()
{
    tick++;
}

void display_temp()
{
    uint8_t tmp;

    if (setting_mode == MODE_IDLE) {
        for (uint8_t t=1; t<7; t++) {
            max7219_show(&disp, t, 10);
        }
        return;
    }

    if (top_temperature == top_temperature) {
        tmp = setting_top_temperature < 100 ? 0xf : (setting_top_temperature / 100) % 10;
        max7219_show(&disp, 1, tmp);

        tmp = setting_top_temperature < 10 ? 0xf : (setting_top_temperature / 10) % 10;
        max7219_show(&disp, 5, tmp);

        tmp = setting_top_temperature % 10;
        max7219_show(&disp, 3, tmp);
    } else {
        max7219_show(&disp, 1, 0xb);
        max7219_show(&disp, 5, 0xf);
        max7219_show(&disp, 3, 1);
    }

    if (bottom_temperature != bottom_temperature) {
        max7219_show(&disp, 4, 0xb);
        max7219_show(&disp, 6, 0xf);
        max7219_show(&disp, 2, 1);
        return;
    }

    tmp = setting_bottom_temperature < 100 ? 0xf : (setting_bottom_temperature / 100) % 10;
    max7219_show(&disp, 4, tmp);

    tmp = setting_bottom_temperature < 10 ? 0xf : (setting_bottom_temperature / 10) % 10;
    max7219_show(&disp, 6, tmp);

    tmp = setting_bottom_temperature % 10;
    max7219_show(&disp, 2, tmp);
}

void gpio_init() {
    //启动GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;

    //输出继电器
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //ADC检测
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void adc_move() {
    adc_count_tick = (adc_count_tick+3) % (ADC_COUNT*3) + 3;

    __disable_irq();

    adc3shot[adc_count_tick] = adc3shot[0];
    adc3shot[adc_count_tick+1] = adc3shot[1];
    adc3shot[adc_count_tick+2] = adc3shot[2];

    __enable_irq();
}

uint16_t adc_correction(uint8_t i) {
    int32_t out = 0;
    uint16_t a = 0, b=0;
    bool c = 0;

    a = adc3shot[i+3];
    b = a;
    out += a;

    for (int8_t x=1; x<ADC_COUNT; x++) {
        a = adc3shot[(x*3)+i+3];
        out += a;
        if (a != b) {
            c = 1;
        }
    }

    out = out / ADC_COUNT;

    if (c) {
        if (i==1) {
            if (abs(out - setting_top_temperature) < 2) {
                return setting_top_temperature;
            }
        }
        if (i==2) {
            if (abs(out - setting_bottom_temperature) < 2) {
                return setting_bottom_temperature;
            }
        }
    }

    return out;
}

void adc_evaluation() {
    adc_move();

    //adc / (4096/250)
    setting_top_temperature = adc_correction(1) / 16;
    setting_bottom_temperature = adc_correction(2) / 16;

    if (setting_top_temperature > 250) {
        setting_top_temperature = 250;
    }
    if (setting_bottom_temperature > 250) {
        setting_bottom_temperature = 250;
    }

    uint16_t tmp = adc_correction(0);

    if (tmp > 2700) {
        setting_mode = MODE_IDLE;
    } else if (tmp > 2400) {
        setting_mode = MODE_BAKE;
    } else if (tmp > 2300) {
        setting_mode = MODE_BAKE | MODE_ROTATE;
    } else if (tmp > 2200) {
        setting_mode = MODE_FERMENT;
        //发酵模式30度
        setting_top_temperature = 30;
        setting_bottom_temperature = 30;
    } else if (tmp > 2100) {
        setting_mode = MODE_BAKE | MODE_FAN;
    } else if (tmp > 1900) {
        setting_mode = MODE_BAKE | MODE_ROTATE | MODE_FAN;
    } else {
        setting_mode = MODE_IDLE;
    }
}


int main(void)
{
    SystemClock_Config();

    HAL_Init();

    gpio_init();

    UART_init();
    printf("boot\n");

    hspi_init();
    ADC_init();
    sensor_config();

    uint8_t err = 0;

    while (1)
    {
        adc_evaluation();

        if (tick > 1000) {
            tick = 0;

            //重置显示
            max7219_reset(&disp);

            //空闲模式关闭所有
            if (setting_mode == MODE_IDLE) {
                printf("mode_idle\n");
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, 0);
                continue;
            }

            //温度检查
            float top = MAX31865_Read_1shot(&top_sensor);
            float bottom = MAX31865_Read_1shot(&bottom_sensor);

            if (top != top) {
                err++;
            } else if (bottom != bottom) {
                err++;
            } else {
                err = 0;
            }

            if (err > 0) {
                printf("max31856 error: %d %d\n", (int16_t)top, (int16_t)bottom);

                if (err > 3) {
                    //显示错误
                    top_temperature = top;
                    bottom_temperature = bottom;

                    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, 0);
                }
                continue;
            }

            //显示
            top_temperature = top;
            bottom_temperature = bottom;

            printf("mode %d temp %d/%d %d/%d\n",
                    setting_mode,
                    (int16_t)top_temperature, setting_top_temperature,
                    (int16_t)bottom_temperature, setting_bottom_temperature
                  );

            //发酵模式下管加热，测温以高的那个为准
            if (setting_mode == MODE_FERMENT) {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);

                if (bottom > top) {
                    top = bottom;
                }

                if (switch_status & 2) {
                    if (top > setting_top_temperature) {
                        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
                        switch_status &= ~2;
                    }
                } else {
                    if (top > 10 && top_temperature < setting_top_temperature-2) {
                        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
                        switch_status |= 2;
                    }
                }
            } else {
                //上加热管
                if (switch_status & 1) {
                    if (top_temperature > setting_top_temperature) {
                        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
                        switch_status &= ~1;
                    }
                } else {
                    if (top_temperature > 10 && top_temperature < setting_top_temperature-2) {
                        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
                        switch_status |= 1;
                    }
                }

                //下加热管
                if (switch_status & 2) {
                    if (bottom_temperature > setting_bottom_temperature) {
                        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
                        switch_status &= ~2;
                    }
                } else {
                    if (bottom_temperature > 10 && bottom_temperature < setting_bottom_temperature-2) {
                        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
                        switch_status |= 2;
                    }
                }
            }

            //热风烤
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, ((setting_mode & MODE_FAN) > 0));
            //旋转烤架
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, ((setting_mode & MODE_ROTATE) > 0));
        }

        display_temp();

        HAL_Delay(1);
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
    top_sensor.spi.CLK_PIN = GPIO_PIN_5;
    top_sensor.spi.CLK_PORT = GPIOA;
    top_sensor.spi.MOSI_PIN = GPIO_PIN_6;
    top_sensor.spi.MOSI_PORT = GPIOA;
    top_sensor.spi.MISO_PIN = GPIO_PIN_7;
    top_sensor.spi.MISO_PORT = GPIOA;
    bottom_sensor.spi = top_sensor.spi;
    disp.spi = top_sensor.spi;

    top_sensor.spi.CE_PIN = GPIO_PIN_8;
    top_sensor.spi.CE_PORT = GPIOA;
    top_sensor.Is3Wire = true;

    bottom_sensor.spi.CE_PIN = GPIO_PIN_9;
    bottom_sensor.spi.CE_PORT = GPIOA;
    bottom_sensor.Is3Wire = true;

    /* top_sensor.spi.hspi = &SpiHandle; */
    /* bottom_sensor.spi.hspi = &SpiHandle; */

    //初始化spi
    SPI_Init(&top_sensor.spi);
    SPI_Init(&bottom_sensor.spi);

    //启动传感器
    MAX31856_AutoConvert(&top_sensor);
    MAX31856_AutoConvert(&bottom_sensor);

    //数码管
    disp.spi.CE_PIN = GPIO_PIN_10;
    disp.spi.CE_PORT = GPIOA;
    /* disp.spi.hspi = &SpiHandle; */
    disp.limit = 5;
    disp.intensity = 3;
    max7219_init(&disp);
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

