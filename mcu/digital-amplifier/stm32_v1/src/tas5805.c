#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "base.h"
#include "i2c.h"
#include "tas5805.h"


static bool tas5805_cmd(uint8_t addr, uint8_t data);
static bool tas5805_cmd_to2(uint8_t addr, uint8_t data);
static bool tas5805_read(uint8_t addr, uint8_t *data);
static bool tas5805_check_cmd(uint8_t addr, uint8_t data);


static bool tas5805_cmd(uint8_t addr, uint8_t data)
{
    uint8_t buf[2] = {addr, data};

    return
        my_i2c_write(I2C_ADDR_5805_1, &buf[0], 2) &&
        my_i2c_write(I2C_ADDR_5805_2, &buf[0], 2);
}

static bool tas5805_cmd_to2(uint8_t addr, uint8_t data)
{
    uint8_t buf[2] = {addr, data};
    return my_i2c_write(I2C_ADDR_5805_2, &buf[0], 2);
}

static bool tas5805_read(uint8_t addr, uint8_t *data)
{
    return my_i2c_mem_read(I2C_ADDR_5805_1, addr, 1, data, 1);
}

void tas5805_init()
{
    //pdn
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, 0);
    vTaskDelay(10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, 1);
    vTaskDelay(10);

    //Reset Modules
    tas5805_cmd(0x01, 1<<4);
    vTaskDelay(10);

    /* tas5805_cmd(0x00, 0x00); //Go to Page 0x00 */
    /* tas5805_cmd(0x7f, 0x00); //Change the Book to 0x00 */
    /* tas5805_cmd(0x00, 0x00); //Go to Page 0x00 */

    tas5805_cmd(0x02, 1<<2); //768kHz/PBTL/BD Modulation
    tas5805_cmd(0x53, 0x60); //Class D Loop Bandwidth to 175kHz
    tas5805_cmd_to2(0x35, 2<<4); //5805_02 Left DAC From:Right
    tas5805_cmd(0x4c, volume); //Volume default
    tas5805_cmd(0x03, 0x03); //Play Mode
}

void tas5805_volume()
{
    tas5805_cmd(0x4c, volume_mute ? 0xFF : volume);
}

static bool tas5805_check_cmd(uint8_t addr, uint8_t data)
{
    uint8_t ret;

    if (!tas5805_read(addr, &ret)) {
        return false;
    }

    return ret == data;
}

void tas5805_check()
{
    if (
            tas5805_check_cmd(0x68, 3) &&
            tas5805_check_cmd(0x71, 0)
       ) {
        printf("tas5805_check_ok\n");
        return;
    }

    printf("tas5805_check_fail\n");
    NVIC_SystemReset();
    while (1);
}

void tas5805_show_sampling_rate()
{
    uint8_t data, ok;
    ok = tas5805_read(0x37, &data);
    if (!ok) {
        oled_show_fs(NULL);
        return;
    }

    switch (data & 0xF) {
        case 0:
            oled_show_fs(NULL);
        break;
        case 0b0010:
            oled_show_fs("8");
            break;
        case 0b0100:
            oled_show_fs("16");
            break;
        case 0b0110:
            oled_show_fs("32");
            break;
        case 0b1000:
            oled_show_fs("44");
            break;
        case 0b1001:
            oled_show_fs("48");
            break;
        case 0b1011:
            oled_show_fs("96");
            break;
        default:
            oled_show_fs(NULL);
    }
}

