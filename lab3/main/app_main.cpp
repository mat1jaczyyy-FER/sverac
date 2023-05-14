#include <stdio.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

const char *LogName = "DS1307";

#include "CDS1307.h"
CDS1307 ds1307(GPIO_NUM_18, GPIO_NUM_19);

extern "C" void app_main() {
    while (1) {
        time_t t = ds1307.getTime();

        struct tm tm;
        gmtime_r(&t, &tm);

        ESP_LOGI(LogName, "%ld - %04d-%02d-%02d %02d:%02d:%02d", t,
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
