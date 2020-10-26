#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "base.h"
#include "i2c.h"
#include "oled.h"
#include "tas5805.h"

typedef enum {
    EVENT_VOL = 1,
    EVENT_MUTE = 2,
    EVENT_INPUT = 3,
} EVENT_ID;

static QueueHandle_t event_queue;

uint8_t volume = VOLUME_DEFAULT + 40;
bool volume_mute = false;

static volatile uint8_t volume_setting = 0;
static volatile uint8_t sw_state = 0;

static void volume_set(int8_t num)
{
    if (num != 0) {
        volume = (int16_t)volume + num;

        volume_setting++;

        if (volume_setting < 15) {
            EVENT_ID flag = EVENT_VOL;
            xQueueSendFromISR(event_queue, &flag, NULL);
        }
    } else {
        tas5805_volume();
        oled_show_vol();

        volume_setting = 0;
    }
}

static void event_service()
{
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    EVENT_ID flag;

    for (;;) {
        uint16_t tick = sw_state ? 10 : 2000;

        if (xQueueReceive(event_queue, &flag, tick) == pdTRUE) {
            switch (flag) {
                case EVENT_VOL:
                    volume_set(0);
                    break;

                case EVENT_MUTE:
                case EVENT_INPUT:
                    volume_mute = !volume_mute;
                    tas5805_volume();
                    oled_show_vol();
                    break;

                default:
                    printf("button_unknow:%d\n", flag);
            }
        } else {
            if (sw_state) {
                sw_state = 0;
                volume_set(0);
            }

            //空闲时检
            if (tick == 2000) {
                HAL_RTCEx_BKUPWrite(NULL, RTC_BKP_DR9, volume);

                tas5805_check();

                tas5805_show_sampling_rate();
            }
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    EVENT_ID flag;

    if (GPIO_Pin == GPIO_PIN_0) {
        if (sw_state == 0) {
            sw_state = 1;
        } else if (sw_state != 1) {
            volume_set(2);
        }
    } else if (GPIO_Pin == GPIO_PIN_1) {
        if (sw_state == 0) {
            sw_state = 2;
        } else if (sw_state != 2) {
            volume_set(-2);
        }
    } else if (GPIO_Pin == GPIO_PIN_2) {
        flag = EVENT_MUTE;
        xQueueSendFromISR(event_queue, &flag, NULL);
    } else if (GPIO_Pin == GPIO_PIN_3) {
        flag = EVENT_INPUT;
        xQueueSendFromISR(event_queue, &flag, NULL);
    } else if (GPIO_Pin == GPIO_PIN_4) {
        NVIC_SystemReset();
        while (1);
    } else if (GPIO_Pin == GPIO_PIN_5) {
        NVIC_SystemReset();
        while (1);
    }
}

void rtc_bkp_init()
{
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_BKP_CLK_ENABLE();

    uint8_t flag = HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR8);
    if (flag) {
        volume = HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR9);
    } else {
        HAL_RTCEx_BKUPWrite(NULL, RTC_BKP_DR8, 1);
        HAL_RTCEx_BKUPWrite(NULL, RTC_BKP_DR9, volume);
    }
}

void UserMain()
{
    //i2s外部接口 0:启用 1:停用
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);

    rtc_bkp_init();

    my_i2c_init();

    tas5805_init();

    event_queue = xQueueCreate(20, sizeof(EVENT_ID));

    vTaskDelay( 10 );

    oled_init();

    event_service();
}

