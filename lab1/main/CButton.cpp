#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "CButton.h"

//
// CButton construction function
//
CButton::CButton(int port){

    //ToDo::
    //BOOT is GPIO0 (HIGH when released, LOW when pressed)

    //Config Port on constructor
    m_pinNumber = (gpio_num_t)port;
    ESP_LOGI(LogName, "Configure port[%d] to input!!!", port);
    gpio_reset_pin(m_pinNumber);
    /* Set the GPIO as a input */
    gpio_set_direction(m_pinNumber, GPIO_MODE_INPUT);

    prevState = gpio_get_level(m_pinNumber);
}

//
// Tick method, need to be called periodically
//
void CButton::tick(){
    //ToDo
    globalTickTimer++;
    int state = gpio_get_level(m_pinNumber);

    // 1 is released, 0 is pressed
    //ESP_LOGI(LogName, "Button state[%d] - prevState[%d]", state, prevState);

    if (state != prevState && state == 0) {
        //ESP_LOGI(LogName, "Rising edge detected");
        if (globalTickTimer - lastPress < 35) {
            //ESP_LOGI(LogName, "Double click detected");
            if(doubleClick != NULL) doubleClick();
        } else {
            //ESP_LOGI(LogName, "Single click detected");
            if(singleClick != NULL) singleClick();
        }
        lastPress = globalTickTimer;

    } else if (globalTickTimer - lastPress == 70 && state == 0) {
        //ESP_LOGI(LogName, "Long press detected");
        if(longPress != NULL) longPress();
    }

    prevState = state;
}
