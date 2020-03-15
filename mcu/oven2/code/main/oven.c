#include <stdint.h>
#include "base.h"

#include "freertos/task.h"
#include "display.h"
#include "wifi.h"
#include "peripherals.h"
#include "oven.h"

static const char *TAG = "oven";

void (*oven_destruct)();
static TaskHandle_t oven_instance;

task tasks[MAX_TASK];
uint8_t task_num;
uint8_t task_idx;

TaskFunction_t ferment_main;
TaskFunction_t bake_main;
TaskFunction_t set_main;
TaskFunction_t run_main;
TaskFunction_t idle_main;

void bake_on()
{
    sw_bake = true;
    sw_bake_start_time = runTime;
    sw_bake_stop_time = 0;
    temp_log_idx = 0;
}

void bake_off()
{
    sw_bake = false;
    sw_bake_stop_time = runTime;
}

void temperature_log()
{
    static uint8_t tick = 0;
    if (sw_bake && (++tick >= temp_log_sampling)) {
        tick = 0;

        temp_log_top[temp_log_idx] = now_temp_top;
        temp_log_bottom[temp_log_idx] = now_temp_bottom;
        temp_log_idx++;
        if (temp_log_idx >= MAX_TEMP_LOG) {
            temp_log_idx = 0;
        }
    }
}

void error_main(void *arg)
{
    for (;;) {
        display_top((uint32_t)arg, 0);
        display_bottom((uint32_t)arg, 0);
        display_set(0, 11);
        display_set(3, 11);

        vTaskDelay(pdMS_TO_TICKS(500));

        display_off();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void mode_check(uint8_t tmp) {
    if (tmp == MODE_IDLE) {
        switch(mode) {
            case STATE_FERMENT:
            case STATE_BAKE:
                state_change(STATE_IDEL, NULL);
        }
    } else if (tmp == MODE_FERMENT) {
        if (mode != STATE_IDEL) {
            state_change(STATE_FERMENT, NULL);
        }
    } else if (tmp & MODE_BAKE) {
        if (mode != STATE_IDEL) {
            state_change(STATE_BAKE, NULL);
        }

        sw_fan = tmp & MODE_FAN;
        sw_rotate = tmp & MODE_ROTATE;
    } else {
        if (mode != STATE_ERROR) {
            state_change(STATE_ERROR, (void*)ERROR_MODE);
        }
    }
}

void state_change(uint8_t state, void *arg) {
    BaseType_t ret = 0;
    TaskHandle_t instance = NULL;

    if (oven_destruct != NULL) {
        oven_destruct();
        oven_destruct = NULL;
    }

    switch(state) {
        case STATE_IDEL:
            ret = xTaskCreate(idle_main, "idle", 1024, arg, 3, &instance);
            break;
        case STATE_FERMENT:
            ret = xTaskCreate(ferment_main, "ferment", 1024, arg, 3, &instance);
            break;
        case STATE_BAKE:
            ret = xTaskCreate(bake_main, "bake", 1024, arg, 3, &instance);
            break;
        case STATE_RUN:
            ret = xTaskCreate(run_main, "run", 1024, arg, 3, &instance);
            break;
        case STATE_SET:
            ret = xTaskCreate(set_main, "set", 1024, arg, 3, &instance);
            break;
        case STATE_ERROR:
            ret = xTaskCreate(error_main, "error", 1024, arg, 3, &instance);
            break;
    }

    if (ret != pdPASS) {
        ESP_LOGI(TAG, "xTaskCreate Error");
    }

    if (oven_instance != NULL) {
        vTaskDelete(oven_instance);
    }

    oven_instance = instance;
    mode = state;
}
