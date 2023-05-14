#include <stdio.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#include "CDS1307.h"

#define DS1307_ADDRESS 0x68

CDS1307::CDS1307(gpio_num_t sda, gpio_num_t scl) {
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda;
    conf.scl_io_num = scl;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
}

uint8_t CDS1307::to_register(uint8_t x) {
    return ((x / 10) << 4) | (x % 10);
}

uint8_t CDS1307::to_value(uint8_t x) {
    return ((x >> 4) * 10) + (x & 0x0F);
}

void CDS1307::read(uint8_t start, uint8_t* data, size_t n) {
    if (n == 0) return;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (DS1307_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, start, 1));
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (DS1307_ADDRESS << 1) | I2C_MASTER_READ, 1));
    ESP_ERROR_CHECK(i2c_master_read(cmd, data, n, I2C_MASTER_LAST_NACK));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS));
    i2c_cmd_link_delete(cmd);
}

void CDS1307::write(uint8_t start, uint8_t* data, size_t n) {
    if (n == 0) return;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (DS1307_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, start, 1));
    ESP_ERROR_CHECK(i2c_master_write(cmd, data, n, 1));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS));
    i2c_cmd_link_delete(cmd);
}

time_t CDS1307::getTime() {
    uint8_t data[7];
    read(0, data, 7);

    struct tm tm;
    tm.tm_sec = to_value(data[0] & 0x7F);
    tm.tm_min = to_value(data[1]);
    tm.tm_hour = to_value(data[2]);
    tm.tm_mday = to_value(data[4]);
    tm.tm_mon = to_value(data[5]) - 1;
    tm.tm_year = to_value(data[6]) + 100;

    return mktime(&tm);
}

void CDS1307::setTime(time_t t) {
    struct tm tm;
    gmtime_r(&t, &tm);

    uint8_t data[7] = {
        to_register(tm.tm_sec),
        to_register(tm.tm_min),
        to_register(tm.tm_hour),
        to_register(tm.tm_wday + 1),
        to_register(tm.tm_mday),
        to_register(tm.tm_mon + 1),
        to_register(tm.tm_year - 100),
    };

    write(0, data, 7);
}
