#ifndef __MQTT_H
#define __MQTT_H

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "freertos/event_groups.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define SSID_IPHONE "iPhone (Александр)"
#define PASSWORD_IPHONE "13575946"
#define SSID_HOME "HUAWEI-0413DP_Wi-Fi5"
#define PASSWORD_HOME "78GkRR59"
#define URI_IPHONE "mqtt://172.20.10.3"
#define URI_HOME "mqtt://192.168.3.8"

#define CONNECTED BIT0
#define SUBSCRIBED BIT1
#define START BIT2


void log_error_if_nonzero(const char *message, int error_code);
esp_err_t wifi_event_handler(void *ctx, system_event_t *event);
void wifi_init(void);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void mqtt_init(EventGroupHandle_t **mqttEvent, esp_mqtt_client_handle_t * client_p);
#endif