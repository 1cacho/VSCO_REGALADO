#include "Termometer.h"

TERMOMETER::TERMOMETER(){};
TERMOMETER::~TERMOMETER(){};

void TERMOMETER::setup(uint8_t pin) {
    portMUX_TYPE mux_temp = portMUX_INITIALIZER_UNLOCKED;
    _mux = mux_temp;
    _io.setup(pin, GPIO_MODE_INPUT_OUTPUT_OD, GPIO_PULLUP_ONLY);
}

bool TERMOMETER::askTemperature() {
    if (!reset()) return false;
    writeByte(0xCC); // Skip ROM
    writeByte(0x44); // Convert T
    return true;
}

uint64_t TERMOMETER::readTemperature() {
    // Si no hay respuesta del sensor, devolvemos un valor de error reconocible
    if (!reset()) return 0xFFFFFFFFFFFFFFFF; 
    
    writeByte(0xCC); // Skip ROM
    writeByte(0xBE); // Read Scratchpad

    uint8_t lsb = readByte();
    uint8_t msb = readByte();

    // Ensamble de 16 bits
    int16_t raw = (int16_t)((msb << 8) | lsb);

    uint64_t temp_escalada = (uint64_t)(raw * 625);
    return temp_escalada;
}

bool TERMOMETER::reset() {
    _io.set(0);                             
    esp_rom_delay_us(480);                  
    
    portENTER_CRITICAL(&_mux);
    _io.set(1);                             
    esp_rom_delay_us(70);                   
    int presence = _io.get();               
    portEXIT_CRITICAL(&_mux);

    esp_rom_delay_us(410);                  
    return (presence == 0);
}

void TERMOMETER::writeBit(uint8_t bit) {
    if (bit & 1) {
        portENTER_CRITICAL(&_mux);
        _io.set(0);
        esp_rom_delay_us(3);
        _io.set(1);
        portEXIT_CRITICAL(&_mux);
        esp_rom_delay_us(60); 
    } else {
        portENTER_CRITICAL(&_mux);
        _io.set(0);
        esp_rom_delay_us(65);
        _io.set(1);
        portEXIT_CRITICAL(&_mux);
        esp_rom_delay_us(5);
    }
}

uint8_t TERMOMETER::readBit() {
    uint8_t bit = 0;
    portENTER_CRITICAL(&_mux);
    _io.set(0);                             
    esp_rom_delay_us(2);
    _io.set(1);                             
    esp_rom_delay_us(10);                   
    bit = _io.get();
    portEXIT_CRITICAL(&_mux); 
    esp_rom_delay_us(50);                   
    return bit;
}

void TERMOMETER::writeByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        writeBit(data & 0x01);
        data >>= 1;
    }
}

uint8_t TERMOMETER::readByte() {
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        if (readBit()) {
            data |= (1 << i);
        }
    }
    return data;
}