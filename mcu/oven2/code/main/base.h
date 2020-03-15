#ifndef BASE_H
#define BASE_H

#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

//模式开关状态
#define MODE_IDLE    0
#define MODE_FERMENT 1
#define MODE_BAKE    2
#define MODE_FAN     4
#define MODE_ROTATE  8

//设定上温度，单位度
#define TASK_SET_TOP 1
//设定下温度，单位度
#define TASK_SET_bottom 2
//设定上温度并等待它达到，单位度
#define TASK_WAIT_TOP 3
//设定下温度并等待它达到，单位度
#define TASK_WAIT_BOTTOM 4
//等待时间，单位分
#define TASK_DELAY 5
//风扇启停
#define TASK_FAN 6
//转动杆启停
#define TASK_ROTATE 7

//最大日志数量
#define MAX_TEMP_LOG 2048
//最大任务数量
#define MAX_TASK 255

#define STATE_IDEL    1
#define STATE_FERMENT 2
#define STATE_BAKE    3
#define STATE_RUN    4
#define STATE_SET    5
#define STATE_ERROR    6

//其它错误
#define ERROR_UNDEFINED 0
//模式读取错误
#define ERROR_MODE 1
//任务溢出
#define ERROR_TASK_OVERFLOW 2
//温度传感器异常
#define ERROR_MAX31865 3

//系统运行时间
extern uint32_t runTime;

//事件队列
extern xQueueHandle event_queue;
typedef uint32_t event_id;

//当前模式
extern uint8_t mode;
//项部设定温度
extern uint8_t temp_top;
//底部设定温度
extern uint8_t temp_bottom;
//当前顶部温度
extern float now_temp_top;
//当前底部温度
extern float now_temp_bottom;

//是否启动烘焙
extern bool sw_bake;
//是否启动风扇
extern bool sw_fan;
//是否启动转杆
extern bool sw_rotate;

//烘培开始时间
extern uint32_t sw_bake_start_time;
//烘培结速时间
extern uint32_t sw_bake_stop_time;

//温度日志当前索引
extern uint16_t temp_log_idx;
//温度日志抽样间隔,单位秒
extern uint8_t temp_log_sampling;
//温度日志上
extern uint8_t temp_log_top[MAX_TEMP_LOG];
//温度日志下
extern uint8_t temp_log_bottom[MAX_TEMP_LOG];

#endif
