#include "FILTER.h"

FILTER::FILTER() {
}

FILTER::~FILTER() {
}


enum FILTER_TYPE {
    BTW,
    IIR,
    MA
};

void FILTER::setup(float* b_coeffs, int b_size, float* a_coeffs, int a_size) {
    b = b_coeffs;
    a = a_coeffs;
    size_b = b_size;
    size_a = a_size;

    x_hist = new float[size_b]{0.0}; 
    y_hist = new float[size_a]{0.0}; 
}

float FILTER::apply(float raw_waveform) {  
    
    FILTER_TYPE current_state;
    if (size_a <= 1) {
        current_state = MA;  // 0 or 1 means no poles (FIR)
    } 
    else {
        current_state = IIR; // >1 there are poles 
    }
    // input history xn = xn-1
    for (int i = size_b - 1; i > 0; i--) {
        x_hist[i] = x_hist[i - 1];
    }
    x_hist[0] = raw_waveform;

    float y_out = 0.0;

    // 2. Evaluate state
    switch (current_state) {
        case MA: 
            // zeros only
            for (int i = 0; i < size_b; i++) {
                y_out += b[i] * x_hist[i];
            }
            break;

        case BTW: 
        case IIR: 
            // FIR zeros
            for (int i = 0; i < size_b; i++) {
                y_out += b[i] * x_hist[i];
            }
            
            // IIR poles
            for (int i = 1; i < size_a; i++) {
                y_out -= a[i] * y_hist[i - 1];
            }

            // output history
            for (int i = size_a - 2; i > 0; i--) {
                y_hist[i] = y_hist[i - 1];
            }
            y_hist[0] = y_out;
            break;
    }

    return y_out;
}

int FILTER::getDACValue(float volts) {
    if (volts < 0.0f) volts = 0.0f;

    float max_voltage = 1.65f; 
    if (volts > max_voltage) volts = max_voltage;

    float mapped_value = (volts / max_voltage) * 255.0f;

    return (int)mapped_value;
}