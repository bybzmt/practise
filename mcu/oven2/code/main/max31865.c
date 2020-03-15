/**
 * Olivier Van den Eede (4ilo) 2019
 */
#include "max31865.h"
#include <string.h>
#include "freertos/task.h"
#include "esp_log.h"

/********************* MAX31865 registers and config bits *********************/

#define MAX31865_WRITE                  0x80

#define MAX31856_REG_CONFIG             0x00
#define MAX31856_REG_RTDMSB             0x01
#define MAX31856_REG_STATUS             0x07

#define MAX31865_CONFIG_CLEARFAULT      (1<<1)
#define MAX31856_CONFIG_3WIRE           (1<<4)
#define MAX31856_CONFIG_1SHOT           (1<<5)
#define MAX31856_CONFIG_MODEAUTO        (1<<6)
#define MAX31856_CONFIG_BIAS            (1<<7)

/********************* Constants *********************/
/* #define MAX31856_RREF 430                        // Reference resistor */
//430电阻实测为428
#define MAX31856_RREF 428                        // Reference resistor
#define MAX31856_FACTOR 32768                    // 2^15 used for data to resistance conversion
#define MAX31856_ALPHA 0.003851                  // PT-100 temperature coefficient

/*********************** Begin Private functions *************************/


static const char *TAG = "max31865";

/**
 * Read x bytes from MAX31865 starting from addr
 *
 * @param addr      Register addr to read from
 * @param buffer    Pointer to rx buffer
 * @param len       Amount of bytes to read
 */
static uint8_t MAX31865_Read_Reg(MAX31865 *xThis, uint8_t addr)
{
    esp_err_t ret;
    static spi_transaction_t trans;

    memset(&trans, 0, sizeof(spi_transaction_t));
    trans.length = 2;
    trans.flags=SPI_TRANS_USE_TXDATA|SPI_TRANS_USE_RXDATA;
    trans.tx_data[0] = addr;

    ret = spi_device_transmit(xThis->spi, &trans);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "spi通信失败");
        return 0;
    }

    return trans.rx_data[1];
}

/**
 * Write a byte in a MAX13865 register
 *
 * @param addr      Register addr to write to
 * @param buffer    Tx data
 */
static void MAX31865_Write_Reg(MAX31865 *xThis, uint8_t addr, uint8_t data)
{
    // Force write bit on address
    addr |= MAX31865_WRITE;

    esp_err_t ret;
    spi_transaction_t trans;

    memset(&trans, 0, sizeof(spi_transaction_t));
    trans.length = 2;
    trans.flags=SPI_TRANS_USE_TXDATA;
    trans.tx_data[0] = addr;
    trans.tx_data[1] = data;

    ret = spi_device_transmit(xThis->spi, &trans);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "spi通信失败");
    }
}

/*
 * 计算温度
 */
static float MAX31856_Calculate(uint8_t *buf)
{
    // Combine 2 bytes into 1 number, and shift 1 down to remove fault bit
    uint16_t data = (((uint16_t)buf[0] << 8) | buf[1]) >> 1;

    // Calculate the actual resistance of the sensor
    float resistance = ((float) data * MAX31856_RREF) / MAX31856_FACTOR;

    // Calculate the temperature from the measured resistance
    float temp = ((resistance / 100) - 1) / MAX31856_ALPHA;

    return temp;
}

/*********************** Begin Public functions *************************/

/**
 * 1次读取温度(自动重置错误)
 */
float MAX31865_Read_1shot(MAX31865 *xThis)
{
    //配置1shot
    uint8_t cfg = MAX31856_CONFIG_1SHOT | MAX31856_CONFIG_BIAS | MAX31865_CONFIG_CLEARFAULT;
    if (xThis->Is3Wire) {
        cfg |= MAX31856_CONFIG_3WIRE;
    }

    MAX31865_Write_Reg(xThis, MAX31856_REG_CONFIG, cfg);

    //等待时钟稳定
    vTaskDelay(70 / portTICK_RATE_MS);

    float ret = MAX31865_Read(xThis);

    //关闭电源
    MAX31856_Off(xThis);

    return ret;
}

/**
 * 启动自动转换
 */
void MAX31856_AutoConvert(MAX31865 *xThis)
{
    uint8_t cfg = MAX31856_CONFIG_MODEAUTO | MAX31856_CONFIG_BIAS | MAX31865_CONFIG_CLEARFAULT;

    if (xThis->Is3Wire) {
        cfg |= MAX31856_CONFIG_3WIRE;
    }

    MAX31865_Write_Reg(xThis, MAX31856_REG_CONFIG, cfg);
}

/*
 * 读取温度
 */
float MAX31865_Read(MAX31865 *xThis)
{
    uint8_t tx[8]={MAX31856_REG_RTDMSB, 0,0}, buf[8];

    esp_err_t ret;
    spi_transaction_t trans;

    memset(&trans, 0, sizeof(spi_transaction_t));
    trans.length = 8;
    trans.tx_buffer = &tx;
    trans.rx_buffer = &buf;

    ret = spi_device_transmit(xThis->spi, &trans);

    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "spi通信失败");
        return 0.0/0.0;
    }

    if (buf[7] != 0) {
        return 0.0/0.0;
    }

    return MAX31856_Calculate(buf+1);
}

/**
 * 关闭电源
 */
void MAX31856_Off(MAX31865 *xThis)
{
    uint8_t status = MAX31865_Read_Reg(xThis, MAX31856_REG_CONFIG);

    status &= ~MAX31856_CONFIG_BIAS;

    MAX31865_Write_Reg(xThis, MAX31856_REG_CONFIG, status);
}
