#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "base.h"

#include "i2c.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"

void oled_show_vol();
void oled_show_fs(char *str);

I2C_HandleTypeDef hi2c1;

static QueueHandle_t button_queue;
static QueueHandle_t vol_queue;

#define VOLUME_TO_DB(v) ((int16_t)24 - ((int16_t)v/2))

#define I2C_ADDR_5805_1 0x58
#define I2C_ADDR_5805_2 0x5a

static uint8_t volume = 0b00110000;
static volatile bool volume_seting = false;
static bool volume_mute = false;

static void tas5805_cmd(uint8_t addr, uint8_t data)
{
    uint8_t buf[2] = {addr, data};
    my_i2c_write(&hi2c1, I2C_ADDR_5805_1, &buf[0], 2);
    my_i2c_write(&hi2c1, I2C_ADDR_5805_2, &buf[0], 2);
}

static void tas5805_cmd_to2(uint8_t addr, uint8_t data)
{
    uint8_t buf[2] = {addr, data};
    my_i2c_write(&hi2c1, I2C_ADDR_5805_2, &buf[0], 2);
}

static uint8_t tas5805_read(uint8_t addr)
{
    uint8_t data=0;
    my_i2c_mem_read(&hi2c1, I2C_ADDR_5805_1, addr, 1, &data, 1);
    return data;
}

static void tas5805_init()
{
    //pcm2707 SSPND
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);

    //usb启用(飞线)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);
    vTaskDelay(1000);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);

    //pdn
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, 0);
    vTaskDelay(10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, 1);
    vTaskDelay(10);

    tas5805_cmd(0x00, 0x00); //Go to Page 0x00
    tas5805_cmd(0x7f, 0x00); //Change the Book to 0x00
    tas5805_cmd(0x00, 0x00); //Go to Page 0x00
    tas5805_cmd(0x02, 1<<2); //768kHz/PBTL/BD Modulation
    tas5805_cmd(0x53, 0x60); //Class D Loop Bandwidth to 175kHz
    tas5805_cmd_to2(0x35, 2<<4); //5805_02 Left DAC From:Right
    tas5805_cmd(0x4c, volume); //Volume default
    tas5805_cmd(0x03, 0x03); //Play Mode
}

static void tas5805_volume(int8_t vol)
{
    if (!volume_seting) {
        volume_seting = true;

        oled_show_vol();

        uint8_t flag = 1;
        xQueueSend(vol_queue, &flag, 100);
    }
}

static void tas5805_show_sampling_rate()
{
    uint8_t ret = tas5805_read(0x37);

    switch (ret & 0xF) {
        case 0:
            oled_show_fs(NULL);
            printf("FS_Error\n");

            //强制重启
            vTaskDelay(2000);
            tas5805_init();
        break;
        case 0b0010:
            oled_show_fs("8");
            printf("8KHz\n");
            break;
        case 0b0100:
            oled_show_fs("16");
            printf("16KHz\n"); break;
        case 0b0110:
            oled_show_fs("32");
            printf("32KHz\n"); break;
        case 0b1000:
            oled_show_fs("44");
            printf("44KHz\n"); break;
        case 0b1001:
            oled_show_fs("48");
            printf("48KHz\n"); break;
        case 0b1011:
            oled_show_fs("96");
            printf("96KHz\n"); break;
        default:
            oled_show_fs(NULL);
            printf("FS_Reserved\n");
    }
}

static void vTask_i2c(void *pvParams)
{
    UNUSED(pvParams);

    uint8_t flag;
    for (;;) {
        if (xQueueReceive(vol_queue, &flag, 2000) == pdTRUE) {
            switch (flag) {
                case 1:
                    volume_seting = false;
                    tas5805_cmd(0x4c, volume_mute ? 0xFF : volume);
                break;

                default:
                    printf("i2c_unkonw: %d\n", flag);
            }
        } else {
            //空闲时检测当前采样率
            tas5805_show_sampling_rate();

        }
    }
}

static void vTask_button(void *pvParams)
{
    UNUSED(pvParams);

    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    uint8_t sw=0;

    uint8_t flag;
    for (;;) {
        if (xQueueReceive(button_queue, &flag, 20) == pdTRUE) {
            switch (flag) {
                case 1:
                case 2:
                    if (sw == 0) {
                        sw = flag;
                    } else if (sw != flag) {
                        if (sw == 1) {
                            volume+=2;
                        } else {
                            volume-=2;
                        }

                        tas5805_volume(volume);
                    }
                    break;
                case 3:
                case 4:
                    volume_mute = !volume_mute;
                    tas5805_volume(volume);
                    break;
                default:
                    printf("button_unknow:%d\n", flag);
            }
        } else {
            sw = 0;
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint8_t flag = 0;

    if (GPIO_Pin == GPIO_PIN_0) {
        flag = 1;
    } else if (GPIO_Pin == GPIO_PIN_1) {
        flag = 2;
    } else if (GPIO_Pin == GPIO_PIN_2) {
        flag = 3;
    } else if (GPIO_Pin == GPIO_PIN_3) {
        flag = 4;
    }

    xQueueSendFromISR(button_queue, &flag, NULL);
}

void oled_show_fs(char *str)
{
    ssd1306_FillRectangle(0, 0, 7*5, 10, Black);

    ssd1306_SetCursor(0, 0);

    if (str == NULL) {
        ssd1306_Char(E, 7, 10, White);
        ssd1306_Char(R, 7, 10, White);
        ssd1306_Char(R, 7, 10, White);
    } else {
        ssd1306_Digit(str, 7, 10, White);
        ssd1306_Char(K, 7, 10, White);
        ssd1306_Char(H, 7, 10, White);
        ssd1306_Char(z, 7, 10, White);
    }

    ssd1306_UpdateScreen();
}

void oled_show_vol()
{
    if (volume_mute) {
        ssd1306_SetCursor(100, 53);
        ssd1306_Char(M, 7, 10, Black);
        ssd1306_Char(u, 7, 10, Black);
        ssd1306_Char(t, 7, 10, Black);
        ssd1306_Char(e, 7, 10, Black);
    } else {
        ssd1306_FillRectangle(100, 53, 7*4, 10, Black);
    }

    char buf[5];

    ssd1306_FillRectangle(50, 20, 16*4, 26, Black);

    int16_t db = VOLUME_TO_DB(volume);

    ssd1306_SetCursor(50, 26);

    if (db < 0) {
        sprintf(buf, "%d", -db);
        ssd1306_Char(2D, 7, 10, White);

        printf("vol: -%ddB\n", -db);
    } else {
        sprintf(buf, "%d", db);
        ssd1306_Char(2B, 7, 10, White);

        printf("vol: %ddB\n", db);
    }

    ssd1306_SetCursor(50+7+1, 20);
    ssd1306_Digit(buf, 16, 26, White);

    uint8_t len = strlen(buf);
    ssd1306_SetCursor(50+7 + 2 + (len*16), 36);
    ssd1306_Char(d, 7, 10, White);
    ssd1306_Char(B, 7, 10, White);

    ssd1306_UpdateScreen();
}

void oled_show_input()
{
    ssd1306_FillRectangle(0, 53, 7*4, 10, Black);
    ssd1306_SetCursor(0, 53);

    ssd1306_Char(U, 7, 10, White);
    ssd1306_Char(S, 7, 10, White);
    ssd1306_Char(B, 7, 10, White);

    ssd1306_UpdateScreen();
}

void oled_init()
{
    ssd1306_Init();

    oled_show_input();

    oled_show_vol();
}

void UserMain()
{
    //i2s外部接口 0:启用 1:停用
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0);


    my_i2c_init(&hi2c1);
    tas5805_init();

    button_queue = xQueueCreate(20, 1);
    vol_queue = xQueueCreate(20, 1);

    xTaskCreate(vTask_i2c, "i2c service", 1024, NULL, 1, NULL);
    xTaskCreate(vTask_button, "button service", 1024, NULL, 2, NULL);

    vTaskDelay( 10 );

    oled_init();
}

