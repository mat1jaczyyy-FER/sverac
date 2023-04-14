#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_adc_cal.h"

const char *LogName = "MAIN";

#include "CButton.h"
#include "CDHT22.h"
#include "CVMA320.h"

CButton button(GPIO_NUM_13);
CDHT22 dht22(GPIO_NUM_15);
CVMA320 vma320(ADC1_CHANNEL_6);

#define PROCESS(sensor) \
do { \
    double result; \
    const char* error; \
    if ((sensor).temperature(&result, &error)) { \
        ESP_LOGI(LogName, "%6s: %5.1lf°C ", #sensor, result); \
    } else { \
        ESP_LOGE(LogName, "%6s: %s ", #sensor, error); \
    } \
} while(0)

void handleClick() {
    PROCESS(dht22);
    PROCESS(vma320);
}

extern "C" void app_main() {
    button.attachClick(handleClick);

    while (1) {
        button.tick();
        vTaskDelay(10 / portTICK_PERIOD_MS);   
    }
}
