#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"

#include "base.h"
#include "wifi.h"
#include "peripherals.h"
#include "oven.h"

static const char *TAG = "main";

static void vTimerCallback(TimerHandle_t pxTimer)
{
    configASSERT( pxTimer );
    runTime++;
}

//系统基准时间
static void vTimerInit()
{
    TimerHandle_t xTimer;
    xTimer = xTimerCreate("Timer",
            pdMS_TO_TICKS(1000),
            pdTRUE,
            NULL,
            vTimerCallback
            );

    if (xTimer == NULL) {
        ESP_LOGI(TAG, "xTimerCreate Error");
    } else {
        xTimerStart(xTimer, 0);
    }
}

static esp_err_t init_spiffs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/www",
      .partition_label = NULL,
      .max_files = 5,   // This decides the maximum number of files that can be created on the storage
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

void app_main(void)
{
    vTimerInit();

    ESP_ERROR_CHECK( nvs_flash_init() );

    ESP_ERROR_CHECK(init_spiffs());

    //初始化事件队列
    event_queue = xQueueCreate(10, sizeof(event_id));

    //初始化
    initialise_wifi();
    peripherals_init();

    for (;;) {
        //检测模式
        /* mode_check(mode_read()); */
        mode_check(MODE_IDLE);

        //读取温度
        /* if (!read_temp()) { */
            /* state_change(STATE_ERROR, (void*)ERROR_MAX31865); */
        /* } */
        now_temp_top = 1;
        now_temp_bottom = 1;

        //记录日志
        temperature_log();

        //控制输出
        relay_ctrl();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

