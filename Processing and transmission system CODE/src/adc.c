#include "main.h"
#include "adc.h"

static const char *TAG = "ADC";
static esp_err_t err;
static TickType_t xLastWakeTime;
static esp_adc_cal_characteristics_t adc1_chars;
static esp_timer_handle_t timer_handler;
static uint32_t voltage;

bool led_ON = true;
bool led_OFF = true;
bool led_BLINK = true;

void adc_init()
{
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    const esp_timer_create_args_t my_timer_args =
    {
        .callback = &timer_callback,
        .name = "My Timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 1));

    xLastWakeTime = xTaskGetTickCount ();

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11));
}

void VoltageRead_Task(void)
{
    for(;;)
    {
        voltage = 2 * esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_6), &adc1_chars);
        ESP_LOGI(TAG, "ADC1_CHANNEL_6: %d mV", voltage);
        if(voltage <= 1000 && led_OFF == true)
        {
            esp_timer_stop(timer_handler);
            gpio_set_level(LED_PIN, false);
            ESP_LOGI(TAG, "error =%d", err);
            led_OFF = false;
            led_BLINK = true;
            led_ON = true;
        }
        else if(voltage > 1001 && voltage <= 2350 && led_BLINK == true)
        {
            esp_timer_stop(timer_handler);
            err= esp_timer_start_periodic(timer_handler, 1000000);
            ESP_LOGI(TAG, "error =%d", err);
            led_OFF = true;
            led_BLINK = false;
            led_ON = true;
        }
        else if (voltage > 2351 && led_ON == true)
        {
            esp_timer_stop(timer_handler);
            err=  esp_timer_start_periodic(timer_handler, 1);
            ESP_LOGI(TAG, "error =%d", err);
            led_OFF = true;
            led_BLINK = true;
            led_ON = false;
        }
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(5000));
    }
}

void timer_callback(void *param)
{
  static bool ON;
  ON = !ON;
  
  gpio_set_level(LED_PIN, ON);
}

