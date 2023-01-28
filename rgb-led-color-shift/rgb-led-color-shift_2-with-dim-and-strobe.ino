

/********************************************************
 * POTENTIAL IMPROVEMENTS                               *
 *                                                      *
 *                                                      *
 *   - Add dimming potentiometer                        *
 *   - Power more than one RGB LED                      *
 *   - Use a power supply and a resistor                *
 *   - Switch to turn off some colors?                  *
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

#define RED_INDEX   0
#define GREEN_INDEX 1
#define BLUE_INDEX  2


byte step_delay = 15;
byte strobe_delay = 0;

byte current_rgb[3] = {255, 255, 255};

byte counter = 0;
byte check_sensors_about_every = 10;


#if DEBUG
void debug_values() {
    char buff[20];
    sprintf(buff, "(%d, %d, %d)",
            current_rgb[RED_INDEX],
            current_rgb[GREEN_INDEX],
            current_rgb[BLUE_INDEX]);
    Serial.println(buff);
}
#endif


void write_RGB_colors() {
    analogWrite(RED_LED,   current_rgb[RED_INDEX]);
    analogWrite(GREEN_LED, current_rgb[GREEN_INDEX]);
    analogWrite(BLUE_LED,  current_rgb[BLUE_INDEX]);
    if (counter % check_sensors_about_every == 0) {
        update_step_delay();
        update_strobe_delay();
    }
    #if DEBUG
    debug_values();
    #endif
}

void write_RGB_zeroes() {
    analogWrite(RED_LED,   0);
    analogWrite(GREEN_LED, 0);
    analogWrite(BLUE_LED,  0);
}

void update_step_delay() {
    step_delay = map(analogRead(PRIMARY_SENSOR_IN), 0, 1023, 1, 200);
}

void update_strobe_delay() {
    strobe_delay = map(analogRead(SECONDARY_SENSOR_IN), 0, 1023, 0, 255);
}

void do_strobe_delay() {
    if (strobe_delay > 27) {
        // TODO: fix
        write_RGB_zeroes();
        delay(strobe_delay);
    }
}


/* ----------------------------------- */

void bring_down_color(byte color_index) {
    while (current_rgb[color_index] >= STEP_DELTA) {
        current_rgb[color_index] = current_rgb[color_index] - STEP_DELTA;
        write_RGB_colors();
        delay(step_delay);
        do_strobe_delay();
    }
}

void bring_up_color(byte color_index) {
    while (current_rgb[color_index] <= (255 - STEP_DELTA)) {
        current_rgb[color_index] = current_rgb[color_index] + STEP_DELTA;
        write_RGB_colors();
        delay(step_delay);
        do_strobe_delay();
    }
}

/* ----------------------------------- */

void setup() {
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    digitalWrite(PRIMARY_SENSOR_OUT, HIGH);
    digitalWrite(SECONDARY_SENSOR_OUT, HIGH);
    pinMode(SECONDARY_SENSOR_OUT, OUTPUT);
    pinMode(PRIMARY_SENSOR_OUT, OUTPUT);
    write_RGB_colors();
    #if DEBUG
    Serial.begin(9600);
    #endif
}

void loop() {

    // first pattern
    bring_down_color(RED_INDEX);
    bring_down_color(GREEN_INDEX);
    bring_up_color(RED_INDEX);
    bring_down_color(BLUE_INDEX);
    bring_up_color(GREEN_INDEX);
    bring_up_color(BLUE_INDEX);

    // second pattern
    bring_down_color(GREEN_INDEX);
    bring_down_color(BLUE_INDEX);
    bring_up_color(GREEN_INDEX);
    bring_down_color(RED_INDEX);
    bring_up_color(BLUE_INDEX);
    bring_up_color(RED_INDEX);

    // third pattern
    bring_down_color(BLUE_INDEX);
    bring_down_color(RED_INDEX);
    bring_up_color(BLUE_INDEX);
    bring_down_color(GREEN_INDEX);
    bring_up_color(RED_INDEX);
    bring_up_color(GREEN_INDEX);

}



