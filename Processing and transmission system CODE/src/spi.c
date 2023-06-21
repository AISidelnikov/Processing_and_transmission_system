
#include "main.h"
#include "spi.h"

static const char *TAG_SPI = "SPI";
static esp_err_t err;

void gpio_set(gpio_num_t gpio, gpio_mode_t mode, uint32_t level)
{
    gpio_pad_select_gpio(gpio);
    ESP_ERROR_CHECK(gpio_set_direction(gpio, mode));
    ESP_ERROR_CHECK(gpio_set_level(gpio, level));
}

void latch(gpio_num_t gpio, uint8_t set)
{
    if(set == 0)
    {
        ESP_ERROR_CHECK(gpio_set_level(gpio, 0));
        vTaskDelay(100 / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(gpio_set_level(gpio, 1));
    }
    else if(set == 1)
    {
        ESP_ERROR_CHECK(gpio_set_level(gpio, 1));
        vTaskDelay(100 / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(gpio_set_level(gpio, 0));
    }
}

void spi_init(devices_t *devices_p)
{
    spi_bus_config_t hbuscfg = {
        .miso_io_num = HSPI_MISO,
        .mosi_io_num = HSPI_MOSI,
        .sclk_io_num = HSPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .flags = SPICOMMON_BUSFLAG_MASTER,
    };

    spi_bus_config_t vbuscfg = {
        .miso_io_num = VSPI_MISO,
        .mosi_io_num = VSPI_MOSI,
        .sclk_io_num = VSPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .flags = SPICOMMON_BUSFLAG_MASTER,
    };

    err = spi_bus_initialize(HSPI_HOST, &hbuscfg, SPI_DMA_CH1);
    ESP_LOGI(TAG_SPI, "HSPI init: %d ", err);
    err = spi_bus_initialize(VSPI_HOST, &vbuscfg, SPI_DMA_CH2);
    ESP_LOGI(TAG_SPI, "VSPI init: %d ", err);

    devices_p->turncfg.address_bits = 0;     
    devices_p->turncfg.command_bits = 0;
    devices_p->turncfg.dummy_bits = 0;
    devices_p->turncfg.mode = 0;
    devices_p->turncfg.duty_cycle_pos = SPI_DUTY_CYCLE_POS;
    devices_p->turncfg.clock_speed_hz = SPI_CLOCK_SPEED_HZ;
    devices_p->turncfg.spics_io_num = HSPI_CS0;
    devices_p->turncfg.queue_size = 1;
    devices_p->turncfg.pre_cb = NULL;
    devices_p->turncfg.post_cb = NULL;
    devices_p->turncfg.flags = SPI_DEVICE_HALFDUPLEX;

    devices_p->slantcfg.address_bits = 0;
    devices_p->slantcfg.command_bits = 0;
    devices_p->slantcfg.dummy_bits = 0;
    devices_p->slantcfg.mode = 0;
    devices_p->slantcfg.duty_cycle_pos = SPI_DUTY_CYCLE_POS;
    devices_p->slantcfg.clock_speed_hz = SPI_CLOCK_SPEED_HZ;
    devices_p->slantcfg.spics_io_num = HSPI_CS1;
    devices_p->slantcfg.queue_size = 1;
    devices_p->slantcfg.pre_cb = NULL;
    devices_p->slantcfg.post_cb = NULL;
    devices_p->slantcfg.flags = SPI_DEVICE_HALFDUPLEX;

    devices_p->offsetcfg.address_bits = 0;
    devices_p->offsetcfg.command_bits = 0;
    devices_p->offsetcfg.dummy_bits = 0;
    devices_p->offsetcfg.mode = 0;
    devices_p->offsetcfg.duty_cycle_pos = SPI_DUTY_CYCLE_POS;
    devices_p->offsetcfg.clock_speed_hz = SPI_CLOCK_SPEED_HZ;
    devices_p->offsetcfg.spics_io_num = HSPI_CS2;
    devices_p->offsetcfg.queue_size = 1;
    devices_p->offsetcfg.pre_cb = NULL;
    devices_p->offsetcfg.post_cb = NULL;
    devices_p->offsetcfg.flags = SPI_DEVICE_HALFDUPLEX;

    devices_p->timecfg.address_bits = 0;
    devices_p->timecfg.command_bits = 0;
    devices_p->timecfg.dummy_bits = 0;
    devices_p->timecfg.mode = 0;
    devices_p->timecfg.duty_cycle_pos = SPI_DUTY_CYCLE_POS;
    devices_p->timecfg.clock_speed_hz = SPI_CLOCK_SPEED_HZ;
    devices_p->timecfg.spics_io_num = VSPI_CS0;
    devices_p->timecfg.queue_size = 1;
    devices_p->timecfg.pre_cb = NULL;
    devices_p->timecfg.post_cb = NULL;
    devices_p->timecfg.flags = SPI_DEVICE_HALFDUPLEX;

    devices_p->oscillationcfg.address_bits = 0;
    devices_p->oscillationcfg.command_bits = 0;
    devices_p->oscillationcfg.dummy_bits = 0;
    devices_p->oscillationcfg.mode = 0;
    devices_p->oscillationcfg.duty_cycle_pos = SPI_DUTY_CYCLE_POS;
    devices_p->oscillationcfg.clock_speed_hz = SPI_CLOCK_SPEED_HZ;
    devices_p->oscillationcfg.spics_io_num = VSPI_CS1;
    devices_p->oscillationcfg.queue_size = 1;
    devices_p->oscillationcfg.pre_cb = NULL;
    devices_p->oscillationcfg.post_cb = NULL;
    devices_p->oscillationcfg.flags = SPI_DEVICE_HALFDUPLEX;

    err = spi_bus_add_device(HSPI_HOST, &devices_p->turncfg, &devices_p->turnHandle);
    ESP_LOGI(TAG_SPI,"SPI BUS ADD TURN: %d ", err);
    err = spi_bus_add_device(HSPI_HOST, &devices_p->slantcfg, &devices_p->slantHandle);
    ESP_LOGI(TAG_SPI,"SPI BUS ADD SLANG: %d ", err);
    err = spi_bus_add_device(HSPI_HOST, &devices_p->offsetcfg, &devices_p->offsetHandle);
    ESP_LOGI(TAG_SPI,"SPI BUS ADD OFFSET: %d ", err);
    err = spi_bus_add_device(VSPI_HOST, &devices_p->timecfg, &devices_p->timeHandle);
    ESP_LOGI(TAG_SPI,"SPI BUS ADD TIME: %d ", err);
    err = spi_bus_add_device(VSPI_HOST, &devices_p->oscillationcfg, &devices_p->oscillationHandle);
    ESP_LOGI(TAG_SPI,"SPI BUS ADD OSCILLATION: %d ", err);
}

esp_err_t spiRead(unsigned char *array, uint8_t lengthAdrray, spi_device_handle_t *spiHandle_p)
{
    spi_transaction_t trans = {
        .addr = 0,
        .cmd = 0,
        .length = 8*lengthAdrray,
        .rxlength = 8*lengthAdrray,
        .rx_buffer = array,
    };
    err = spi_device_transmit(*spiHandle_p, &trans);
    ESP_LOGI(TAG_SPI,"SPI TRANSMIT: %d ", err);
    return err;
}