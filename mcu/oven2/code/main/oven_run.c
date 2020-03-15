#include <stdint.h>
#include "base.h"

#include "freertos/task.h"
#include "display.h"
#include "peripherals.h"
#include "oven.h"


//取消标记
static uint32_t run_abort = 0;

static void run_sw2_up()
{
    if (runTime - run_abort > 0) {
        state_change(STATE_IDEL, NULL);
    }
}

static void run_sw2_down()
{
    run_abort = runTime;
}

void run_main()
{
    //开启加热功能
    bake_on();
    sw_fan = false;
    sw_rotate = false;

    Events cb = {
        .sw2 = {
            .up = run_sw2_up,
            .down = run_sw2_down,
        },
    };

    bool old, now;
    event_id id;
    uint32_t start;

    while (task_idx > 0) {
        task_idx--;

        switch (tasks[task_idx].tid) {
            case TASK_SET_TOP:
                temp_top = tasks[task_idx].val;
                break;
            case TASK_SET_bottom:
                temp_bottom = tasks[task_idx].val;
                break;
            case TASK_WAIT_TOP:
                temp_top = tasks[task_idx].val;
                old = tasks[task_idx].val > now_temp_top;

                do {
                    if (xQueueReceive(event_queue, &id, pdMS_TO_TICKS(500))) {
                        peripheral_event(&cb, id);
                    } else {
                        display_top(tasks[task_idx].tid, 0);
                        display_set(0, 14);
                        display_bottom(tasks[task_idx].val, 0);
                    }

                    now = tasks[task_idx].val > now_temp_top;
                } while (old != now);
                break;
            case TASK_WAIT_BOTTOM:
                temp_bottom = tasks[task_idx].val;
                old = tasks[task_idx].val > now_temp_bottom;

                do {
                    if (xQueueReceive(event_queue, &id, pdMS_TO_TICKS(500))) {
                        peripheral_event(&cb, id);
                    } else {
                        display_top(tasks[task_idx].tid, 0);
                        display_set(0, 14);
                        display_bottom(tasks[task_idx].val, 0);
                    }

                    now = tasks[task_idx].val > now_temp_bottom;
                } while (old != now);
                break;
            case TASK_DELAY:
                start = runTime;
                while ( runTime - start >= (uint32_t)(tasks[task_idx].val) * 60) {
                    if (xQueueReceive(event_queue, &id, pdMS_TO_TICKS(500))) {
                        peripheral_event(&cb, id);
                    } else {
                        display_top(tasks[task_idx].tid, 0);
                        display_set(0, 14);
                        display_bottom(tasks[task_idx].val - ((runTime - start) / 60), 0);
                    }
                }
                break;
            case TASK_FAN:
                sw_fan = (bool)tasks[task_idx].val;
                break;
            case TASK_ROTATE:
                sw_rotate = (bool)tasks[task_idx].val;
                break;
        }

        tasks[task_idx].time = runTime - sw_bake_start_time;
    }

    state_change(STATE_IDEL, NULL);
}
