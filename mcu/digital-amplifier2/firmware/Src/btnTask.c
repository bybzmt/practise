#include "base.h"
#include "bsp_ec11.h"

#define cut_and_mod(a,b,max) (a<b ? a+max-b : a-b)

void audio_changeVolume(int8_t num);

static uint8_t btn_mode = 0;
static uint16_t btn_tick = 1000;
static bool btn_timeout = 0;

static QueueHandle_t btn_que;

enum evt {
    evt_left = 1,
    evt_right,
    evt_sw_down,
    evt_sw_up,
    evt_entry,
};

static void btn_left_irq (void)
{
    uint8_t data = evt_left;
    xQueueSendFromISR(btn_que, &data, NULL);
}

static void btn_right_irq (void)
{
    uint8_t data = evt_right;
    xQueueSendFromISR(btn_que, &data, NULL);
}

static void btn_down_irq (void)
{
    uint8_t data = evt_sw_down;
    xQueueSendFromISR(btn_que, &data, NULL);
}

static void btn_up_irq (void)
{
    uint8_t data = evt_sw_up;
    xQueueSendFromISR(btn_que, &data, NULL);
}

static void btn_mode1(uint8_t evt);
static void btn_mode2(uint8_t evt);
static void btn_mode3(uint8_t evt);

/* 默认状态，调整音量 */
static void btn_mode1(uint8_t evt)
{
    switch (evt)
    {
        case evt_entry:
            btn_tick = 1000;
            btn_mode = 1;
            audio_setMute(0);
            oled_mode1(audio.input_mode);
            break;

        case evt_left:
            btn_tick = 100;
            audio_changeVolume(btn_timeout ? -1 : -5);
            oled_mode1(audio.input_mode);
            break;

        case evt_right:
            btn_tick = 100;
            audio_changeVolume(btn_timeout ? 1 : 5);
            oled_mode1(audio.input_mode);
            break;

        case evt_sw_down:
            btn_tick = 1000;
            break;

        case evt_sw_up:
            if (btn_timeout) {
                btn_mode3(evt_entry);
            } else {
                btn_mode2(evt_entry);
            }
            break;
    }
}

/* 调整输入源 */
static void btn_mode2(uint8_t evt)
{
    switch (evt)
    {
        case evt_entry:
            btn_mode = 2;
            audio_setMute(1);
            oled_mode1(audio.input_mode);
            break;

        case evt_left:
            audio.input_mode = cut_and_mod(audio.input_mode, 1, 3);
            oled_mode1(audio.input_mode);
            audio_notify_dev();
            break;

        case evt_right:
            audio.input_mode = (audio.input_mode+1) % 3;
            oled_mode1(audio.input_mode);
            audio_notify_dev();
            break;

        case evt_sw_up:
            btn_mode1(evt_entry);
            break;
    }
}

/* 调整设置 */
static void btn_mode3(uint8_t evt)
{
    static int8_t focus = 0;

    switch (evt)
    {
        case evt_entry:
            btn_mode = 3;
            oled_mode2(focus);

            focus = 0;
            break;

        case evt_left:
            focus = cut_and_mod(focus, 1, 7);
            oled_mode2(focus);
            break;

        case evt_right:
            focus = (focus+1) % 7;
            oled_mode2(focus);
            break;

        case evt_sw_up:
            switch (focus)
            {
                case 1:
                    audio.set_out1 = 0;
                    break;
                case 2:
                    audio.set_out1 = 1;
                    break;
                case 3:
                    audio.set_out1 = 2;
                    break;
                case 4:
                    audio.set_out2 = 0;
                    break;
                case 5:
                    audio.set_out2 = 1;
                    break;
                case 6:
                    audio.set_out2 = 2;
                    break;
            }

            btn_mode1(evt_entry);
            break;
    }
}

void task_btn_service()
{
    printf("task btn running\n");

    uint8_t evt = 0;

    ec11_rotate_left = btn_left_irq;
    ec11_rotate_right = btn_right_irq;
    ec11_sw_down = btn_down_irq;
    ec11_sw_up = btn_up_irq;

    btn_que = xQueueCreate(10, 1);

    bsp_ec11_init();
    oled_init();

    btn_mode1(evt_entry);

    for (;;) {
        if (xQueueReceive(btn_que, &evt, btn_tick)) {
            printf("mode:%d evt:%d\n", btn_mode, evt);

            switch (btn_mode) {
                case 1: btn_mode1(evt); break;
                case 2: btn_mode2(evt); break;
                case 3: btn_mode3(evt); break;
            }

            btn_timeout = 0;
        } else {
            btn_timeout = 1;
            btn_tick = 1000;
        }
    }
}

void audio_changeVolume(int8_t num)
{
    if (num > 0) {
        if ((int16_t)audio.vol + num > 255) {
            audio.vol = 255;
        } else {
            audio.vol += num;
        }
    } else {
        if ((int16_t)audio.vol - num < 0) {
            audio.vol = 0;
        } else {
            audio.vol += num;
        }
    }

    audio_notify_dev();
}

