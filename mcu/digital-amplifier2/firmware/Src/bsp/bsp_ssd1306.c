#include "base.h"

#include "ssd1306.h"
#include "ssd1306_tests.h"

void bsp_ssd1306_show_fs(char *str)
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

void bsp_ssd1306_show_vol()
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

    sprintf(buf, "%d", volume);

    ssd1306_FillRectangle(50+7+1, 20, 16*3+7*2+1, 26, Black);
    ssd1306_SetCursor(50+7+1, 20);
    ssd1306_Digit(buf, 16, 26, White);

    uint8_t len = strlen(buf);
    ssd1306_SetCursor(50+7+2 + (len*16), 36);
    ssd1306_Char(25, 7, 10, White);

    ssd1306_UpdateScreen();
}

void bsp_ssd1306_show_input()
{
    ssd1306_FillRectangle(0, 53, 7*4, 10, Black);
    ssd1306_SetCursor(0, 53);

    ssd1306_Char(U, 7, 10, White);
    ssd1306_Char(S, 7, 10, White);
    ssd1306_Char(B, 7, 10, White);

    ssd1306_UpdateScreen();
}

void bsp_ssd1306_init(void)
{
    ssd1306_Init();

    ssd1306_TestAll();

    /* bsp_ssd1306_show_input(); */
    /* bsp_ssd1306_show_vol(); */
}