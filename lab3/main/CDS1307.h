#ifndef _CDS1307_h
#define _CDS1307_h

#include "driver/i2c.h"

class CDS1307 {
    public:
        CDS1307(gpio_num_t sda, gpio_num_t scl);
        void read(uint8_t start, uint8_t* data, size_t n);
        void write(uint8_t start, uint8_t* data, size_t n);
        time_t getTime();
        void setTime(time_t t);

    private:
        uint8_t to_register(uint8_t x);
        uint8_t to_value(uint8_t x);
};

#endif
