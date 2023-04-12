#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "CLed.h"

//
// CLED construction function
//
CLed::CLed(int port){
    //Config Port on constructor
    m_pinNumber = (gpio_num_t)port;
    ESP_LOGI(LogName, "Configure port[%d] to output!!!", port);
    gpio_reset_pin(m_pinNumber);
    /* Set the GPIO as a output */
    gpio_set_direction(m_pinNumber, GPIO_MODE_OUTPUT);
}

//
// Tick method, need to be called periodically
//
void CLed::tick(){
    
    switch(m_state){
        case OFF:
            //ESP_LOGI(LogName, "LED OFF.");
            gpio_set_level(m_pinNumber, 0);
            //vTaskDelay(1000 / portTICK_RATE_MS);
            break;
        case ON:
            //ESP_LOGI(LogName, "LED ON.");
            gpio_set_level(m_pinNumber, 1);
            break;
        case BLINK:
            if((esp_timer_get_time() - m_lastBlink) > LED_BLINK){
                m_lastBlink = esp_timer_get_time();
                m_lastBlinkState = (m_lastBlinkState == 0) ? 1 : 0;
                ESP_LOGI(LogName, "LED BLINK - STATE[%d]",m_lastBlinkState);
                gpio_set_level(m_pinNumber, m_lastBlinkState);
            };
            break;
        case SLOW_BLINK:
            if((esp_timer_get_time() - m_lastBlink) > LED_BLINK_SLOW){                
                m_lastBlink = esp_timer_get_time();
                m_lastBlinkState = (m_lastBlinkState == 0) ? 1 : 0;
                ESP_LOGI(LogName, "LED BLINK SLOW - STATE[%d]",m_lastBlinkState);
                gpio_set_level(m_pinNumber, m_lastBlinkState);
            };
            break;
        case FAST_BLINK:
            if((esp_timer_get_time() - m_lastBlink) > LED_BLINK_FAST){                
                m_lastBlink = esp_timer_get_time();
                m_lastBlinkState = (m_lastBlinkState == 0) ? 1 : 0;
                ESP_LOGI(LogName, "LED BLINK FAST - STATE[%d]",m_lastBlinkState);
                gpio_set_level(m_pinNumber, m_lastBlinkState);
            };
            break;
        default:
            //gpio_set_level(m_pinNumber, 1);
            break;
    }
}

//
// Set LED mode
//
void CLed::setLedState(LedStatus x){
    //const char str_tmp[50];
    //snprintf(str_tmp, 50, "Set LED state to %d", x);
    ESP_LOGI(LogName, "Set LED state %d", x);
    m_state = x;
    
       
}