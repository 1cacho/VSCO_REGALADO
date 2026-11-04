#ifndef __WATERPUMP_H__
#define __WATERPUMP_H__

#include "SimplePWM.h"
#include "SimpleGPIO.h"
#include "esp_timer.h"

enum DIRECTION
{
    fill,
    empty
};

enum STATE_PUMP
{
    stop,
    pouring
};

class WATERPUMP
{
public:
    WATERPUMP();
    ~WATERPUMP();
    
    // Solo requerimos el pin de salida PWM y el canal
    void setup(const uint8_t pin, const uint8_t channel);
    
    // Control de potencia (0.0 a 100.0)
    uint64_t pour(float power, uint16_t mL, uint8_t pour_rate);
    void stop();

private:
    SimplePWM pwm;
    TimerConfig _pump_timer_config;


};

#endif // __WATERPUMP_H__

