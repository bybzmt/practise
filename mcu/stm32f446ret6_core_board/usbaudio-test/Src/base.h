#ifndef BASE_H
#define BASE_H

#include <stdbool.h>
#include "cmsis_os.h"

#include "stm32f4xx_hal.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "usbd_audio_if.h"

#include "bsp_audio.h"

#define VOLUME_DEFAULT 0b00110000
#define VOLUME_TO_DB(v) ((int16_t)24 - ((int16_t)v/2))


#endif
