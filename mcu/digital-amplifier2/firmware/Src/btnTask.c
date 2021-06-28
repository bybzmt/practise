#include "base.h"
#include "bsp_ec11.h"

#define cut_and_mod(a,b,max) (a<b ? a+max-b : a-b)

void audio_changeVolume(int8_t num);

static uint8_t btn_mode = 0;
static QueueHandle_t btn_que;

enum evt {
    evt_left = 1,
    evt_right,
    evt_sw_down,
    evt_sw_up,
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

/* 默认状态，调整音量 */
static void btn_mode0(uint8_t evt, uint8_t *fast)
{
    switch (evt)
    {
        case evt_left:
            audio_changeVolume(*fast ? -5 : -1);
            oled_mode1(audio.input_mode);
            *fast = 1;
            break;
        case evt_right:
            audio_changeVolume(*fast ? 5 : 1);
            oled_mode1(audio.input_mode);
            *fast = 1;
            break;
        case evt_sw_down:
            *fast = 0;
            break;
        case evt_sw_up:
            if (fast) {
                btn_mode = 1;
                audio_setMute(1);
                oled_mode1(audio.input_mode);
            } else {
                oled_mode2(0);
                btn_mode = 2;
            }
            break;
    }

}

/* 调整输入源 */
static void btn_mode1(uint8_t evt, uint8_t *fast)
{
    switch (evt)
    {
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
            btn_mode = 0;
            audio_setMute(0);
            oled_mode1(audio.input_mode);
            break;
    }
}

/* 静音调整音量 */
static void btn_mode2(uint8_t evt, uint8_t *fast)
{
    switch (evt)
    {
        case evt_left:
            audio_changeVolume(*fast ? -5 : -1);
            oled_mode1(3);
            *fast = 1;
            break;
        case evt_right:
            audio_changeVolume(*fast ? 5 : 1);
            oled_mode2(3);
            *fast = 1;
            break;
        case evt_sw_up:
            btn_mode = 0;
            audio_setMute(0);
            oled_mode1(audio.input_mode);
            break;
    }
}

/* 调整设置 */
static void btn_mode3(uint8_t evt, uint8_t *fast)
{
    static int8_t focus = 0;

    switch (evt)
    {
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

            btn_mode = 0;
            audio_setMute(0);
            oled_mode1(audio.input_mode);
            break;
    }
}

void task_btn_service()
{
    uint8_t evt = 0;
    uint8_t fast = 0;

    ec11_rotate_left = btn_left_irq;
    ec11_rotate_right = btn_right_irq;
    ec11_sw_down = btn_down_irq;
    ec11_sw_up = btn_up_irq;

    btn_que = xQueueCreate(10, 1);

    for (;;) {
        if (xQueueReceive(btn_que, &evt, fast ? 50 : 1000)) {
            switch (btn_mode) {
                case 0: btn_mode0(evt, &fast); break;
                case 1: btn_mode1(evt, &fast); break;
                case 2: btn_mode2(evt, &fast); break;
                case 3: btn_mode3(evt, &fast); break;
            }
        } else {
            fast = 0;
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

