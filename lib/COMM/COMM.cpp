#include "COMM.h"

COMM::COMM(){};

void COMM::setup (uint8_t ARDUINO_IN_PIN, uint8_t ARDUINO_OUT_PIN, const uint8_t channel) {
    arduino_in.setup(ARDUINO_IN_PIN, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);
    arduino_out.setup(ARDUINO_OUT_PIN, channel, &_arduino_timer_config, false);
};

bool COMM::read() {
    bool present_sample = arduino_in.get();
    return present_sample;
};

void COMM::send(uint32_t value) {
    arduino_out.setDuty(value);
};