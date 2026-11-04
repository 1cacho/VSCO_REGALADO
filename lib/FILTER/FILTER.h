#ifndef __FILTER_H__
#define __FILTER_H__

class FILTER {
public:
    FILTER();
    ~FILTER();

    void setup(float* b_coeffs, int b_size, float* a_coeffs, int a_size);
    float apply(float current_input);
    int getDACValue(float volts);

private:
    float* b;
    float* a;
    int size_b;
    int size_a;

    float* x_hist;
    float* y_hist;
};

#endif // __FILTER_H__