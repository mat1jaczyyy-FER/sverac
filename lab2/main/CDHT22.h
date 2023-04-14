#ifndef _CDHT22_h
#define _CDHT22_h

class CDHT22 {
    public:
        CDHT22(gpio_num_t p);
        int temperature(double* result, const char** error);

    private:
        gpio_num_t pin;
        int wait_for_state(int timeout, int state, int* duration);
        int wait_for_state(int timeout, int state);
};

#endif
