#ifndef _COMM_H__
#define _COMM_H__

#include "SimpleGPIO.h"
#include "SimplePWM.h"

class COMM
{
public:
    COMM();
    void setup(uint8_t ARDUINO_IN_PIN, uint8_t ARDUINO_OUT_PIN, const uint8_t channel);
    void send(uint32_t value);
    bool read();
    

private:
    SimpleGPIO arduino_in;
    SimplePWM arduino_out;
    TimerConfig _arduino_timer_config;

};

#endif // _COMM_H1__