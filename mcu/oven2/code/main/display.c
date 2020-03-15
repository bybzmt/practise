
#include "base.h"
#include "freertos/task.h"
#include "peripherals.h"
#include "display.h"

//显示位
void display_set(uint8_t i, uint8_t data) {
    max7219_show(&nixietube, i, data);
}

void display_top(uint8_t number, bool dip)
{
    uint8_t tmp;
    uint8_t bit = dip ? (1<<7) : 0;

    tmp = number < 100 ? 0xf : (number / 100) % 10;
    max7219_show(&nixietube, 0, tmp|bit);

    tmp = number < 10 ? 0xf : (number / 10) % 10;
    max7219_show(&nixietube, 1, tmp|bit);

    tmp = number % 10;
    max7219_show(&nixietube, 2, tmp|bit);
}

void display_bottom(uint8_t number, bool dip)
{
    uint8_t tmp;
    uint8_t bit = dip ? (1<<7) : 0;

    tmp = number < 100 ? 0xf : (number / 100) % 10;
    max7219_show(&nixietube, 0, tmp|bit);

    tmp = number < 10 ? 0xf : (number / 10) % 10;
    max7219_show(&nixietube, 1, tmp|bit);

    tmp = number % 10;
    max7219_show(&nixietube, 2, tmp|bit);
}


void display_temp_now_and_set(uint16_t ms)
{
    static uint16_t ms_total=0;

    if (ms == 0) {
        ms_total = 0;
        return;
    }

    if (ms_total <= 2000) {
        //显示设定温度
        display_top(temp_top, 0);
        display_bottom(temp_bottom, 0);
    } else if (ms_total <= 2200) {
        display_off();
    } else if (ms_total <= 3200) {
        //显示当前温度
        display_top(now_temp_top, 1);
        display_bottom(now_temp_bottom, 1);
    } else {
        display_off();
        ms_total = 0;
    }

    ms_total += ms;
}

void display_off()
{
    display_set(0, 0xf);
    display_set(1, 0xf);
    display_set(2, 0xf);
    display_set(3, 0xf);
    display_set(4, 0xf);
    display_set(5, 0xf);
}

void display_refresh()
{
    max7219_refresh(&nixietube);
}
