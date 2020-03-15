#include <stdint.h>
#include "base.h"

#include "freertos/task.h"
#include "display.h"
#include "peripherals.h"
#include "oven.h"

static const char *TAG = "oven_bake";

static bool sw_fast = false;
static uint8_t bake_temp_top = 150;
static uint8_t bake_temp_bottom = 150;

static void bake_sw1_increase()
{
    bake_temp_top += sw_fast ? 5 : 1;
    sw_fast = true;

    if (bake_temp_top > 250) {
        bake_temp_top = 250;
    }
    temp_top = bake_temp_top;
    display_temp_now_and_set(0);
}

static void bake_sw1_decrease()
{
    bake_temp_top -= sw_fast ? 5 : 1;
    sw_fast = true;

    if (bake_temp_top < 10) {
        bake_temp_top = 10;
    }
    temp_top = bake_temp_top;
    display_temp_now_and_set(0);
}

static void bake_sw2_increase()
{
    bake_temp_bottom += sw_fast ? 5 : 1;
    sw_fast = true;

    if (bake_temp_top > 250) {
        bake_temp_top = 250;
    }
    temp_bottom = bake_temp_bottom;
    display_temp_now_and_set(0);
}

static void bake_sw2_decrease()
{
    bake_temp_bottom -= sw_fast ? 5 : 1;
    sw_fast = true;

    if (bake_temp_bottom < 10) {
        bake_temp_bottom = 10;
    }
    temp_bottom = bake_temp_bottom;
    display_temp_now_and_set(0);
}

void bake_main()
{
    //开启加热功能
    bake_on();
    sw_fan = false;
    sw_rotate = false;

    Events cb = {
        .sw1 = {
            .increase = bake_sw1_increase,
            .decrease = bake_sw1_decrease,
        },
        .sw2 = {
            .increase = bake_sw2_increase,
            .decrease = bake_sw2_decrease,
        },
    };

    event_id id;

    for(;;) {
        if (xQueueReceive(event_queue, &id, pdMS_TO_TICKS(200))) {
            ESP_LOGI(TAG, "event: %d", id);

            peripheral_event(&cb, id);
        } else {
            sw_fast = false;
            display_temp_now_and_set(200);
        }
    }
}
