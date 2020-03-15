#include <stdint.h>
#include "base.h"

#include "freertos/task.h"
#include "display.h"
#include "peripherals.h"
#include "oven.h"

#define FERMENT_LOW 10
#define FERMENT_MAX 50

static const char *TAG = "oven_ferment";

static bool sw_fast = false;
static uint8_t ferment_temp_top = 30;
static uint8_t ferment_temp_bottom = 30;

static void ferment_sw1_increase()
{
    ferment_temp_top += sw_fast ? 5 : 1;
    sw_fast = true;

    if (ferment_temp_top > FERMENT_MAX) {
        ferment_temp_top = FERMENT_MAX;
    }
    temp_top = ferment_temp_top;

    display_top(temp_top, 0);
    display_temp_now_and_set(0);
}

static void ferment_sw1_decrease()
{
    ferment_temp_top -= sw_fast ? 5 : 1;
    sw_fast = true;

    if (ferment_temp_top < FERMENT_LOW) {
        ferment_temp_top = FERMENT_LOW;
    }
    temp_top = ferment_temp_top;

    display_top(temp_top, 0);
    display_temp_now_and_set(0);
}

static void ferment_sw2_increase()
{
    ferment_temp_bottom += sw_fast ? 5 : 1;
    sw_fast = true;

    if (ferment_temp_bottom > FERMENT_MAX) {
        ferment_temp_bottom = FERMENT_MAX;
    }
    temp_bottom = ferment_temp_bottom;

    display_bottom(temp_bottom, 0);
    display_temp_now_and_set(0);
}

static void ferment_sw2_decrease()
{
    ferment_temp_bottom -= sw_fast ? 5 : 1;
    sw_fast = true;

    if (ferment_temp_bottom < FERMENT_LOW) {
        ferment_temp_bottom = FERMENT_LOW;
    }
    temp_bottom = ferment_temp_bottom;

    display_bottom(temp_bottom, 0);
    display_temp_now_and_set(0);
}

void ferment_main()
{
    //开启加热功能
    bake_on();
    sw_fan = false;
    sw_rotate = false;

    Events cb = {
        .sw1 = {
            .increase = ferment_sw1_increase,
            .decrease = ferment_sw1_decrease,
        },
        .sw2 = {
            .increase = ferment_sw2_increase,
            .decrease = ferment_sw2_decrease,
        },
    };

    event_id id;

    for(;;) {
        if (xQueueReceive(event_queue, &id, pdMS_TO_TICKS(200))) {
            ESP_LOGI(TAG, "event: %d", id);

            peripheral_event(&cb, id);
        } else {
            display_temp_now_and_set(200);
            sw_fast = false;
        }
    }
}
