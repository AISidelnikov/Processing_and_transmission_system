#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_timer.h"
#include "driver/gpio.h"

#define LED_PIN 2

void adc_init();
void VoltageRead_Task(void);
void timer_callback(void *param);