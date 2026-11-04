#ifndef __POTENTIOMETER_H__
#define __POTENTIOMETER_H__

// X9C103 digital potentiometer (positions 0 to 99)
// CS is wired to GND (always selected), U/D = direction, each INC pulse = 1 step

#include "SimpleGPIO.h"

class POTENTIOMETER
{
public:
    void setup(uint8_t inc_pin, uint8_t ud_pin);
    void set(int position);
    int get();

private:
    SimpleGPIO _inc, _ud;
    int _position = 0;
    void step(int direction, int steps);
};

#endif // __POTENTIOMETER_H__
