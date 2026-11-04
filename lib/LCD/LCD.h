#ifndef __LCD_H__
#define __LCD_H__

#include <stdint.h>
#include "driver/i2c_master.h"

// Same commands as the NibbleLCD library from class
#define CMD_CLEAR       0b00000001
#define CMD_ENTRY_MODE  0b00000110
#define CMD_DISPLAY_ON  0b00001100
#define CMD_FUNC_SET    0b00101000   // 4-bit mode, 2 lines

#define LCD_COLUMNS 20

class LCD
{
public:
    void setup(uint8_t sda_pin, uint8_t scl_pin);
    void writeCommand(uint8_t command);
    void print(char character);
    void printLine(uint8_t row, const char text[]);

private:
    i2c_master_dev_handle_t _lcd;
    void sendNibble(uint8_t nibble, uint8_t rs);
};

#endif // __LCD_H__
