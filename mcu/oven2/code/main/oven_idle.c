#include <stdint.h>
#include "base.h"

#include "freertos/task.h"
#include "display.h"
#include "peripherals.h"
#include "oven.h"
#include "wifi.h"

static const char *TAG = "oven_idle";

static uint8_t idle_sw_down = 0;

static void idle_sw1_up()
{
    //转换到设定模式
    state_change(STATE_SET, NULL);
}

static void idle_sw2_up()
{
    //按住3秒松开重置wifi
    if (runTime - idle_sw_down > 3) {
        smartconfig_start();
    }
    idle_sw_down = 0;
}

static void idle_sw2_down()
{
    idle_sw_down = runTime;
}

void idle_main()
{
    //关闭加热功能
    bake_off();
    sw_fan = false;
    sw_rotate = false;

    //显示---
    display_set(0, 10);
    display_set(1, 10);
    display_set(2, 10);
    display_set(3, 10);
    display_set(4, 10);
    display_set(5, 10);

    Events cb = {
        .sw1 = {
            .up = idle_sw1_up,
        },
        .sw2 = {
            .down = idle_sw2_down,
            .up = idle_sw2_up,
        },
    };

    event_id id;

    for(;;) {
        if (xQueueReceive(event_queue, &id, pdMS_TO_TICKS(1000))) {
            ESP_LOGI(TAG, "event: %d", id);

            peripheral_event(&cb, id);
        } else {
            display_refresh();
        }
    }
}
