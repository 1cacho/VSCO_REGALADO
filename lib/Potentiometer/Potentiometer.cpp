#include "Potentiometer.h"
#include "esp_rom_sys.h"

#define UP   1
#define DOWN 0

void POTENTIOMETER::setup(uint8_t inc_pin, uint8_t ud_pin)
{
    _inc.setup(inc_pin, GPO);
    _ud.setup(ud_pin, GPO);
    _inc.set(1);

    // At power-up the position is unknown: go down 100 steps to reach 0
    step(DOWN, 100);
    _position = 0;
}

void POTENTIOMETER::set(int position)
{
    if (position < 0)
        position = 0;
    if (position > 99)
        position = 99;

    if (position > _position)
        step(UP, position - _position);
    if (position < _position)
        step(DOWN, _position - position);

    _position = position;
}

int POTENTIOMETER::get()
{
    return _position;
}

void POTENTIOMETER::step(int direction, int steps)
{
    _ud.set(direction);
    for (int i = 0; i < steps; i++) {
        _inc.set(0);              // falling edge = 1 step
        esp_rom_delay_us(5);
        _inc.set(1);
        esp_rom_delay_us(5);
    }
}
