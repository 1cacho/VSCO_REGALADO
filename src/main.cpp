#include "definitions.h"

extern "C" void app_main()
{
    esp_task_wdt_deinit();
    stepper.setup(STEPPER_DIR_PIN, STEPPER_PUL_PIN, STEPPER_CHANNEL);
    termometer.setup(TERMOMETER_PIN);
    //uart.setup(115200, UART_NUM_0, TX_PIN, RX_PIN, 256);
    push.setup(PUSH_PIN, GPIO_MODE_INPUT, GPIO_PULLDOWN_ONLY);
    //color.setup(COLOR_PIN, S2_PIN, S3_PIN, RGB_PINS, RGB_CHANNELS);
    fan.setup(FAN_PIN);
    contrast.setup(POT_INC_PIN, POT_UD_PIN);
    contrast.set(contrast_base);
    lcd.setup(LCD_SDA_PIN, LCD_SCL_PIN);
    timer.setup(onSegundoCallback, "Cronometro1Min");
    push.addInterrupt(GPIO_INTR_POSEDGE, onPushCallback, 200000);
    calpush.setup(CAL_PIN, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);
    calpush.addInterrupt(GPIO_INTR_NEGEDGE, onCalCallback, 200000);
    rgb.setUp(RGB_PINS, RGB_CHANNELS,  &_timer_config_rgb, 0);

    vsco.setup(50, EncoderA, EncoderB, Enable, LPWM, RPWM, controller::PI, discretizationtype::Tustin, Kp, Ki, 0.0f, 0.0f, RefRPM);
    while (true)
    {
        current_time_ms = esp_timer_get_time() / 1000;



        
        // 1. Solo pide la temperatura si NO estamos esperando una conversión
        if (!waiting_temp)
        {
            if (termometer.askTemperature())
            {
                termometer_time_ms = current_time_ms;
                waiting_temp = true; // Bloquea nuevas peticiones mientras convierte
            }
        }
        // 2. Cuando ya pasaron los 800 ms desde la petición, lee el resultado
        else if (800 < (current_time_ms - termometer_time_ms))
        {
            uint64_t temp_escalada = termometer.readTemperature();
            if (temp_escalada != 0xFFFFFFFFFFFFFFFF)
            {
                temp_real = (float)temp_escalada / 10000.0f;
            }
            waiting_temp = false; // Libera el candado para pedir la siguiente lectura

            // Contraste de la LCD segun la temperatura
            contrast.set(contrast_base - 2 * (temp_real - TEMP_REFERENCE));
            sprintf(lcd_text, "Temp: %.1f C", temp_real);
            lcd.printLine(0, lcd_text);
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
                if (!airok) {
                //print instruction to press yellow button, no air reference yet
                lcd.printLine(1, "Yellow btn to");
                lcd.printLine(2, "calibrate");
                }
                else if (!soapok) {
                }
                else {
                //print instruction to press green button to start measurement
                lcd.printLine(1, "Green btn to ");
                lcd.printLine(2, "start measurement");
                }
            }

            if (push_clicked)
            {
                current_state = PLACEMENT;
                new_state = true;
            }
            else if(cal_clicked){
                calibrando = true;
                current_state = AIR;
                new_state = true;
            }
            break;

        case PLACEMENT:
            if (new_state) {
                new_state = false;
                if (!airok or !soapok) {
                    lcd.printLine(1, "Place max sample");
                    lcd.printLine(2, "and press green");
                }
                else {
                    lcd.printLine(1, "Place any sample");
                    lcd.printLine(2, "and press green");
                }
                
            }
            rgb.blink(255,255,0, 500000);
            if (push_clicked) {
                new_state = true;
                // La referencia de aire solo se mide al calibrar o si todavia no existe
                current_state = DOWN;
            }
        break;

        case AIR:
        if (new_state) {
            new_state = false;
            //reference measure air WAIT
            lcd.printLine(1, "Air reference, wait");
            lcd.printLine(2, "");
            stepper_time_ms = stepper.move(115, STEPPER_PPR, down);
            prev_time_ms = current_time_ms;
        }
        if (stepper_time_ms < (current_time_ms - prev_time_ms)) {
            vsco.update();
            if ((10000 + stepper_time_ms) < (current_time_ms - prev_time_ms)){
                min = vsco.vscoMin();
                airok= true;
                vsco.stop();
                new_state = true;
                current_state = UP_AIR;
            }
        }
        break;

        case UP_AIR:
        if (new_state) {
            new_state = false;
            //ascending, wait
            lcd.printLine(1, "Ascending, wait");
            stepper_time_ms = stepper.move(115, STEPPER_PPR, up);
            prev_time_ms = current_time_ms;
        }
        if (stepper_time_ms < (current_time_ms - prev_time_ms)) {
            current_state = PLACEMENT;
            new_state = true;
        }
        break;

        case DOWN:
        if (new_state) {
            new_state = false;
            //printf(txBuffer, "Lowering \n %s", txMeasurements);
            lcd.printLine(1, "Lowering probe");
            //color.blink(rgb_out);
            stepper_time_ms = stepper.move(115, STEPPER_PPR, down);
            prev_time_ms = current_time_ms;
            }
        if (stepper_time_ms < (current_time_ms - prev_time_ms)) {
            rgb.setColor(0, 255, 0);
            //make sure to center the sample
            lcd.printLine(1, "Center sample");
            lcd.printLine(2, "Then press green");
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
            lcd.printLine(1, "Mixing sample");
            lcd.printLine(2, "");
            prev_time_ms = current_time_ms;
        }
        rgb.blink(255,0,0,500000);
        vsco.revolvervsco();
        if (10000 < (current_time_ms - prev_time_ms)) {
            vsco.stop();
            current_state = MEASURE;
            new_state = true;
        }
        break;

        case MEASURE:
        if (new_state) {
            if (!airok or !soapok) {
                lcd.printLine(1, "Measuring max ref");
            }
            else {
                //print instruction to press green button to start measurement
                lcd.printLine(1, "Measuring viscosity");
            }
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
            lcd.printLine(1, "Measurement ready");
            sprintf(lcd_text, "Visc: %.2f cP", meas);
            lcd.printLine(3, lcd_text);
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
            stepper_time_ms = stepper.move (115, STEPPER_PPR, up);
            prev_time_ms = current_time_ms;
            //remove sample, elevating viscosimeter print
            lcd.printLine(1, "Raising probe");
        }
        if ((stepper_time_ms) < (current_time_ms - prev_time_ms)) {
            //print take sample, go to wash station
            lcd.printLine(1, "Take sample -> wash");
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
            lcd.printLine(1, "Press green");
            lcd.printLine(2, "when wash ready");
        }
        if (push_clicked&& !clean_en) {
            stepper_time_ms = stepper.move(115, STEPPER_PPR, down);
            prev_time_ms = current_time_ms;
            clean_en = true;
            // Lowering...
            lcd.printLine(1, "Lowering probe");
            lcd.printLine(2, "");
        }

        if (clean_en && (current_time_ms - prev_time_ms) > stepper_time_ms) {
            vsco.lavarvsco();

            if (!timer_started) {
                timer.startPeriodic(1000000);
                timer_started = true;
                sec = 60;
                //print washing 
                lcd.printLine(1, "Washing");
            }

            if (timer.interruptAvailable()) {
                sec--;
                //print washing and sec variable
                sprintf(lcd_text, "Washing %d s", sec);
                lcd.printLine(1, lcd_text);
            }

            if (0 == sec) {
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
            lcd.printLine(1, "Raising probe");
            stepper_time_ms = stepper.move(115, STEPPER_PPR, up);
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
            sec = 20;
            //print drying
            lcd.printLine(1, "Drying");
        }
        if (timer.interruptAvailable()) {
            sec --;
            //print timer
            sprintf(lcd_text, "Drying %d s", sec);
            lcd.printLine(1, lcd_text);
        }
        if (0 == sec) {
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