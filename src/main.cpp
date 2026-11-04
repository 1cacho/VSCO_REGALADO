#include "definitions.h"

extern "C" void app_main()
{

    stepper.setup(STEPPER_DIR_PIN, STEPPER_PUL_PIN, STEPPER_CHANNEL);
    termometer.setup(TERMOMETER_PIN);
    //uart.setup(115200, UART_NUM_0, TX_PIN, RX_PIN, 256);
    push.setup(PUSH_PIN, GPIO_MODE_INPUT, GPIO_PULLDOWN_ONLY);
    //color.setup(COLOR_PIN, S2_PIN, S3_PIN, RGB_PINS, RGB_CHANNELS);
    fan.setup(FAN_PIN);
    timer.setup(onSegundoCallback, "Cronometro1Min");
    push.addInterrupt(GPIO_INTR_POSEDGE, onPushCallback, 200000);
    calpush.setup(CAL_PIN, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);
    calpush.addInterrupt(GPIO_INTR_NEGEDGE, onCalCallback, 200000);
    rgb.setUp(RGB_PINS, RGB_CHANNELS,  &_timer_config_rgb, 0);

    vsco.setup(50, EncoderA, EncoderB, Enable, LPWM, RPWM, controller::PI, discretizationtype::Tustin, Kp, Ki, 0.0f, 0.0f, RefRPM);
    TickType_t last = xTaskGetTickCount();
    while (true)
    {
        current_time_ms = esp_timer_get_time() / 1000;



        
        if (termometer.askTemperature())
        {
            termometer_time_ms = current_time_ms;
        }
        if (80 < (current_time_ms - termometer_time_ms))
        {
            uint64_t temp_escalada = termometer.readTemperature();
            if (temp_escalada != 0xFFFFFFFFFFFFFFFF)
            {
                temp_real = (float)temp_escalada / 10000.0f;
            }
        }
        if (poll_frequency < (current_time_ms - send_time_ms))
        {
        bool push_clicked = push.interruptAvailable();
        bool cal_clicked  = calpush.interruptAvailable();
        send_time_ms = current_time_ms;
        rpm = vsco.getRPM();
        corrinteg = vsco.getIntegral();

        printf("%s, Temp: %.1f, RPM: %.1f, Visc: %.2f, Int: %.2f\n",
        STATE_NAMES[current_state], temp_real, rpm, meas, corrinteg);
        
        switch (current_state)
        {

        case IDLE:
            if (new_state)
            {
                new_state = false;
                rgb.setColor(255, 255, 0);
                vsco.stop();

                sprintf(txBuffer, "Awaiting for sample \n %s", txMeasurements);
            }
            if (push_clicked)
            {
                current_state = PLACEMENT;
                new_state = true;
            }
            else if(cal_clicked){
                calibrando = true;
                current_state = PLACEMENT;
                new_state = true;
            }
            break;

        case PLACEMENT:
            if (new_state) {
                new_state = false;
                sprintf(txBuffer, "Ensure sample placement \n %s", txMeasurements);
                rgb.blink(255,255,0, 500000);
            }
            if (push_clicked) {
                new_state = true;
                // La referencia de aire solo se mide al calibrar o si todavia no existe
                if (calibrando || !airok) {
                    current_state = AIR;
                } else {
                    current_state = DOWN;
                }
            }
        break;

        case AIR:
            if (new_state) {
                new_state = false;
                //reference measure air 
                prev_time_ms = current_time_ms;
            }   
            vsco.update();
            if (10000 < (current_time_ms - prev_time_ms)) {
                min = vsco.vscoMin();
                airok= true;
                new_state = true;
                current_state = DOWN;
                vsco.stop();
            }
        break;
        case DOWN:
        if (new_state) {
            new_state = false;
            //printf(txBuffer, "Lowering \n %s", txMeasurements);
            //color.blink(rgb_out);
            stepper_time_ms = stepper.move(150, STEPPER_PPR, down);
            prev_time_ms = current_time_ms;
            }
        if (stepper_time_ms < (current_time_ms - prev_time_ms)) {
            rgb.setColor(0, 255, 0);
            if (push_clicked) {
                new_state = true;
                current_state = STIR;
            }
        }
        break;
        

        case STIR:
        if (new_state) {
            new_state = false;
            //print Mixing sample
            rgb.blink(255,0,0,500000);
            prev_time_ms = current_time_ms;
        }
        vsco.revolvervsco();
        if (30000 < (current_time_ms - prev_time_ms)) {
            vsco.stop();
            current_state = MEASURE;
            new_state = true;
        }
        break;

        case MEASURE:
        if (new_state) {
            //obtaining viscosity
            new_state = false;   
            prev_time_ms = current_time_ms;
        }
        vsco.update();
        if (10000 < (current_time_ms - prev_time_ms)) {
            if(calibrando){

                max = vsco.vscoMax();
                soapok = true;
            } else {
                meas = vsco.vscoMeas(soapok, airok);
            }
            current_state = DEPLOYMENT;
            new_state = true;
            vsco.stop();
        }
        break;

        case DEPLOYMENT:
        if (new_state) {
            new_state = false;
            rgb.setColor(0,255,0);
            //measurement ready print 
            prev_time_ms = current_time_ms;
        }
        if (7000 < (current_time_ms - prev_time_ms)) {
            current_state = UP;
            new_state = true;
        }
        break;

        case UP:
        if (new_state) {
            new_state = false;
            stepper_time_ms = stepper.move (150, STEPPER_PPR, up);
            prev_time_ms = current_time_ms;
            //remove sample, elevating viscosimeter print
        }
        if ((stepper_time_ms) < (current_time_ms - prev_time_ms)) {
            //print take sample, go to wash station
            if ((stepper_time_ms + 5000) < (current_time_ms - prev_time_ms)) {
                current_state = CLEANING;
                new_state = true;
                rgb.setColor(0,0,0);
            }
        }
        break;

        case CLEANING:
        if (new_state) {
            new_state = false;
        //print press when wash station ready
        }
        if (push_clicked&& !clean_en) {
            stepper_time_ms = stepper.move(160, STEPPER_PPR, down);
            prev_time_ms = current_time_ms;
            clean_en = true;
            // Lowering...
        }

        if (clean_en && (current_time_ms - prev_time_ms) > stepper_time_ms) {
            vsco.lavarvsco();

            if (!timer_started) {
                timer.startPeriodic(1000000);
                timer_started = true;
                //print washing 
            }

            if (timer.interruptAvailable()) {
                sec++;
                //print washing and sec variable
            }

            if (60 <= sec) {
                vsco.stop();
                timer.stopPeriodic();      
                current_state = UP_CLEAN;  
                new_state = true;        
                clean_en = false;
                timer_started = false;
                sec = 0;
            }
        }
        break;

        case UP_CLEAN:
        if (new_state) {
            new_state = false;
            //ascending print
            stepper_time_ms = stepper.move(160, STEPPER_PPR, up);
            prev_time_ms = current_time_ms;
        }
        if (stepper_time_ms < (current_time_ms - prev_time_ms)) {
            current_state = DRYING;
            new_state = true;
        }
        break;
       
        case DRYING:
        if (new_state) {
            new_state = false;
            fan.start();
            timer.startPeriodic(1000000);
            sec = 0;
            //print drying
        }
        if (timer.interruptAvailable()) {
            sec ++;
            //print timer
        }
        if (120 <= sec) {
            fan.stop();
            current_state = RESTART;
            new_state = true;
            sec = 0;
            timer.stopPeriodic();
        }
        break;

        case RESTART:
        if (new_state) {
            // min y airok se conservan para no repetir la referencia de aire
            calibrando = false;
            meas = 0;
            rpm = 0.0f;
            new_state = true;
            current_state = IDLE;
            prev_time_ms  = 0;
        }
        break;

        }

        }

    }
}