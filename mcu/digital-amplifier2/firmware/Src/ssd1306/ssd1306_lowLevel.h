#ifndef __SSD1306_LOWLEVEL_H__
#define __SSD1306_LOWLEVEL_H__

#include <stdbool.h>
#include <stdint.h>

// Low-level procedures
void ssd1306_Delay(uint16_t tick);
void ssd1306_Reset(void);
bool ssd1306_WriteCommand(uint8_t byte);
bool ssd1306_WriteData(uint8_t* buffer, size_t buff_size);

#endif
