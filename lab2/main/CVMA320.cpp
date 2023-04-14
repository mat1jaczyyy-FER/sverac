#include <stdio.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_adc_cal.h"

#include "common.h"
#include "CVMA320.h"

CVMA320::CVMA320(adc1_channel_t c) {
    channel = c;
    
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, ADC_ATTEN_DB_11);

    adc_chars = (esp_adc_cal_characteristics_t*)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1156, adc_chars);
}

// https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html
// -24°C => 124146.341463 Ω
//  28°C =>  11317.829457 Ω
//  80°C =>   1917.659805 Ω

const double A = 0.8889912488e-3;
const double B = 2.503324574e-4;
const double C = 1.167865553e-7;

#define ADC_SAMPLES 10

int CVMA320::temperature(double* result, const char** error) {
    ASSERT(result, "Result pointer is NULL");
    
    double mV = 0;
    for (int i = 0; i < ADC_SAMPLES; i++)
        mV += (double)esp_adc_cal_raw_to_voltage(adc1_get_raw(channel), adc_chars) / ADC_SAMPLES;

    double lnR = log(10000 * mV / (3300 - mV));
    *result = 1.0 / (A + B * lnR + C * (lnR * lnR * lnR)) - 273.15;

    return 1;
}
