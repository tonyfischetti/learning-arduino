

/********************************************************
 * POTENTIAL IMPROVEMENTS                               *
 *                                                      *
 *                                                      *
 *   - Add dimming potentiometer                        *
 *   - Don't analog read _every_ step                   *
 *   - Power more than one RGB LED                      *
 *   - Use a power supply and a resistor                *
 *   - Use smaller data types (maybe bytes)             *
 *   - Avoid function call to debug if no debug?        *
 *   - Switch to turn off some colors?                  *
 *   - Parameterize step_delta?                         *
 *   - Refactoring (maybe DRY with bring down/up        *
 *     for each color                                   *
 *                                                      *
 ********************************************************/


#define DEBUG false

#define RED_LED   11
#define GREEN_LED 10
#define BLUE_LED  9

#define STEP_DELTA 5

#define EPSILON 250

#define PRIMARY_SENSOR_IN    A0
#define PRIMARY_SENSOR_OUT   2
#define SECONDARY_SENSOR_IN  A1
#define SECONDARY_SENSOR_OUT 4



byte step_delay = 15;
byte strobe_delay = 0;

byte current_red   = 255;
byte current_green = 255;
byte current_blue  = 255;

byte counter = 0;
byte check_sensors_about_every = 10;


void debug_values() {
    if(DEBUG) {
        char buff[20];
        sprintf(buff, "(%d, %d, %d)",
                current_red, current_green, current_blue);
        Serial.print(buff);
    }
}

void RGB_color(int rval, int gval, int bval) {
    analogWrite(RED_LED, rval);
    analogWrite(GREEN_LED, gval);
    analogWrite(BLUE_LED, bval);
    if (counter % check_sensors_about_every == 0) {
        update_step_delay();
        update_strobe_delay();
    }
}

void update_step_delay() {
    int sensorValue = analogRead(PRIMARY_SENSOR_IN);
    step_delay = map(sensorValue, 0, 1023, 1, 200);
}

void update_strobe_delay() {
    int sensorValue = analogRead(SECONDARY_SENSOR_IN);
    strobe_delay = map(sensorValue, 0, 1023, 0, 255);
}

void do_strobe_delay() {
    if (strobe_delay > 27) {
        RGB_color(0, 0, 0);
        delay(strobe_delay);
    }
}


/* ----------------------------------- */

void bring_down_red() {
    while (current_red >= STEP_DELTA) {
        current_red = current_red - STEP_DELTA;
        RGB_color(current_red, current_green, current_blue);
        delay(step_delay);
        do_strobe_delay();
    }
}

void bring_up_red() {
    while (current_red <= (255 - STEP_DELTA)) {
        current_red = current_red + STEP_DELTA;
        RGB_color(current_red, current_green, current_blue);
        delay(step_delay);
        do_strobe_delay();
    }
}

void bring_down_green() {
    while (current_green >= STEP_DELTA) {
        current_green = current_green - STEP_DELTA;
        RGB_color(current_red, current_green, current_blue);
        delay(step_delay);
        do_strobe_delay();
    }
}

void bring_up_green() {
    while (current_green <= (255 - STEP_DELTA)) {
        current_green = current_green + STEP_DELTA;
        RGB_color(current_red, current_green, current_blue);
        delay(step_delay);
        do_strobe_delay();
    }
}

void bring_down_blue() {
    while (current_blue >= STEP_DELTA) {
        current_blue = current_blue - STEP_DELTA;
        RGB_color(current_red, current_green, current_blue);
        delay(step_delay);
        do_strobe_delay();
    }
}

void bring_up_blue() {
    while (current_blue <= (255 - STEP_DELTA)) {
        current_blue = current_blue + STEP_DELTA;
        RGB_color(current_red, current_green, current_blue);
        delay(step_delay);
        do_strobe_delay();
    }
}

/* ----------------------------------- */

void setup() {
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(SECONDARY_SENSOR_OUT, OUTPUT);
    pinMode(PRIMARY_SENSOR_OUT, OUTPUT);
    RGB_color(current_red, current_green, current_blue);
    digitalWrite(PRIMARY_SENSOR_OUT, HIGH);
    digitalWrite(SECONDARY_SENSOR_OUT, HIGH);
    #if DEBUG
    Serial.begin(9600);
    #endif
}

void loop() {

    // first pattern
    bring_down_red();
    bring_down_green();
    bring_up_red();
    bring_down_blue();
    bring_up_green();
    bring_up_blue();

    // second pattern
    bring_down_green();
    bring_down_blue();
    bring_up_green();
    bring_down_red();
    bring_up_blue();
    bring_up_red();

    // third pattern
    bring_down_blue();
    bring_down_red();
    bring_up_blue();
    bring_down_green();
    bring_up_red();
    bring_up_green();
}


