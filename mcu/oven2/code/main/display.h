#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "base.h"

//显示位
void display_set(uint8_t i, uint8_t data);

void display_top(uint8_t number, bool dip);

void display_bottom(uint8_t number, bool dip);

void display_off();

//交替显示当前温度和设定温度
void display_temp_now_and_set(uint16_t ms);

//刷新
void display_refresh();

#endif
