#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "base.h"
#include "msp_i2c.h"
#include "bsp_tas5805.h"

#define ADDR_1 (0x5a)
#define ADDR_2 (0x5c)

void bsp_tas5805_init()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
    vTaskDelay(20);

    /* msp_i2c_write_reg(0x00, 0x00); //Go to Page 0x00 */
    msp_i2c_write_reg(ADDR_1, 0x02, 1<<2); //768kHz/PBTL/BD Modulation
    msp_i2c_write_reg(ADDR_2, 0x02, 1<<2); //768kHz/PBTL/BD Modulation
    msp_i2c_write_reg(ADDR_1, 0x53, 0x60); //Class D Loop Bandwidth to 175kHz
    msp_i2c_write_reg(ADDR_2, 0x53, 0x60); //Class D Loop Bandwidth to 175kHz

    msp_i2c_write_reg(ADDR_2, 0x35, 2<<4); //5805_02 Left DAC From:Right

    msp_i2c_write_reg(ADDR_1, 0x4e, 0b10111011); //Volume Update every 4 FS periods
    msp_i2c_write_reg(ADDR_2, 0x4e, 0b10111011); //Volume Update every 4 FS periods

    msp_i2c_write_reg(ADDR_1, 0x78, 0b10000000); //clear analog fault
    msp_i2c_write_reg(ADDR_2, 0x78, 0b10000000); //clear analog fault
}

void bsp_tas5805_play(uint32_t AudioFreq, uint8_t bit_depth, volume_t vol)
{
    bsp_tas5805_volume(vol);

    msp_i2c_write_reg(ADDR_1, 0x03, 0x03); //Play Mode
    msp_i2c_write_reg(ADDR_2, 0x03, 0x03); //Play Mode
}

void bsp_tas5805_stop(void)
{
    bsp_tas5805_volume(0);
}

void bsp_tas5805_volume(uint8_t vol)
{
    uint8_t _vol = 0xff - vol;

    msp_i2c_write_reg(ADDR_1, 0x4c, _vol);
    msp_i2c_write_reg(ADDR_2, 0x4c, _vol);
}

void bsp_tas5805_deInit(void)
{
    bsp_tas5805_volume(0);
    vTaskDelay(1);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
    vTaskDelay(1);
}

void bsp_tas5805_check(void)
{
    /* uint8_t power_state1 = msp_i2c_read_reg(ADDR_1, 0x68); */
    /* uint8_t power_state2 = msp_i2c_read_reg(ADDR_2, 0x68); */

    uint8_t chan_fault1 = msp_i2c_read_reg(ADDR_1, 0x70);
    uint8_t chan_fault2 = msp_i2c_read_reg(ADDR_2, 0x70);

    uint8_t fault1 = msp_i2c_read_reg(ADDR_1, 0x71) & 0b11000111;
    uint8_t fault2 = msp_i2c_read_reg(ADDR_2, 0x71) & 0b11000111;

    uint8_t over_temperature_shutdown1 = msp_i2c_read_reg(ADDR_1, 0x72) & 0b00000001;
    uint8_t over_temperature_shutdown2 = msp_i2c_read_reg(ADDR_2, 0x72) & 0b00000001;

    uint8_t over_temperature_warning1 = msp_i2c_read_reg(ADDR_2, 0x73) & 0b00000100;
    uint8_t over_temperature_warning2 = msp_i2c_read_reg(ADDR_2, 0x73) & 0b00000100;

    printf("chan_fault1 %d\n", chan_fault1);
    printf("chan_fault2 %d\n", chan_fault2);
    printf("fault1 %d\n", fault1);
    printf("fault2 %d\n", fault2);
    printf("over_temperature_shutdown1 %d\n", over_temperature_shutdown1);
    printf("over_temperature_shutdown2 %d\n", over_temperature_shutdown2);
    printf("over_temperature_warning1 %d\n", over_temperature_warning1);
    printf("over_temperature_warning2 %d\n", over_temperature_warning2);

}

