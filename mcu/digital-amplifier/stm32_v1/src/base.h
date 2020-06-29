#ifndef BASE_H
#define BASE_H

#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define VOLUME_DEFAULT 0b00110000
#define VOLUME_TO_DB(v) ((int16_t)24 - ((int16_t)v/2))

extern uint8_t volume;
extern bool volume_mute;

#endif
