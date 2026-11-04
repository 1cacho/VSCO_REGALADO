#include "WaterPump.h"

WATERPUMP::WATERPUMP() {};
WATERPUMP::~WATERPUMP() {};

void WATERPUMP::setup(const uint8_t pin, const uint8_t channel)
{

    // _pump_timer_config.frequency = 5000;

    pwm.setup(pin, channel, &_pump_timer_config, false); 
    
    pwm.setDuty(0.0f); 
}

uint64_t WATERPUMP::pour(float power, uint16_t mL, uint8_t pour_rate)
{
    pwm.setDuty(power);
    uint64_t pump_time_ms = mL*pour_rate;
    return pump_time_ms;
}

void WATERPUMP::stop() 
{
    pwm.setDuty(0.0f); 
}
