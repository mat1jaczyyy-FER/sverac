#ifndef _CVMA320_h
#define _CVMA320_h

class CVMA320 {
    public:
        CVMA320(adc1_channel_t c);
        int temperature(double* result, const char** error);

    private:
        adc1_channel_t channel;
        esp_adc_cal_characteristics_t* adc_chars;
};

#endif
