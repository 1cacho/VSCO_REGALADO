#include "StepperMotor.h"

STEPPERMOTOR* STEPPERMOTOR::instancia = nullptr;
STEPPERMOTOR::STEPPERMOTOR(){};
STEPPERMOTOR::~STEPPERMOTOR(){};


void IRAM_ATTR timer_callback(void* arg) {
    if (STEPPERMOTOR::instancia != nullptr) {
        STEPPERMOTOR::instancia->stop(); 
    }
}

void STEPPERMOTOR::setup (const uint8_t pin_dir, const uint8_t pin_pul, const uint8_t channel) {
    
    dir.setup(pin_dir, GPIO_MODE_OUTPUT, GPIO_FLOATING);
    pul.setup(pin_pul, channel, &_stepper_timer_config, false); 
    pul.setDuty(0.0f); 
    pul.setFrequency(PWM_FREQ);

    instancia = this;
    timer.setup(timer_callback, "STEPPER_TIMER");
    
};

uint64_t STEPPERMOTOR::move(uint16_t distance, uint16_t ppr, STEPPER_DIR direction) {
    
    pul.setDuty(50.0f);
    dir.set(direction);


    uint32_t total_steps = (ppr / 8) * distance;

    uint64_t motor_time_ms = ((uint64_t)total_steps * 1000) / PWM_FREQ;

    timer.startOnce(motor_time_ms*1000);
    
    return motor_time_ms;

};

void STEPPERMOTOR::stop() {
    pul.setDuty(0);        // Apaga la señal PWM
};

