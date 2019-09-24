/**
 * Olivier Van den Eede (4ilo) 2019
 */
#include "MAX31865.h"

/********************* MAX31865 registers and config bits *********************/

#define MAX31865_WRITE                  0x80

#define MAX31856_REG_CONFIG             0x00
#define MAX31856_REG_RTDMSB             0x01

#define MAX31865_CONFIG_CLEARFAULT      (1<<1)
#define MAX31856_CONFIG_3WIRE           (1<<4)
#define MAX31856_CONFIG_1SHOT           (1<<5)
#define MAX31856_CONFIG_MODEAUTO        (1<<6)
#define MAX31856_CONFIG_BIAS            (1<<7)

/********************* Constants *********************/
#define MAX31856_RREF 430                        // Reference resistor
#define MAX31856_FACTOR 32768                    // 2^15 used for data to resistance conversion
#define MAX31856_ALPHA 0.003851                  // PT-100 temperature coefficient

/*********************** Begin Private functions *************************/

/**
 * Read x bytes from MAX31865 starting from addr
 *
 * @param addr      Register addr to read from
 * @param buffer    Pointer to rx buffer
 * @param len       Amount of bytes to read
 */
static uint8_t MAX31865_Read_Reg(MAX31865 *xThis, uint8_t addr)
{
    uint8_t tx[2] = {addr, 0}, rx[2];

    SPI_TransmitReceive(&xThis->spi, tx, rx, 2);

    return rx[1];
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

    uint8_t tx[2] = {addr, data};

    SPI_Transmit(&xThis->spi, tx, 2);
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
    HAL_Delay(70);

    //读取数据
    uint8_t tx[3]={MAX31856_REG_RTDMSB, 0,0}, buf[3];

    SPI_TransmitReceive(&xThis->spi, tx, buf, 2);

    //关闭电源
    MAX31856_Off(xThis);

    return MAX31856_Calculate(buf+1);
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
    uint8_t tx[3]={MAX31856_REG_RTDMSB, 0,0}, buf[3];

    SPI_TransmitReceive(&xThis->spi, tx, buf, 2);

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
