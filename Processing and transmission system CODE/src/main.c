#include "main.h"
#include "spi.h"
#include "mqtt.h"

static const char *TAG = "MAIN";
static esp_err_t err;
static EventGroupHandle_t *mqttEventGroup_p;
static devices_t devices;
static xSemaphoreHandle mutexH;
static xSemaphoreHandle mutexV;
static char *json = NULL;
static data_t data;
static esp_mqtt_client_handle_t client;

void HSPITrans_Task(void *pvParameters)
{
    uint8_t turn[7] = {0};
    uint8_t slant[7] = {0};
    uint8_t offset[7] = {0};
    mutexH = xSemaphoreCreateMutex();
    for(;;)
    {
        xSemaphoreTake(mutexH, portMAX_DELAY);
        ESP_LOGI(TAG, "TASK 1");
        
        ESP_ERROR_CHECK(spiRead(turn, 7, &devices.turnHandle));
        data.turn = (turn[2] << 24) | (turn[3] << 16) | (turn[4] << 8) | (turn[5] << 0);
        memset(&turn, 0, sizeof(uint8_t)*7);

        ESP_ERROR_CHECK(spiRead(slant, 7, &devices.slantHandle));
        data.slant = (slant[2] << 24) | (slant[3] << 16) | (slant[4] << 8) | (slant[5] << 0);
        memset(&slant, 0, sizeof(uint8_t)*7);

        ESP_ERROR_CHECK(spiRead(offset, 7, &devices.offsetHandle));
        data.offset = (offset[2] << 24) | (offset[3] << 16) | (offset[4] << 8) | (offset[5] << 0);
        memset(&offset, 0, sizeof(uint8_t)*7);

        xSemaphoreGive(mutexH);
        vTaskDelay(250 / portTICK_PERIOD_MS);     
    }
}

void VSPITrans_Task(void *pvParameters)
{
    uint8_t time[4] = {0};
    uint8_t osc[4] = {0};
    mutexV = xSemaphoreCreateMutex();
    for(;;)
    {
        xSemaphoreTake(mutexV, portMAX_DELAY);
        ESP_LOGI(TAG, "TASK 2");

        latch(STLD, 0);

        ESP_ERROR_CHECK(spiRead(time, 4, &devices.timeHandle));
        timeConvert(time, &data.time);
        memset(time, 0, sizeof(uint8_t)*4);

        ESP_ERROR_CHECK(spiRead(osc, 4, &devices.oscillationHandle));
        data.oscillation = (osc[0] << 0);
        memset(osc, 0, sizeof(uint8_t)*4);

        xSemaphoreGive(mutexV);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }   
}

void Publisher_Task(void *pvParameters)
{
    for(;;)
    {  
        xEventGroupWaitBits(*mqttEventGroup_p, START, pdFALSE, pdTRUE, portMAX_DELAY);
        xEventGroupWaitBits(*mqttEventGroup_p, CONNECTED, pdFALSE, pdTRUE, portMAX_DELAY);
        if(xSemaphoreTake(mutexV, portMAX_DELAY) && xSemaphoreTake(mutexH, portMAX_DELAY))
        {
            ESP_LOGI(TAG, "TASK 3");
            jsonCreate(&data, &json);
            esp_mqtt_client_publish(client, "test", json, 0, 1, 0);
            json = NULL;
            xSemaphoreGive(mutexV);
            xSemaphoreGive(mutexH);
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }
    }
}

void app_main()
{
    gpio_set(STLD, GPIO_MODE_OUTPUT, 1);
    gpio_set(CLEAR, GPIO_MODE_OUTPUT, 1);   
    spi_init(&devices);
    nvs_flash_init();
    wifi_init();
    mqtt_init(&mqttEventGroup_p, &client);

    xEventGroupWaitBits(*mqttEventGroup_p, START, pdFALSE, pdTRUE, portMAX_DELAY);
    latch(CLEAR, 0);
    gpio_set(ENABLE, GPIO_MODE_OUTPUT, 1);
    xTaskCreate(HSPITrans_Task, "task1", 2048, 0, 5, NULL);
    xTaskCreate(VSPITrans_Task, "task2", 2048, 0, 5, NULL);
    xTaskCreate(Publisher_Task, "task3", 2048, 0, 5, NULL);
}

void jsonCreate(data_t *data, char **json)
{
    cJSON *root;
    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "HOURS", data->time.hours);
    cJSON_AddNumberToObject(root, "MINUTES", data->time.minutes);
    cJSON_AddNumberToObject(root, "SECONDS", data->time.seconds);
    cJSON_AddNumberToObject(root, "MILES SECONDS", data->time.mseconds);
    cJSON_AddNumberToObject(root, "MICRO SECONDS", data->time.useconds);
    cJSON_AddNumberToObject(root, "TURN", data->turn);
    cJSON_AddNumberToObject(root, "SLANT", data->slant);
    cJSON_AddNumberToObject(root, "OFFSET", data->offset);
    cJSON_AddNumberToObject(root, "OSCILLATION", data->oscillation);
    *json = cJSON_Print(root);
}

void timeConvert(uint8_t *time, times_t *res)
{
    res->useconds = *(time + 3) | (*(time + 2)  << 8) | (*(time + 1)  << 16) | (*(time + 0)  << 24);
    res->hours = res->seconds / 3600000000;
    res->useconds = res->useconds - res->hours * 3600000000;
    res->minutes = res->useconds / 60000000;
    res->useconds = res->useconds - res->minutes * 60000000;
    res->seconds = res->useconds / 1000000;
    res->useconds = res->useconds - res->seconds * 1000000;
    res->mseconds = res->useconds / 1000;
    res->useconds = res->useconds - res->mseconds * 1000;
}
