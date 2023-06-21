#ifndef __SPI_H
#define __SPI_H

#include "driver/spi_master.h"
#include "driver/gpio.h"

#define SPI_CLOCK_SPEED_HZ 100000
#define SPI_DUTY_CYCLE_POS 128

#define HSPI_MISO 12 
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_CS0 15 // 25
#define HSPI_CS1 10 // 26
#define HSPI_CS2 9 // 27

#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCLK 18 
#define VSPI_CS0 5
#define VSPI_CS1 2 // 14
#define VSPI_CS2 4 // 16
#define STLD 21

typedef struct{
    spi_device_interface_config_t turncfg;
    spi_device_interface_config_t slantcfg; 
    spi_device_interface_config_t offsetcfg;
    spi_device_interface_config_t timecfg;
    spi_device_interface_config_t oscillationcfg;

    spi_device_handle_t turnHandle;
    spi_device_handle_t slantHandle;
    spi_device_handle_t offsetHandle;
    spi_device_handle_t timeHandle;
    spi_device_handle_t oscillationHandle;
}devices_t;


void gpio_set(gpio_num_t gpio, gpio_mode_t mode, uint32_t level);
void latch(gpio_num_t gpio, uint8_t set);
void spi_init(devices_t* devices);
esp_err_t spiRead(unsigned char *array, uint8_t lengthAdrray, spi_device_handle_t *spiHandle_p);

#endif