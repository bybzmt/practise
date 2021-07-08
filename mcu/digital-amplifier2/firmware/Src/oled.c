#include "base.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"

static void _oled_show_fs(char *str)
{
    ssd1306_FillRectangle(0, 0, 7*5, 10, Black);

    ssd1306_SetCursor(0, 0);

    ssd1306_Digit(str, 7, 10, White);
    ssd1306_Char(K, 7, 10, White);
    ssd1306_Char(H, 7, 10, White);
    ssd1306_Char(z, 7, 10, White);
}

static void _oled_show_vol()
{
    if (audio.mute) {
        ssd1306_SetCursor(100, 53);
        ssd1306_Char(M, 7, 10, Black);
        ssd1306_Char(u, 7, 10, Black);
        ssd1306_Char(t, 7, 10, Black);
        ssd1306_Char(e, 7, 10, Black);
    } else {
        ssd1306_FillRectangle(100, 53, 7*4, 10, Black);
    }

    char buf[5];

    sprintf(buf, "%d", audio.vol);

    ssd1306_FillRectangle(50+7+1, 20, 16*3+7*2+1, 26, Black);
    ssd1306_SetCursor(50+7+1, 20);
    ssd1306_Digit(buf, 16, 26, White);

    uint8_t len = strlen(buf);
    ssd1306_SetCursor(50+7+2 + (len*16), 36);
    ssd1306_Char(25, 7, 10, White);
}

static void _oled_show_input()
{
    ssd1306_FillRectangle(0, 53, 7*4, 10, Black);
    ssd1306_SetCursor(0, 53);

    ssd1306_Char(U, 7, 10, White);
    ssd1306_Char(S, 7, 10, White);
    ssd1306_Char(B, 7, 10, White);
}

void oled_init(void)
{
    return;
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

void oled_mode1(uint8_t focus)
{
    return;
    _oled_show_input();
    _oled_show_vol();
    _oled_show_fs("48");

    ssd1306_UpdateScreen();
}

void oled_mode2(uint8_t focus)
{
    return;
}
