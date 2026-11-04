#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <esp_task_wdt.h>
#pragma once
#include <cstring>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Termometer.h"
#include "WaterPump.h"
#include "StepperMotor.h"
#include "ColorSensor.h"
#include "COMM.h"
#include "SimpleUART.h"
#include "DCMotor.h"
#include "Encoder.h"
#include "Control.h"
#include "Viscosimeter.h"
#include "RGB.h"


#define poll_frequency 200

//nema 
STEPPERMOTOR stepper;
#define STEPPER_DIR_PIN 32
#define STEPPER_PUL_PIN 33
#define STEPPER_CHANNEL 1

uint64_t stepper_time_ms = 0;


//termometer 
TERMOMETER termometer;
#define TERMOMETER_PIN 14
#define TERMOMETER_TIME_CONSTANT_MS 800
uint64_t termometer_time_ms = 0;
float temp_real;



//uart
SimpleUART uart;
#define TX_PIN 1
#define RX_PIN 3
char rxBuffer;
char txBuffer [128];
char txMeasurements [64];
uint64_t send_time_ms = 0;
#define SEND_TIME_CONSTANT 50;

//joystick 
SimpleGPIO js_up, js_down, push, calpush;
#define CAL_PIN 5
#define JS_PIN_UP 34
#define JS_PIN_DOWN  35
#define PUSH_PIN 4
bool calibrando = false;
void IRAM_ATTR onCalCallback(void* arg) {
    calpush.setInterruptFlag();
}
void IRAM_ATTR onPushCallback(void* arg) {
    push.setInterruptFlag();
}


//color sensor 
COLORSENSOR color;
#define S2_PIN 16
#define S3_PIN 17
#define COLOR_PIN 36

//rgb
uint8_t RGB_PINS [3] = {22, 23, 25};
uint8_t RGB_CHANNELS[3] = {4, 5, 6};
uint32_t rgb_out[3] = {0,0,0};
RGB rgb;
TimerConfig _timer_config_rgb;

//fan
DCMOTOR fan;
#define FAN_PIN 21

//timer
SimpleTimer timer;
int sec = 0;

// Esta función la llama el ESP32 automáticamente cada 1 segundo
void IRAM_ATTR onSegundoCallback(void* arg) {
    timer.setInterrupt();
}

//Viscosimeter
#define EncoderA GPIO_NUM_19
#define EncoderB GPIO_NUM_18
#define sampletime 20000
#define Enable GPIO_NUM_13
#define RPWM GPIO_NUM_27
#define LPWM GPIO_NUM_26
#define Kp 0.2328f
#define Ki 2.6607f
#define T1m 10000000.0f
#define Tlavado 9000000.0f

float u =0;
float corrinteg = 0;
bool is_state_new = true;
uint64_t start_time = 0;
uint64_t now = 0;
float rpm = 0.0f;
float bufferspeed[10] = {0};
float min = 0;
float max = 0;
float meas = 0;
float RefRPM  = 120.0f;
bool airok = false;
bool soapok = false;


enum STATE {
    IDLE,
    PLACEMENT,
    AIR,
    DOWN, 
    STIR,
    MEASURE,
    DEPLOYMENT,
    UP,
    CLEANING,
    UP_CLEAN,
    DRYING,
    RESTART
    
    /*INACTIVE,
    PUREAIR,
    PURESOAP,
    SOAPMEAS,
    WASH,
    STIRR,
    UP,
    DOWN*/
};


STATE current_state = IDLE;
bool new_state = true;
bool clean_en = false;
bool timer_started = false;
uint64_t current_time_ms = 0;
uint64_t prev_time_ms = 0;





viscosimeter vsco;

