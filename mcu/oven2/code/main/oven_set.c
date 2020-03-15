#include <stdint.h>
#include "base.h"

#include "esp_log.h"
#include "freertos/task.h"
#include "display.h"
#include "peripherals.h"
#include "oven.h"

static const char *TAG = "oven_set";

static uint8_t set_tid = 0;
static uint8_t set_val = 0;

//编码器快速转动标记
static bool sw_fast = false;

static void set_sw1_increase()
{
    set_tid++;
    if (set_tid > TASK_ROTATE) {
        set_tid = TASK_ROTATE;
    }
    display_top(set_tid, 0);
}

static void set_sw1_decrease()
{
    set_tid--;
    if (set_tid > TASK_ROTATE) {
        set_tid = 0;
    }
    display_top(set_tid, 0);
}

static void set_sw2_increase()
{
    set_val += sw_fast ? 5 : 1;
    sw_fast = true;

    display_bottom(set_val, 0);
}

static void set_sw2_decrease()
{
    set_val -= sw_fast ? 5 : 1;
    sw_fast = true;

    display_bottom(set_val, 0);
}

static void set_sw1_up()
{
    if (set_tid == 0) {
        state_change(STATE_RUN, NULL);
    } else {
        if (task_idx < MAX_TASK) {
            tasks[task_idx++].tid = set_tid;
            tasks[task_idx++].val = set_val;
            tasks[task_idx++].time = 0;

            set_tid = 0;
            set_val = 0;

            display_top(set_tid, 0);
            display_bottom(set_val, 0);
        } else {
            state_change(STATE_ERROR, (void*)ERROR_TASK_OVERFLOW);
        }
    }
}

static void set_sw2_up()
{
    state_change(STATE_IDEL, NULL);
}

void set_main()
{
    task_idx = 0;

    Events cb = {
        .sw1 = {
            .increase = set_sw1_increase,
            .decrease = set_sw1_decrease,
            .up = set_sw1_up,
        },
        .sw2 = {
            .increase = set_sw2_increase,
            .decrease = set_sw2_decrease,
            .up = set_sw2_up,
        }
    };

    event_id id;

    for(;;) {
        if (xQueueReceive(event_queue, &id, pdMS_TO_TICKS(200))) {
            ESP_LOGI(TAG, "event: %d", id);

            peripheral_event(&cb, id);
        } else {
            sw_fast = false;
            display_refresh();
        }
    }
}
