#ifndef _CButton_h
#define _CButton_h
 
extern "C" {
    typedef void (*ButtonEventHandler)();
}

class CButton {
    public:
        CButton(gpio_num_t p);
        void tick();
        void attachClick(ButtonEventHandler method);

    private:
        gpio_num_t pin;
        int prevState;
        ButtonEventHandler handler = NULL;
};

#endif
