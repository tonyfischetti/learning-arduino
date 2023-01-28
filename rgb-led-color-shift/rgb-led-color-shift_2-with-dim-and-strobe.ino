

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

// pins that contol each color of the RGB LED(s)
#define RED_LED   11
#define GREEN_LED 10
#define BLUE_LED  9

// value to change the color by on each step
#define STEP_DELTA 5

// number of milliseconds to wait for buttons, etc. to settle
#define EPSILON 250

#define PRIMARY_SENSOR_IN    A0
#define PRIMARY_SENSOR_OUT   2
#define SECONDARY_SENSOR_IN  A1
#define SECONDARY_SENSOR_OUT 4
// TODO: tertiary sensors

// The indices into the `current_rgb` array for each color
#define RED_INDEX   0
#define GREEN_INDEX 1
#define BLUE_INDEX  2


// holds the values for each RGB channel (globally)
byte current_rgb[3] = {255, 255, 255};

// holds the index (into pattern_functions) for the current
// pattern's function pointer
byte current_pattern_fun_index = 0;

// counter of steps. will overflow. that's ok
byte counter = 0;

byte check_sensors_about_every = 10;




void nothing_function () {
    // correct?
    return NULL;
}

typedef void (*SensorUpdateFunction) ();
SensorUpdateFunction update_primary_sensor  = nothing_function;
SensorUpdateFunction update_secondary_sensor = nothing_function;

typedef void (*PatternFunction) ();
PatternFunction current_pattern_function = nothing_function;



#if DEBUG
// TODO: more debug info (pattern, etc...)
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
        update_primary_sensor();
        update_secondary_sensor();
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



/* --------------------------------------------------------- */

/* PATTERN 0:
 *
 *   Shifts through all the colors. Speed, strobe, and brightness are
 *   parameterized.
 *
 */

byte step_delay_0 = 15;
byte strobe_delay_0 = 0;

void update_step_delay() {
    step_delay_0 = map(analogRead(PRIMARY_SENSOR_IN), 0, 1023, 1, 200);
}

void update_strobe_delay() {
    strobe_delay_0 = map(analogRead(SECONDARY_SENSOR_IN), 0, 1023, 0, 255);
}

void do_strobe_delay() {
    if (strobe_delay_0 > 27) {
        // TODO: fix
        write_RGB_zeroes();
        delay(strobe_delay_0);
    }
}

void bring_down_color(byte color_index) {
    while (current_rgb[color_index] >= STEP_DELTA) {
        current_rgb[color_index] = current_rgb[color_index] - STEP_DELTA;
        write_RGB_colors();
        delay(step_delay_0);
        do_strobe_delay();
    }
}

void bring_up_color(byte color_index) {
    while (current_rgb[color_index] <= (255 - STEP_DELTA)) {
        current_rgb[color_index] = current_rgb[color_index] + STEP_DELTA;
        write_RGB_colors();
        delay(step_delay_0);
        do_strobe_delay();
    }
}

void all_color_change_pattern() {

    update_primary_sensor = update_step_delay;
    update_secondary_sensor = update_strobe_delay;

    // first sub-pattern
    bring_down_color(RED_INDEX);
    bring_down_color(GREEN_INDEX);
    bring_up_color(RED_INDEX);
    bring_down_color(BLUE_INDEX);
    bring_up_color(GREEN_INDEX);
    bring_up_color(BLUE_INDEX);

    // second sub-pattern
    bring_down_color(GREEN_INDEX);
    bring_down_color(BLUE_INDEX);
    bring_up_color(GREEN_INDEX);
    bring_down_color(RED_INDEX);
    bring_up_color(BLUE_INDEX);
    bring_up_color(RED_INDEX);

    // third sub-pattern
    bring_down_color(BLUE_INDEX);
    bring_down_color(RED_INDEX);
    bring_up_color(BLUE_INDEX);
    bring_down_color(GREEN_INDEX);
    bring_up_color(RED_INDEX);
    bring_up_color(GREEN_INDEX);

    // TODO: goto end??
}

/* --------------------------------------------------------- */

/* PATTERN 1:
 *
 *   Solid color with option to change the values for each RGB channel
 *
 */

void update_red_brightness() {
    current_rgb[RED_INDEX] = map(analogRead(PRIMARY_SENSOR_IN),
                                            0, 1023, 0, 255);
}

void update_green_brightness() {
    current_rgb[GREEN_INDEX] = map(analogRead(SECONDARY_SENSOR_IN),
                                              0, 1023, 0, 255);
}


void solid_color_pattern() {

    update_primary_sensor = update_red_brightness;
    update_secondary_sensor = update_green_brightness;

    current_rgb[0] = 255;
    current_rgb[1] = 255;
    current_rgb[2] = 255;
    write_RGB_colors();

    delay(100);
}


/* --------------------------------------------------------- */

PatternFunction pattern_functions[2] = {
    all_color_change_pattern,
    solid_color_pattern
};

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

    pattern_functions[current_pattern_fun_index]();

}



