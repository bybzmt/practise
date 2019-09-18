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

static uint8_t spixfer(MAX31865 *gpio, uint8_t x) {
    uint8_t reply = 0;

    for (int i=7; i>=0; i--) {
        reply <<= 1;

        HAL_GPIO_TogglePin(gpio->CLK_PORT, gpio->CLK_PIN);

        HAL_GPIO_WritePin(gpio->MOSI_PORT, gpio->MOSI_PIN, x & (1<<i));

        HAL_GPIO_TogglePin(gpio->CLK_PORT, gpio->CLK_PIN);

        if (HAL_GPIO_ReadPin(gpio->MISO_PORT, gpio->MISO_PIN) == GPIO_PIN_SET) {
            reply |= 1;
        }
    }

    return reply;
}

/**
 * Read x bytes from MAX31865 starting from addr
 *
 * @param addr      Register addr to read from
 * @param buffer    Pointer to rx buffer
 * @param len       Amount of bytes to read
 */
static void MAX31865_read(MAX31865 *gpio, uint8_t addr, uint8_t *buffer, uint8_t len)
{
    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_RESET);

    spixfer(gpio, addr);

    for (uint8_t i=0; i<len; i++) {
        buffer[i] = spixfer(gpio, 0xff);
    }

    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_SET);
}

/**
 * Write a byte in a MAX13865 register
 *
 * @param addr      Register addr to write to
 * @param buffer    Tx data
 */
static void MAX31865_write(MAX31865 *gpio, uint8_t addr, uint8_t data)
{
    addr |= MAX31865_WRITE;                                 // Force write bit on address

    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_RESET);

    spixfer(gpio, addr);
    spixfer(gpio, data);

    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_SET);
}

/**
 * Enable of disable MAX831865 bias voltage
 * @param enable Enable of disable
 */
void enableBias(MAX31865 *gpio, uint8_t enable)
{
    uint8_t status;
    MAX31865_read(gpio, MAX31856_REG_CONFIG, &status, 1);

    if (enable)
    {
        status |= MAX31856_CONFIG_BIAS;
    } else
    {
        status &= ~MAX31856_CONFIG_BIAS;
    }

    MAX31865_write(gpio, MAX31856_REG_CONFIG, status);
}

/**
 * Enable of disable MAX831865 auto convert
 * @param enable Enable of disable
 */
void autoConvert(MAX31865 *gpio, uint8_t enable)
{
    uint8_t status;
    MAX31865_read(gpio, MAX31856_REG_CONFIG, &status, 1);

    if (enable)
    {
        status |= MAX31856_CONFIG_MODEAUTO;
    } else
    {
        status &= ~MAX31856_CONFIG_MODEAUTO;
    }

    MAX31865_write(gpio, MAX31856_REG_CONFIG, status);
}

/**
 * Set the amount of wires the temperature sensor uses
 * @param numwires 2,3 or 4 wires
 */
void setWires(MAX31865 *gpio)
{
    uint8_t status;
    MAX31865_read(gpio, MAX31856_REG_CONFIG, &status, 1);

    if (gpio->Is3Wire) // 3-wire
    {
        status |= MAX31856_CONFIG_3WIRE;
    } else // 2-4 wire
    {
        status &= ~MAX31856_CONFIG_3WIRE;
    }

    MAX31865_write(gpio, MAX31856_REG_CONFIG, status);
}

/**
 * Perform a single shot conversion
 */
void single_shot(MAX31865 *gpio)
{
    uint8_t status;

    // Read config register
    MAX31865_read(gpio, MAX31856_REG_CONFIG, &status, 1);

    // Enable 1shot bit, and write back
    status |= MAX31856_CONFIG_1SHOT;

    MAX31865_write(gpio, MAX31856_REG_CONFIG, status);
}

/*********************** Begin Public functions *************************/

/**
 * Initialise MAX31865 for single shot temperature conversion
 *
 * @param max_gpio  MAX31865 structure with pinout
 * @param wires     Amount of wires on the temperature probe (2,3 or 4)
 */
void MAX31865_init(MAX31865 *gpio)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin = gpio->CE_PIN;
    HAL_GPIO_Init(gpio->CE_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = gpio->CLK_PIN;
    HAL_GPIO_Init(gpio->CLK_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = gpio->MOSI_PIN;
    HAL_GPIO_Init(gpio->MOSI_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin       = gpio->MISO_PIN;
    HAL_GPIO_Init(gpio->MISO_PORT, &GPIO_InitStruct);

    // Datalines in reset state
    HAL_GPIO_WritePin(gpio->CE_PORT, gpio->CE_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(gpio->CLK_PORT, gpio->CLK_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(gpio->MOSI_PORT, gpio->MOSI_PIN, GPIO_PIN_RESET);

    /* setWires(gpio); */
    /* enableBias(gpio, 0); */
    /* autoConvert(gpio, 0); */
}

/**
 * Perform a single temperature conversion, and calculate the value
 *
 * @return  Temperature as float
 */
float MAX31865_readTemp(MAX31865 *gpio)
{
    uint8_t cfg = MAX31856_CONFIG_1SHOT | MAX31856_CONFIG_BIAS | MAX31865_CONFIG_CLEARFAULT;
    if (gpio->Is3Wire) {
        cfg |= MAX31856_CONFIG_3WIRE;
    }

    MAX31865_write(gpio, MAX31856_REG_CONFIG, cfg);

    HAL_Delay(70);

    // Read data from max31865 data registers
    uint8_t buf[2];
    MAX31865_read(gpio, MAX31856_REG_RTDMSB, buf, 2);

    /* printHex("SPI_Receive", buf, 2); */

    // Combine 2 bytes into 1 number, and shift 1 down to remove fault bit
    uint16_t data = (((uint16_t)buf[0] << 8) | buf[1]) >> 1;

    // Calculate the actual resistance of the sensor
    float resistance = ((float) data * MAX31856_RREF) / MAX31856_FACTOR;

    // Calculate the temperature from the measured resistance
    float temp = ((resistance / 100) - 1) / MAX31856_ALPHA;

    return temp;
}


