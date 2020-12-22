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

extern I2C_HandleTypeDef hi2c1;
extern PCD_HandleTypeDef hpcd;
extern SAI_HandleTypeDef hsai_out;


void bsp_tas6424_init(void);
void bsp_tas6424_DC_diagnostic(void);
void bsp_ssd1306_init(void);


#endif
