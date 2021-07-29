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
    evt_entry,
    evt_oled_refresh,
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
    static uint32_t old_tick=0;
    uint32_t tick=0;

    switch (evt)
    {
        case evt_entry:
            btn_mode = 1;
            oled_mode1();
            break;

        case evt_left:
            /* tick = HAL_GetTick() - old_tick; */
            /* audio_changeVolume(tick > 200 ? -2 : -10); */
            /* old_tick = HAL_GetTick(); */
            audio_changeVolume(-2);
            oled_mode1();
            break;

        case evt_right:
            /* tick = HAL_GetTick() - old_tick; */
            /* audio_changeVolume(tick > 200 ? 2 : 10); */
            /* old_tick = HAL_GetTick(); */

            audio_changeVolume(2);
            oled_mode1();
            break;

        case evt_sw_down:
            old_tick = HAL_GetTick();
            break;

        case evt_sw_up:
            tick = HAL_GetTick() - old_tick;
            if (tick > 1000) {
                btn_mode3(evt_entry);
            } else {
                btn_mode2(evt_entry);
            }
            break;

        case evt_oled_refresh:
            oled_mode1();
            break;
    }
}

/* 调整输入源 */
static void btn_mode2(uint8_t evt)
{
    static uint8_t input_mode=0;

    switch (evt)
    {
        case evt_entry:
            btn_mode = 2;
            audio_setMute(1);
            input_mode = settings.input_mode;

            oled_mode2(input_mode);
            break;

        case evt_left:
            input_mode = cut_and_mod(input_mode, 1, 4);
            oled_mode2(input_mode);
            break;

        case evt_right:
            input_mode = (input_mode+1) % 4;
            oled_mode2(input_mode);
            break;

        case evt_sw_up:
            if (input_mode < 3) {
                settings.input_mode = input_mode;
                audio_setMute(0);
            }
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
            oled_mode3(focus);

            focus = 0;
            break;

        case evt_left:
            focus = cut_and_mod(focus, 1, 5);
            oled_mode3(focus);
            break;

        case evt_right:
            focus = (focus+1) % 5;
            oled_mode3(focus);
            break;

        case evt_sw_up:
            switch (focus)
            {
                case 1:
                    settings.auto_switch = !settings.auto_switch;
                    break;
                case 2:
                    settings.auto_switch = 0;
                    settings.headphone_on = !settings.headphone_on;
                    break;
                case 3:
                    settings.auto_switch = 0;
                    settings.speakers_on = !settings.speakers_on;
                    break;
                case 4:
                    settings.auto_off = !settings.auto_off;
                    break;
            }

            btn_mode1(evt_entry);
            break;
    }
}

void task_btn_service()
{
    printf("task btn running\n");

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW,
    gpio_init.Mode  = GPIO_MODE_INPUT,
    gpio_init.Pull  = GPIO_NOPULL,
    gpio_init.Pin   = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOA, &gpio_init);

    gpio_init.Pin   = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    uint8_t evt = 0;

    ec11_rotate_left = btn_left_irq;
    ec11_rotate_right = btn_right_irq;
    ec11_sw_down = btn_down_irq;
    ec11_sw_up = btn_up_irq;

    btn_que = xQueueCreate(2, 1);

    bsp_ec11_init();
    oled_init();

    btn_mode1(evt_entry);

    for (;;) {
        if (xQueueReceive(btn_que, &evt, 1000)) {
            /* printf("mode:%d evt:%d\n", btn_mode, evt); */

            switch (btn_mode) {
                case 1: btn_mode1(evt); break;
                case 2: btn_mode2(evt); break;
                case 3: btn_mode3(evt); break;
            }

        } else {

            if (settings.auto_switch) {
                if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)) {
                    if (settings.headphone_on != 1 || settings.speakers_on != 0) {
                        settings.headphone_on = 1;
                        settings.speakers_on = 0;
                        audio_notify_dev();
                    }
                } else {
                    if (settings.headphone_on != 0 || settings.speakers_on != 1) {
                        settings.headphone_on = 0;
                        settings.speakers_on = 1;
                        audio_notify_dev();
                    }
                }
            }

            bool is_usb = !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
            if (audio.power_from_usb != is_usb) {
                audio.power_from_usb = is_usb;
                audio_notify_dev();
            }
        }
    }
}

void audio_changeVolume(int8_t num)
{
    if (num > 0) {
        if ((int16_t)settings.vol + num >= 255) {
            settings.vol = 255;
        } else {
            settings.vol += num;
        }
    } else {
        if ((int16_t)settings.vol - num <= 0) {
            settings.vol = 0;
        } else {
            settings.vol += num;
        }
    }

    audio_notify_dev();
}

void btn_oled_refresh(void)
{
    uint8_t data = evt_oled_refresh;
    xQueueSend(btn_que, &data, 10);
}
