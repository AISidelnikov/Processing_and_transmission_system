#include "main.h"
#include "mqtt.h"

static const char *TAG_MQTT = "MQTT";
static EventGroupHandle_t wifiEventGroup;
static EventGroupHandle_t mqttEventGroup;


void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
      ESP_LOGE(TAG_MQTT, "Last error %s: 0x%x", message, error_code);
    }
}

void wifi_init(void)
{
    tcpip_adapter_init();
    wifiEventGroup = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID_IPHONE,
            .password = PASSWORD_IPHONE,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(TAG_MQTT, "start the WIFI SSID:[%s] password:[%s]", SSID_IPHONE, "******");
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG_MQTT, "Waiting for wifi");
    xEventGroupWaitBits(wifiEventGroup, CONNECTED, false, true, portMAX_DELAY);
}

esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            ESP_LOGI(TAG_MQTT, "TRYING TO CONNECT WITH Wi-Fi");    
        break;

        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGE(TAG_MQTT, "Wi-Fi CONNECTED");
        break;

        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifiEventGroup, CONNECTED);
        break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG_MQTT, "Wi-Fi DISCONNECTED, RETRYING Wi-Fi");
            esp_wifi_connect();
            xEventGroupClearBits(wifiEventGroup, CONNECTED);
        break;
        default:
        break;
    }
    return ESP_OK;
}

void mqtt_init(EventGroupHandle_t **mqttEvent, esp_mqtt_client_handle_t* client_p)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = URI_IPHONE,
        .password = "ESP32",
        .username = "ESP32",
    };
    *client_p = esp_mqtt_client_init(&mqtt_cfg);
    mqttEventGroup = xEventGroupCreate();
    *mqttEvent = &mqttEventGroup;
    esp_mqtt_client_register_event(*client_p, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(*client_p);
}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG_MQTT, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
        case MQTT_EVENT_CONNECTED:
            xEventGroupSetBits(mqttEventGroup, CONNECTED);
            ESP_LOGE(TAG_MQTT, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "test", 2);
            ESP_LOGI(TAG_MQTT, "sent subscribe successful, msg_id=%d", msg_id);
        break;
        case MQTT_EVENT_DISCONNECTED:
            xEventGroupClearBits(mqttEventGroup, CONNECTED);
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DISCONNECTED");
        break;
        case MQTT_EVENT_SUBSCRIBED:
            xEventGroupSetBits(mqttEventGroup, SUBSCRIBED);
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
        case MQTT_EVENT_UNSUBSCRIBED:
            xEventGroupClearBits(mqttEventGroup, SUBSCRIBED);
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DATA");
            if((strncmp(event->data, "START", 5)) == 0)
            {
                xEventGroupSetBits(mqttEventGroup, START);
            }
            else if((strncmp(event->data, "STOP", 4)) == 0)
            {
                printf("STOP: %d", xEventGroupClearBits(mqttEventGroup, START));
            }
            // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            // printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG_MQTT, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
        break;
        default:
            ESP_LOGI(TAG_MQTT, "Other event id:%d", event->event_id);
        break;
    }
}