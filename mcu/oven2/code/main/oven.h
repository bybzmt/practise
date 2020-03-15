#ifndef OVEN_H
#define OVEN_H

#include <stdint.h>
#include "base.h"

//任务详情
typedef struct{
    uint8_t tid;
    uint8_t val;
    //动作完成时间，相对于烘培启动时间秒
    uint16_t time;
} task;

extern void (*oven_destruct)();

//任务列表
extern task tasks[MAX_TASK];
//当前任务
extern uint8_t task_idx;

//开启加热
void bake_on();
//关闭加热
void bake_off();

//检查并切换模式
void mode_check(uint8_t mode);
//直接切换模式
void state_change(uint8_t state, void *arg);

//记录温度日志
void temperature_log();

#endif
