#ifndef __TERMOMETER_H
#define __TERMOMETER_H

#include <stdint.h>
#include "SimpleGPIO.h"
#include "freertos/FreeRTOS.h"
#include "SimpleGPIO.h"
#include "esp_rom_sys.h"      // Proporciona esp_rom_delay_us()
#include "freertos/task.h"



class TERMOMETER {
public:
    // Constructor
    TERMOMETER();
    ~TERMOMETER();

    void setup(uint8_t pin);

    bool askTemperature();

    uint64_t readTemperature();

private:

    SimpleGPIO _io;       // Instancia interna de tu librería GPIO
    portMUX_TYPE _mux;    // Mutex para las secciones críticas de FreeRTOS

    // Métodos de bajo nivel de 1-Wire
    bool reset();
    void writeBit(uint8_t bit);
    uint8_t readBit();
    void writeByte(uint8_t data);
    uint8_t readByte();


};

#endif // __TERMOMETER_H