/* Esptouch example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_vfs.h"
#include <esp_http_server.h>

#include "base.h"
#include "wifi.h"
#include "webserver.h"
#include "oven.h"

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)

static const char *TAG = "webserver";

static httpd_handle_t server = NULL;

static esp_err_t index_handler(httpd_req_t *req);
static esp_err_t status_handler(httpd_req_t *req);
static esp_err_t logs_handler(httpd_req_t *req);
static esp_err_t tasks_handler(httpd_req_t *req);
static esp_err_t task_add_handler(httpd_req_t *req);
static esp_err_t task_reset_handler(httpd_req_t *req);
static esp_err_t bake_handler(httpd_req_t *req);

static const httpd_uri_t url_index = {
    .uri       = "/*",
    .method    = HTTP_GET,
    .handler   = index_handler,
};

static const httpd_uri_t url_status = {
    .uri       = "/api/status",
    .method    = HTTP_GET,
    .handler   = status_handler,
};

static const httpd_uri_t url_logs = {
    .uri       = "/api/logs",
    .method    = HTTP_GET,
    .handler   = logs_handler,
};

static const httpd_uri_t url_tasks = {
    .uri       = "/api/tasks",
    .method    = HTTP_GET,
    .handler   = tasks_handler,
};

static const httpd_uri_t url_task_add = {
    .uri       = "/api/task/add",
    .method    = HTTP_POST,
    .handler   = task_add_handler,
};

static const httpd_uri_t url_task_reset = {
    .uri       = "/api/task/reset",
    .method    = HTTP_POST,
    .handler   = task_reset_handler,
};

static const httpd_uri_t url_bake = {
    .uri       = "/api/bake",
    .method    = HTTP_POST,
    .handler   = bake_handler,
};

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

static esp_err_t index_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX] = "/www";
    char *buf = malloc(1024);

    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath+4, "/index.html", sizeof(filepath)-4);
    } else {
        strlcat(filepath+4, req->uri, sizeof(filepath)-4);
    }

    ESP_LOGI(TAG, "file: %s", req->uri);

    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGI(TAG, "Failed to open file : %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "404 Not Found.");
        goto ERR;
    }

    set_content_type_from_file(req, filepath);

    ssize_t read_bytes;

    do {
        read_bytes = read(fd, buf, 1024);
        if (read_bytes == -1) {
            ESP_LOGE(TAG, "Failed to read file : %s", filepath);
            break;
        } else if (read_bytes > 0) {
            if (httpd_resp_send_chunk(req, buf, read_bytes) != ESP_OK) {
                ESP_LOGI(TAG, "File sending failed!");
                break;
            }
        }
    } while (read_bytes > 0);

    close(fd);

ERR:
    free(buf);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

//获取状态信息
//系统时间，烘焙状态，风扇状态，转杆状态，当前模式,烘焙启动时间，烘焙结速时间
//当前日志位置,最大日志数，日志采样时间，当前任务位置，最大作务数
static esp_err_t status_handler(httpd_req_t *req)
{
    void *buf;
    int idx=0;
    int i;

    buf = malloc(1024);

    httpd_resp_set_type(req, "application/json;charset=utf-8");

    i = sprintf(buf+idx, "{\"ret\":true,\"time\":%d", runTime);
    idx+=i;
    i = sprintf(buf+idx, ",\"bake\":%d", sw_bake);
    idx+=i;
    i = sprintf(buf+idx, ",\"fan\":%d", sw_fan);
    idx+=i;
    i = sprintf(buf+idx, ",\"rotate\":%d", sw_rotate);
    idx+=i;
    i = sprintf(buf+idx, ",\"mode\":%d", mode);
    idx+=i;
    i = sprintf(buf+idx, ",\"start\":%d", sw_bake_start_time);
    idx+=i;
    i = sprintf(buf+idx, ",\"stop\":%d", sw_bake_stop_time);
    idx+=i;
    i = sprintf(buf+idx, ",\"log\":%d", temp_log_idx);
    idx+=i;
    i = sprintf(buf+idx, ",\"sampling\":%d", temp_log_sampling);
    idx+=i;
    i = sprintf(buf+idx, ",\"logmax\":%d", MAX_TEMP_LOG);
    idx+=i;
    i = sprintf(buf+idx, ",\"task\":%d", task_idx);
    idx+=i;
    i = sprintf(buf+idx, ",\"taskmax\":%d}", MAX_TASK);
    idx+=i;

    httpd_resp_send(req, buf, idx);
    free(buf);

    ESP_LOGI(TAG, "status_handler");

    return ESP_OK;
}

static esp_err_t logs_handler(httpd_req_t *req)
{
    void *buf = malloc(1024);
    int oft = 0;
    int toft;
    char *fmt;

    httpd_resp_set_type(req, "application/json;charset=utf-8");

    esp_err_t ret = httpd_req_get_url_query_str(req, buf, 100);
    if (ret != ESP_OK) {
        httpd_resp_sendstr(req, "{\"ret\":false}");
        goto ERR2;
    }

    char stmp[6];
    uint16_t idx, len, tidx;
    stmp[5] = 0;

    ret = httpd_query_key_value(buf, "idx", stmp, 5);
    if (ret != ESP_OK) {
        goto ERR2;
    }
    idx = atoi(stmp);

    ret = httpd_query_key_value(buf, "len", stmp, 5);
    if (ret != ESP_OK) {
        goto ERR2;
    }
    len = atoi(stmp);

    if (idx >= MAX_TEMP_LOG) {
        goto ERR2;
    }

    len = MAX_TEMP_LOG - idx < len ? MAX_TEMP_LOG - idx : len;
    if (len < 1) {
        goto ERR2;
    }

    toft = sprintf(buf, "{\"ret\":true,\"top\":[");
    oft += toft;

    tidx = idx;
    while (tidx < idx+len) {
        fmt = ",%d";
        if (tidx == idx) {
            fmt = "%d";
        }

        toft = sprintf(buf+oft, fmt, temp_log_top[tidx]);
        oft += toft;

        if (1024 - oft < 20) {
            ret = httpd_resp_send_chunk(req, buf, oft);
            if (ret != ESP_OK) {
                goto ERR;
            }
            oft = 0;
        }

        tidx++;
    }

    toft = sprintf(buf, "],\"bottom\":[");
    oft += toft;

    tidx = idx;
    while (tidx < idx+len) {
        fmt = ",%d";
        if (tidx == idx) {
            fmt = "%d";
        }

        toft = sprintf(buf+oft, fmt, temp_log_bottom[tidx]);
        oft += toft;

        if (1024 - oft < 20) {
            ret = httpd_resp_send_chunk(req, buf, oft);
            if (ret != ESP_OK) {
                goto ERR;
            }
            oft = 0;
        }

        tidx++;
    }

    toft = sprintf(buf, "]}");
    oft += toft;
    httpd_resp_send_chunk(req, buf, oft);

    httpd_resp_send_chunk(req, NULL, 0);

    free(buf);
    return ESP_OK;

ERR2:
    httpd_resp_sendstr(req, "{\"ret\":false}");
ERR:
    free(buf);
    ESP_LOGI(TAG, "bake_handler error");
    return ESP_OK;
}

static esp_err_t tasks_handler(httpd_req_t *req)
{
    char *buf = malloc(1024);
    int oft = 0;
    int toft;
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json;charset=utf-8");

    toft = sprintf(buf, "{\"ret\":true,\"tasks\":[");
    oft += toft;

    for (int i=0; i<task_idx; i++) {
        char *fmt = ",{\"tid\":%d,\"val\":%d,\"time\":%d}";
        if (i==0) {
            fmt = "{\"tid\":%d,\"val\":%d,\"time\":%d}";
        }

        toft = sprintf(buf+oft, fmt, tasks[i].tid, tasks[i].val, tasks[i].time);
        oft += toft;

        if (1024 - oft < 100) {
            ret = httpd_resp_send_chunk(req, buf, oft);
            if (ret != ESP_OK) {
                goto ERR;
            }
            oft = 0;
        }
    }

    buf[oft] = '}';
    oft++;

    httpd_resp_send_chunk(req, buf, oft);

    httpd_resp_send_chunk(req, NULL, 0);

    free(buf);

    ESP_LOGI(TAG, "tasks_handler ok");
    return ESP_OK;

ERR:
    free(buf);
    ESP_LOGI(TAG, "tasks_handler error");
    return ESP_OK;
}

static esp_err_t task_add_handler(httpd_req_t *req)
{
    char buf[40];
    int i;
    uint8_t retry = 0;
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json;charset=utf-8");

    for (;;) {
        i = httpd_req_recv(req, buf, 40);
        if (i == HTTPD_SOCK_ERR_TIMEOUT && retry++ < 3) {
            continue;
        }
        break;
    }

    char stmp[6];
    uint8_t tid, tval;
    stmp[5] = 0;

    ret = httpd_query_key_value(buf, "tid", stmp, 5);
    if (ret != ESP_OK) {
        goto ERR;
    }
    tid = atoi(stmp);

    ret = httpd_query_key_value(buf, "tval", stmp, 5);
    if (ret != ESP_OK) {
        goto ERR;
    }
    tval = atoi(stmp);

    if (tid == 0 || tid > TASK_ROTATE) {
        goto ERR;
    }

    if (mode == STATE_IDEL || mode == STATE_SET) {
        tasks[task_idx].tid = tid;
        tasks[task_idx].val = tval;
        tasks[task_idx].time = 0;
        task_idx++;

        httpd_resp_sendstr(req, "{\"ret\":true}");
        ESP_LOGI(TAG, "task_add_handler ok");
        return ESP_OK;
    } else {
        goto ERR;
    }

ERR:
    ESP_LOGI(TAG, "task_add_handler error");
    httpd_resp_sendstr(req, "{\"ret\":false}");
    return ESP_OK;
}

static esp_err_t task_reset_handler(httpd_req_t *req)
{
    char buf[20];
    int i;
    uint8_t retry = 0;

    httpd_resp_set_type(req, "application/json;charset=utf-8");

    for (;;) {
        i = httpd_req_recv(req, buf, 20);
        if (i == HTTPD_SOCK_ERR_TIMEOUT && retry++ < 3) {
            continue;
        }
        break;
    }

    if (strcmp(buf, "reset=on") == 0) {
        if (mode == STATE_IDEL || mode == STATE_SET) {
            task_idx = 0;

            httpd_resp_sendstr(req, "{\"ret\":true}");
            ESP_LOGI(TAG, "task_reset_handler ok");
            return ESP_OK;
        }
    }

    ESP_LOGI(TAG, "task_reset_handler error");
    httpd_resp_sendstr(req, "{\"ret\":false}");
    return ESP_OK;
}

static esp_err_t bake_handler(httpd_req_t *req)
{
    char buf[20];
    int i;
    uint8_t retry = 0;

    httpd_resp_set_type(req, "application/json;charset=utf-8");

    for (;;) {
        i = httpd_req_recv(req, buf, 20);
        if (i == HTTPD_SOCK_ERR_TIMEOUT && retry++ < 3) {
            continue;
        }
        break;
    }

    if (strcmp(buf, "bake=off") == 0) {
        if (mode == STATE_RUN) {
            state_change(STATE_IDEL, NULL);

            httpd_resp_sendstr(req, "{\"ret\":true}");
            ESP_LOGI(TAG, "bake_handler ok");
            return ESP_OK;
        } else {
            goto ERR;
        }
    }

    if (strcmp(buf, "bake=on") == 0) {
        if (mode == STATE_IDEL || mode == STATE_SET) {
            state_change(STATE_RUN, NULL);

            httpd_resp_sendstr(req, "{\"ret\":true}");
            ESP_LOGI(TAG, "bake_handler ok");
            return ESP_OK;
        } else {
            goto ERR;
        }
    }

ERR:
    ESP_LOGI(TAG, "bake_handler error");
    httpd_resp_sendstr(req, "{\"ret\":false}");
    return ESP_OK;
}


void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &url_status);
        httpd_register_uri_handler(server, &url_logs);
        httpd_register_uri_handler(server, &url_tasks);
        httpd_register_uri_handler(server, &url_task_add);
        httpd_register_uri_handler(server, &url_task_reset);
        httpd_register_uri_handler(server, &url_bake);
        httpd_register_uri_handler(server, &url_index);
        return;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return;
}

void stop_webserver()
{
    // Stop the httpd server
    httpd_stop(server);
    server = NULL;
}


