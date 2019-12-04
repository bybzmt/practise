#ifndef MAX7219_h
#define MAX7219_h

#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "spi.h"

typedef struct {
    SPIx spi;
    uint8_t intensity;
    uint8_t limit;
    uint8_t _display[8];
} max7219;

typedef enum {
    max7219_digit0=1,
    max7219_digit1,
    max7219_digit2,
    max7219_digit3,
    max7219_digit4,
    max7219_digit5,
    max7219_digit6,
    max7219_digit7
} max7219_digit;


void max7219_test(max7219 *h, bool ok);
void max7219_init(max7219 *h);
void max7219_reset(max7219 *h);
void max7219_show(max7219 *h, max7219_digit digit, uint8_t data);
void max7219_set(max7219 *h, uint8_t addr, uint8_t data);

#endif
