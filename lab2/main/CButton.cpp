#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "CButton.h"

CButton::CButton(gpio_num_t p) {
    pin = p;
    
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    prevState = gpio_get_level(pin);
}

void CButton::attachClick(ButtonEventHandler method) {
    handler = method;
}

void CButton::tick() {
    int state = gpio_get_level(pin);

    if (state != prevState && state == 0 && handler != NULL)
        handler();

    prevState = state;
}
