#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

#include "SimplePWM.h"
#include "SimpleGPIO.h"
#include "esp_timer.h"


class DCMOTOR {
public:
    DCMOTOR();
    ~DCMOTOR();

    void setup (uint8_t gpio_num);
    void start ();
    void stop();
private:


    SimpleGPIO fan;
    SimplePWM pwm;

};

#endif // __DCMOTOR_H__
