#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"

#define ENABLE 22
#define CLEAR 4

typedef struct{
    uint32_t hours;
    uint32_t minutes;
    uint32_t seconds;
    uint32_t mseconds;
    uint32_t useconds;
}times_t;

typedef struct
{  
    times_t time;
    uint32_t turn;
    uint32_t slant;
    uint32_t offset;
    uint32_t oscillation;
}data_t;

void jsonCreate(data_t* data, char **json);
void timeConvert(uint8_t *time, times_t *res);
// void HSPITrans_Task(void);
// void VSPITrans_Task(void);
// void Publisher_Task(void);

#endif