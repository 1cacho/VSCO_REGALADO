#include "RGB.h"

RGB::RGB()
{
}

void RGB::setUp(uint8_t pins[3], uint8_t ch[3], TimerConfig *timer, bool invert)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        pwm[i].setup(pins[i], ch[i], timer, invert);
    }
}

void RGB::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    pwm[0].setDigitalLevel(r);
    pwm[1].setDigitalLevel(g);
    pwm[2].setDigitalLevel(b);
}

void RGB::setColor(uint32_t colorCode)
{
    setColor(colorCode >> 16 & 0xFF, colorCode >> 8 & 0xFF, colorCode & 0xFF);
}

void RGB::blink(uint32_t colorCode, int dt_us = 0.5e6)
{
    _current = esp_timer_get_time();
    if(_current - _last > dt_us){
        _last = _current;
        this->setColor(colorCode);
    } else {
        this->setColor(0x000000);
    }
}
 void RGB::blink (uint8_t r, uint8_t g, uint8_t b, int dt_us) 
{
     _current = esp_timer_get_time();
    if(_current - _last > dt_us){
        _last = _current;
        this->setColor(r,g,b);
    } else {
        this->setColor(0, 0, 0);
    }

}