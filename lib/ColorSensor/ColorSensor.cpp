#include "ColorSensor.h"

// 1. Variable global volátil para contar los pulsos en segundo plano
volatile uint32_t pulses = 0;

// 2. La función ISR pura (fuera de la clase)
void IRAM_ATTR color_sensor_isr(void* arg) {
    // Solo incrementamos el contador, ignorando el debounce
    pulses = pulses + 1;
}

int64_t map_value(int64_t x, int64_t in_min, int64_t in_max, int64_t out_min, int64_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void COLORSENSOR:: setup(uint8_t COLOR_PIN, uint8_t S2_PIN, uint8_t S3_PIN, uint8_t RGB_PINS [3], uint8_t RGB_CHANNEL[3]) {
    color_in.setup(COLOR_PIN, GPIO_MODE_INPUT, GPIO_FLOATING);
    s2.setup(S2_PIN, GPIO_MODE_OUTPUT, GPIO_FLOATING);   
    s3.setup(S3_PIN, GPIO_MODE_OUTPUT, GPIO_FLOATING);
    //color_in.addInterrupt(GPIO_INTR_POSEDGE, color_sensor_isr, 0);
    for (int i = 0; i < 3; i++) {
    rgb_out[i].setup(RGB_PINS[i], RGB_CHANNEL[i] + i, &_timer_config_led, 0);
    }
   

};

void COLORSENSOR:: setColor (STATE_COLOR rgb_color) {
    if (rgb_color == R) {
        s2.set(0);
        s3.set(0);
    }
    if (rgb_color == G) {
        s2.set(1);
        s3.set(1);
    }
    if (rgb_color == B) {
        s2.set(0);
        s3.set(1);
    }

};

void COLORSENSOR:: readColor(uint8_t rgb[3]){
    uint32_t pulses_r, pulses_g, pulses_b;

    setColor(R);
    vTaskDelay(pdMS_TO_TICKS(5));
    pulses = 0;
    vTaskDelay(pdMS_TO_TICKS(20));
    pulses_r = pulses;

    setColor(G);
    vTaskDelay(pdMS_TO_TICKS(5));
    pulses = 0;
    vTaskDelay(pdMS_TO_TICKS(20));
    pulses_g = pulses;

    setColor(B);
    vTaskDelay(pdMS_TO_TICKS(5));
    pulses = 0;
    vTaskDelay(pdMS_TO_TICKS(20));
    pulses_b = pulses;

    printf("PULSOS CRUDOS -> R: %lu, G: %lu, B: %lu\n", pulses_r, pulses_g, pulses_b);

    int r = map_value(pulses_r, R_MIN, R_MAX, 0, 255);
    int g = map_value(pulses_g, G_MIN, G_MAX, 0, 255);
    int b = map_value(pulses_b, B_MIN, B_MAX, 0, 255);


    if (r > 1023) { r = 255; } else if (r < 0) { r = 0; }
    if (g > 1023) { g = 255; } else if (g < 0) { g = 0; }
    if (b > 1023) { b = 255; } else if (b < 0) { b = 0; }


    rgb[R] = {(uint8_t) r};
    rgb[G] = {(uint8_t) g};
    rgb[B] = {(uint8_t) b};
}

void COLORSENSOR::display(uint32_t rgb[3]) {
    for (int i = 0; i < 3; i++) {
        rgb_out[i].setDigitalLevel(rgb[i]);
        rgb_out[i].setFrequency(5000);
    }
    
}

void COLORSENSOR::blink(uint32_t rgb[3]) {
    for (int i = 0; i< 3; i++) {
        rgb_out[i].setDigitalLevel(rgb[i]/2);
        rgb_out[i].setFrequency(2);
    }
}
