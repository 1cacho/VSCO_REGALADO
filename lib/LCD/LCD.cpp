#include "LCD.h"
#include "esp_rom_sys.h"

// I2C backpack (PCF8574): P0 = RS, P2 = EN, P3 = backlight, P4-P7 = D4-D7
#define LCD_ADDRESS 0x27     // use 0x3F if the screen shows nothing
#define BIT_RS      0b00000001
#define BIT_EN      0b00000100
#define BIT_LIGHT   0b00001000

// First address of each row of a 20x4 LCD
uint8_t row_address[4] = {0x00, 0x40, 0x14, 0x54};

void LCD::setup(uint8_t sda_pin, uint8_t scl_pin)
{
    i2c_master_bus_handle_t bus;
    i2c_master_bus_config_t bus_config = {};
    bus_config.i2c_port = I2C_NUM_0;
    bus_config.sda_io_num = (gpio_num_t)sda_pin;
    bus_config.scl_io_num = (gpio_num_t)scl_pin;
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.glitch_ignore_cnt = 7;
    bus_config.flags.enable_internal_pullup = true;
    i2c_new_master_bus(&bus_config, &bus);

    i2c_device_config_t lcd_config = {};
    lcd_config.device_address = LCD_ADDRESS;
    lcd_config.scl_speed_hz = 100000;
    i2c_master_bus_add_device(bus, &lcd_config, &_lcd);

    // Start-up sequence for 4-bit mode (HD44780 datasheet)
    esp_rom_delay_us(50000);
    sendNibble(0x30, 0);
    esp_rom_delay_us(5000);
    sendNibble(0x30, 0);
    esp_rom_delay_us(200);
    sendNibble(0x30, 0);
    esp_rom_delay_us(200);
    sendNibble(0x20, 0);

    writeCommand(CMD_FUNC_SET);
    writeCommand(CMD_DISPLAY_ON);
    writeCommand(CMD_ENTRY_MODE);
    writeCommand(CMD_CLEAR);
    esp_rom_delay_us(2000);
}

// A command is sent in two nibbles with RS = 0
void LCD::writeCommand(uint8_t command)
{
    sendNibble(command & 0xF0, 0);
    sendNibble((command << 4) & 0xF0, 0);
}

// A character is sent in two nibbles with RS = 1
void LCD::print(char character)
{
    sendNibble(character & 0xF0, BIT_RS);
    sendNibble((character << 4) & 0xF0, BIT_RS);
}

// Writes a whole row (20 characters), fills the rest with spaces
void LCD::printLine(uint8_t row, const char text[])
{
    writeCommand(0x80 + row_address[row]);
    bool end_of_text = false;
    for (uint8_t i = 0; i < LCD_COLUMNS; i++) {
        if (text[i] == '\0')
            end_of_text = true;
        if (end_of_text)
            print(' ');
        else
            print(text[i]);
    }
}

// Puts the nibble on D4-D7 and makes a pulse on EN
void LCD::sendNibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data[3];
    data[0] = nibble | rs | BIT_LIGHT;
    data[1] = nibble | rs | BIT_LIGHT | BIT_EN;   // EN = 1
    data[2] = nibble | rs | BIT_LIGHT;            // EN = 0 -> LCD reads the data
    i2c_master_transmit(_lcd, data, 3, 50);
}
