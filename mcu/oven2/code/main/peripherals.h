#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include "driver/spi_master.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "base.h"
#include "max31865.h"
#include "max7219.h"

typedef void (*Callback) ();

typedef struct {
    Callback increase;
    Callback decrease;
    Callback down;
    Callback up;
} Encoder;

typedef struct {
    Encoder sw1;
    Encoder sw2;
} Events;

extern max7219 nixietube;

bool peripheral_event(Events *cb, event_id id);

void peripherals_init();

//通过adc读取模式
uint8_t mode_read();

//读取温度
bool read_temp();

//控制输出
void relay_ctrl();


#endif
