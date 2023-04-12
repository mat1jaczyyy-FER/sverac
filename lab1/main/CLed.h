// CLed.h

#ifndef _CLed_h
#define _CLed_h

#define BLINK_GPIO CONFIG_BLINK_GPIO

enum LedStatus{ OFF, ON, BLINK, FAST_BLINK, SLOW_BLINK};


#define LED_BLINK_FAST   250000L
#define LED_BLINK       1000000L
#define LED_BLINK_SLOW  2000000L

class CLed{
    
    public:
        CLed(int port);
        void tick();
        void setLedState(LedStatus x);
    
    private:
        LedStatus m_state = OFF;
        gpio_num_t m_pinNumber;
        const char *LogName = "CLed";
        int64_t  m_lastBlink = 0;
        int m_lastBlinkState = 0;
};
#endif