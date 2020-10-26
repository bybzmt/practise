#ifndef OLED_H
#define OLED_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "base.h"

void oled_show_vol(void);
void oled_show_fs(char *str);
void oled_init(void);
void oled_show_input(void);

#endif
