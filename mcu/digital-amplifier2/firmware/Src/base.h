#ifndef BASE_H
#define BASE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "audio.h"
#include "vectorTable.h"

void task_usb_input();
void task_spdif_input();
void task_bt_input();
void task_btn_service();
void task_dev_setter();

void btn_oled_refresh(void);
void oled_init(void);
void oled_mode1(void);
void oled_mode2(uint8_t focus);
void oled_mode3(uint8_t focus);

#ifdef __cplusplus
}
#endif

#endif
