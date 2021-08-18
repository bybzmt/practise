#include "base.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"

#define VOLUME_TO_DB(v) (((int16_t)v - 207) / 2)

static void _oled_show_fs()
{
    ssd1306_SetCursor(0, 0);

    char buf[6];
    sprintf(buf, "%d", (uint8_t)(audio.freq / 1000));
    ssd1306_Digit(buf, 7, 10, White);
    ssd1306_WriteString("KHz ", &Font_7x10, White);

    sprintf(buf, "%d", audio.bit_depth);
    ssd1306_Digit(buf, 7, 10, White);
    ssd1306_WriteString("bit", &Font_7x10, White);
}

static void _oled_show_vol()
{
    if (settings.mute) {
        ssd1306_SetCursor(100, 53);
        ssd1306_Char(M, 7, 10, Black);
        ssd1306_Char(u, 7, 10, Black);
        ssd1306_Char(t, 7, 10, Black);
        ssd1306_Char(e, 7, 10, Black);
    }

    char buf[6];
    int16_t db = VOLUME_TO_DB(settings.vol);

    ssd1306_SetCursor(40, 18);
    if (db < 0) {
        sprintf(buf, "%d", -db);
        ssd1306_Char(2D, 16, 26, White);

        printf("vol: -%ddB\n", -db);
    } else {
        sprintf(buf, "%d", db);
        ssd1306_Char(2B, 16, 26, White);

        printf("vol: %ddB\n", db);
    }

    ssd1306_SetCursor(50+7+1, 20);
    ssd1306_Digit(buf, 16, 26, White);

    uint8_t len = strlen(buf);
    ssd1306_SetCursor(50+7+2 + (len*16), 36);
    ssd1306_WriteString("dB", &Font_7x10, audio.power_from_usb ? Black : White);
}

static void _oled_show_input(void)
{
    ssd1306_SetCursor(0, 53);

    if (settings.input_mode==0) {
        ssd1306_WriteString("USB", &Font_7x10, White);
    }
    else if (settings.input_mode == 1) {
        ssd1306_WriteString("BT", &Font_7x10, White);
    }
    else if (settings.input_mode == 2) {
        ssd1306_WriteString("SPDIF", &Font_7x10, White);
    }
}

void oled_init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef  gpio;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOC, &gpio);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 1);

    ssd1306_Init();
    /* ssd1306_TestAll(); */
}

void oled_mode1(void)
{
    ssd1306_Fill(Black);

    _oled_show_input();
    _oled_show_vol();
    _oled_show_fs();

    ssd1306_UpdateScreen();
}

void oled_mode2(uint8_t focus)
{
    ssd1306_Fill(Black);

    uint8_t line_h = 15;

    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("input: usb", &Font_7x10, focus==0 ? Black : White);
    if (settings.input_mode == 0) {
        ssd1306_WriteString(" *", &Font_7x10, focus==0 ? Black : White);
    }

    ssd1306_SetCursor(0, line_h*1);
    ssd1306_WriteString("input: BT", &Font_7x10, focus==1 ? Black : White);
    if (settings.input_mode == 1) {
        ssd1306_WriteString(" *", &Font_7x10, focus==1 ? Black : White);
    }

    ssd1306_SetCursor(0, line_h*2);
    ssd1306_WriteString("input: SPDIF", &Font_7x10, focus==2 ? Black : White);
    if (settings.input_mode == 2) {
        ssd1306_WriteString(" *", &Font_7x10, focus==2 ? Black : White);
    }

    ssd1306_SetCursor(0, line_h*3);
    ssd1306_WriteString("MUTE", &Font_7x10, focus==3 ? Black : White);

    ssd1306_UpdateScreen();
}

void oled_mode3(uint8_t focus)
{
    ssd1306_Fill(Black);

    uint8_t line_h = 12;

    if (focus < 5) {
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("Back", &Font_7x10, focus==0 ? Black : White);

        ssd1306_SetCursor(0, line_h*1);
        ssd1306_WriteString("auto_switch", &Font_7x10, focus==1 ? Black : White);
        if (settings.auto_switch) {
            ssd1306_WriteString(" Yes", &Font_7x10, focus==1 ? Black : White);
        } else {
            ssd1306_WriteString(" No", &Font_7x10, focus==1 ? Black : White);
        }

        ssd1306_SetCursor(0, line_h*2);
        ssd1306_WriteString("headphone_on", &Font_7x10, focus==2 ? Black : White);
        if (settings.auto_switch) {
            ssd1306_WriteString(" N/A", &Font_7x10, focus==2 ? Black : White);
        } else if (settings.headphone_on) {
            ssd1306_WriteString(" Yes", &Font_7x10, focus==2 ? Black : White);
        } else {
            ssd1306_WriteString(" No", &Font_7x10, focus==2 ? Black : White);
        }

        ssd1306_SetCursor(0, line_h*3);
        ssd1306_WriteString("speakers_on", &Font_7x10, focus==3 ? Black : White);
        if (settings.auto_switch) {
            ssd1306_WriteString(" N/A", &Font_7x10, focus==3 ? Black : White);
        } else if (settings.speakers_on) {
            ssd1306_WriteString(" Yes", &Font_7x10, focus==3 ? Black : White);
        } else {
            ssd1306_WriteString(" No", &Font_7x10, focus==3 ? Black : White);
        }

        ssd1306_SetCursor(0, line_h*4);
        ssd1306_WriteString("auto_off", &Font_7x10, focus==4 ? Black : White);
        if (settings.auto_off) {
            ssd1306_WriteString(" Yes", &Font_7x10, focus==4 ? Black : White);
        } else {
            ssd1306_WriteString(" No", &Font_7x10, focus==4 ? Black : White);
        }
    } else {
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("BT reset", &Font_7x10, Black);

        ssd1306_SetCursor(0, line_h*1);
        ssd1306_WriteString("Back", &Font_7x10, White);
    }

    ssd1306_UpdateScreen();
}
