#ifndef __RGB_H__
#define __RGB_H__

#include "SimplePWM.h"
#include <esp_timer.h>

class RGB
{

public:
    RGB();                                                                       //* constructor
    void setUp(uint8_t pins[3], uint8_t ch[3], TimerConfig *timer, bool invert); //* Setup del Led
    void setColor(uint8_t r, uint8_t g, uint8_t b);                              //*Metodos para seleccionar color
    void setColor(uint32_t colorCode);
    void blink(uint32_t  colorCode, int dt_us);//*Segundo metodo para definir el color
    void blink(uint8_t r, uint8_t g, uint8_t b, int dt_us);

private:
    SimplePWM pwm[3]; //* delaracion de objetos PWM
    int _last, _current;
    bool _on;
};

#endif // __RGB_H_
