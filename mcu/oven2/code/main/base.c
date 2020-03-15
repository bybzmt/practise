#include <stdint.h>
#include "base.h"

#include "freertos/task.h"


uint32_t runTime = 0;

xQueueHandle event_queue = NULL;

uint8_t mode;
uint8_t temp_top;
uint8_t temp_bottom;
float now_temp_top;
float now_temp_bottom;

bool sw_bake;
bool sw_fan;
bool sw_rotate;

uint32_t sw_bake_start_time = 0;
uint32_t sw_bake_stop_time = 0;

uint16_t temp_log_idx = 0;
uint8_t temp_log_sampling = 1;
uint8_t temp_log_top[MAX_TEMP_LOG];
uint8_t temp_log_bottom[MAX_TEMP_LOG];
