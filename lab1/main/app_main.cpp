/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "CLed.h"
#include "CButton.h"

static const char *TAG = "MAIN";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO 2 //CONFIG_BLINK_GPIO
#define BUTTON_GPIO 4

//
//Task LED Loop
//
void taskLed_loop (void *parameters)
{
    ESP_LOGI(TAG, "Start TASK LED Loop.");
    
    CLed *led;
    //Cast parameter
    led = (CLed*)parameters;
    ESP_LOGI(TAG, "Get Led pointer.");
    
    while(1) {
        //Do tick
        led->tick();  
        vTaskDelay(100 / portTICK_PERIOD_MS);      
    }
}

//
//Task Button Loop
//
void taskBtn_loop (void *parameters)
{
    ESP_LOGI(TAG, "Start TASK Button Loop.");
    
    CButton *button;
    //Cast parameter
    button = (CButton*)parameters;
    ESP_LOGI(TAG, "Get Button pointer.");
    
    while(1) {
        //Do tick
        button->tick();  
        vTaskDelay(10 / portTICK_PERIOD_MS);      
    }
}

TaskHandle_t xHandleLED = NULL;
TaskHandle_t xHandleButton = NULL;

void handleSingleClick(void) {
    ESP_LOGI(TAG, "Single Click Event Raised.");
}

void handleDoubleClick(void) {
    ESP_LOGI(TAG, "Double Click Event Raised.");
}

void handleLongPress(void) {
    ESP_LOGI(TAG, "Long Press Event Raised.");
}

//ESP32 mian function
extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Start MAIN.");
    
    //Create CLed object
    CLed led1(BLINK_GPIO);

    led1.setLedState(LedStatus::BLINK);

    //Create Task
    
    ESP_LOGI(TAG, "Start Task Create (LED).");
    xTaskCreate(taskLed_loop,      //Task function
                "ledLoop",      //Name of task in task scheduler
                1024*5,         //Stack size
                (void*)&led1,   //Parameter send to function
                1,              //Priority
                &xHandleLED);      //task handler 
    ESP_LOGI(TAG, "Task Created (LED)."); 

    //Create CButton object
    CButton button1(BUTTON_GPIO);

    button1.attachSingleClick(handleSingleClick);
    button1.attachDoubleClick(handleDoubleClick);
    button1.attachLongPress(handleLongPress);

    //Create Task
    
    ESP_LOGI(TAG, "Start Task Create (Button).");
    xTaskCreate(taskBtn_loop,      //Task function
                "btnLoop",      //Name of task in task scheduler
                1024*5,         //Stack size
                (void*)&button1,//Parameter send to function
                1,              //Priority
                &xHandleButton);      //task handler 
    ESP_LOGI(TAG, "Task Created (Button)."); 
    
    //Main loop
    while(1) {
        led1.setLedState(LedStatus::BLINK);
        vTaskDelay(10000 / portTICK_PERIOD_MS);

        led1.setLedState(LedStatus::FAST_BLINK);
        vTaskDelay(10000 / portTICK_PERIOD_MS);

        led1.setLedState(LedStatus::SLOW_BLINK);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }

}
