#ifndef __COLORSENSOR_H__
#define __COLORSENSOR_H__

#include "SimplePWM.h"
#include "SimpleTimer.h"
#include "SimpleADC.h"
#include "SimpleGPIO.h"

#define R_MAX 80
#define R_MIN 13
#define G_MAX 77
#define G_MIN 12
#define B_MAX 87
#define B_MIN 14

enum STATE_COLOR {
    R,
    G,
    B
};


class COLORSENSOR {
public:
    COLORSENSOR(){};
    ~COLORSENSOR(){};

    void setup(uint8_t READ_PIN, uint8_t S2_PIN, uint8_t S3_PIN, uint8_t RGB_PINS[3], uint8_t RGB_CHANNEL[3]);
    void setColor (STATE_COLOR rgb_color);
    void readColor (uint8_t rgb [3]);

    void display (uint32_t rgb [3]);
    void blink (uint32_t rgb [3]);


    static COLORSENSOR* instancia_color;
private:
    SimpleTimer timer;
    SimpleGPIO s2, s3, color_in;
    SimplePWM rgb_out[3];
    TimerConfig _timer_config_led;
};


#endif // __COLORSENSOR_H__