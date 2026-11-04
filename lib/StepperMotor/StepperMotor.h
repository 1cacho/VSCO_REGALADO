#ifndef __STEPPERMOTOR_H__
#define __STEPPERMOTOR_H__

#include "SimplePWM.h"
#include "SimpleGPIO.h"
#include "SimpleTimer.h"

#define PWM_FREQ 1000
#define STEPPER_PPR 200

enum STEPPER_DIR {
    up,
    down
};

enum STATE_STEPPER {
    moving,
    suspended
};

class STEPPERMOTOR
{
public:
    STEPPERMOTOR();
    ~STEPPERMOTOR();
    
    void setup(const uint8_t pin_dir, const uint8_t pin_pul, const uint8_t channel);
    uint64_t move(uint16_t distance, uint16_t ppr, STEPPER_DIR direction);
    void stop(); 

    static STEPPERMOTOR* instancia;

private:
    SimpleGPIO dir;
    SimplePWM pul;
    SimpleTimer timer;
    TimerConfig _stepper_timer_config;
};


#endif // __STEPPERMOTOR_H__