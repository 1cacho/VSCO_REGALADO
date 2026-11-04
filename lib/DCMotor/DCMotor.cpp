#include "DCMotor.h"

DCMOTOR::DCMOTOR () {}
DCMOTOR::~DCMOTOR (){}


enum DIRECTION {
    up, 
    down
};

void DCMOTOR::setup(uint8_t gpio_num)
{
        fan.setup(gpio_num, GPIO_MODE_OUTPUT, GPIO_PULLUP_ONLY);
        fan.set(0);
};

void DCMOTOR::start()
{
    fan.set(1);
};


void DCMOTOR::stop() {
    fan.set(0);
};
