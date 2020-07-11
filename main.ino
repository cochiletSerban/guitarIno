#include <RGBLed.h>

RGBLed led(10, 11, 9, COMMON_CATHODE); // 10 RED // 11 GREEN // 9 BLUE

const byte potPin = 0;
const byte interruptPin = 2;
const byte muxInPin = 3;

volatile int selectedRange = 0;
volatile int mode = 1;
volatile int selectedColor = 0;
volatile int fxDelay = 500;
volatile int brightness = 100;

byte controlPins[] = {
    B00000000,
    B10000000,
    B01000000,
    B11000000,
    B00100000,
    B10100000,
    B01100000,
    B11100000,
    B00010000,
    B10010000,
    B01010000,
    B11010000,
    B00110000,
    B10110000,
    B01110000,
    B11110000
};

void setPin(int outputPin) {
    PORTD = controlPins[outputPin];
    pinMode(interruptPin, INPUT_PULLUP);
}

void setup() {
    Serial.begin(9600);

    DDRD = B11111111;
    pinMode(muxInPin, INPUT);

    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), modeSelect, LOW);
}


void loop() {
    int * ledColor = getLedColor(selectedColor);

    switch (mode) {
        case 1:
            Serial.println("mode 1 is selected");
            solidColorFlash(ledColor, fxDelay);
            break;
        case 2:
            Serial.println("mode 2 is selected");
            breathe(ledColor, fxDelay);
            break;
        case 3:
            colorWheel(brightness);
            Serial.println("mode 3 is selected");
            break;
        case 4:
            Serial.println("mode 4 is selected");
            led.brightness(ledColor, brightness);
            break;
    }

    free(ledColor);
}

// be able to modify color wheel spd , fix brit at 100%
// test everyting

int readMux() {
    for (int i = 0; i < 16; i++) {
        setPin(i);
        if (digitalRead(muxInPin)) {
            return i;
        }
    }
}

void modeSelect() {
    int selectedNumber = readMux();
    selectedRange = analogRead(potPin);
    int * color = getLedColor(selectedColor);

    if (selectedRange <= 1) {
        selectedColor = selectedNumber;
        led.setColor(color);
    } else {
        mode = selectedNumber;
        if (mode == 1 || mode == 2 || mode == 3) {
            if (selectedRange >= 100) {
                fxDelay = map(selectedRange, 100, 1023, 40, 1023);
            } else {
                fxDelay = 40;
            }
            led.brightness(color, mapSelectedRangeToBrightness(selectedRange));
        } else if (mode > 3) {
            brightness = mapSelectedRangeToBrightness(selectedRange);
            led.brightness(color, brightness);
        }
    }

    Serial.println(fxDelay);

    free(color);

    led.off();
}


// ////////////////////////////// MODES ///////////////////////////////////// //

void solidColorFlash(int color[3], int delay) {
    led.flash(color, delay, delay);
}

void breathe(int color[3], int delay) {
    led.fadeIn(color, 10, delay / 2);
    led.fadeOut(color, 10, delay / 2);
}

void colorWheel(int dly) {
    for (int hue = 0; hue < 360 && mode == 3; hue++) {
        writeHSV(hue, 1, 1);
        delay(dly);
    }
}


// ////////////////////////////// HELPERS /////////////////////////////////// //

int mapSelectedRangeToBrightness(int selectedRange) {
    if (selectedRange > 100) {
        return map(selectedRange, 100, 1023, 5, 100);
    }
    return 0;
}

void writeHSV(int h, double s, double v) {
    //this is the algorithm to convert from RGB to HSV
    double r = 0;
    double g = 0;
    double b = 0;

    double hf = h / 60.0;

    int i = (int) floor(h / 60.0);
    double f = h / 60.0 - i;
    double pv = v * (1 - s);
    double qv = v * (1 - s * f);
    double tv = v * (1 - s * (1 - f));

    switch (i) {
        case 0: //rojo dominante
            r = v;
            g = tv;
            b = pv;
            break;
        case 1: //verde
            r = qv;
            g = v;
            b = pv;
            break;
        case 2:
            r = pv;
            g = v;
            b = tv;
            break;
        case 3: //azul
            r = pv;
            g = qv;
            b = v;
            break;
        case 4:
            r = tv;
            g = pv;
            b = v;
            break;
        case 5: //rojo
            r = v;
            g = pv;
            b = qv;
            break;
    }

    //set each component to a integer value between 0 and 255
    int red = constrain((int) 255 * r, 0, 255);
    int green = constrain((int) 255 * g, 0, 255);
    int blue = constrain((int) 255 * b, 0, 255);

    led.setColor(red, green, blue);
}

int * getLedColor(int selectedColor) {

    int * color = calloc(3, sizeof(int));

    switch (selectedColor) {
        case 1:
            color[0] = 255;
            color[1] = 0;
            color[2] = 0;
            break;
        case 2:
            color[0] = 0;
            color[1] = 255;
            color[2] = 0;
            break;
        case 3:
            color[0] = 0;
            color[1] = 0;
            color[2] = 255;
            break;
        case 4:
            color[0] = 255;
            color[1] = 0;
            color[2] = 255;
            break;
        case 5:
            color[0] = 255;
            color[1] = 255;
            color[2] = 0;
            break;
        case 7:
            color[0] = 0;
            color[1] = 255;
            color[2] = 255;
            break;
        case 8:
            color[0] = 255;
            color[1] = 165;
            color[2] = 0;
            break;
        case 9:
            color[0] = 255;
            color[1] = 105;
            color[2] = 0;
            break;
        case 10:
            color[0] = 0;
            color[1] = 128;
            color[2] = 128;
            break;
        default:
            color[0] = 255;
            color[1] = 255;
            color[2] = 255;
    }

    return color;
}

void printColor(int * color) {
    char buffer[50];
    sprintf(buffer, "Color is: %d %d %d\n", color[0], color[1], color[2]);
    Serial.print(buffer);
}

// brightness = map(analogRead(potPin), 0, 1023, 0, 255);
// led.setColor(color); // map(brightness + 2, 0, 257, 0, 100)
// map(brightness + 2, 0, 257, 0, 100)
//map(analogRead(potPin), 0, 1023, 0, 255);



// if ((mode == 1 || mode == 2) && selectedRange > 1) {
//     fxDelay = selectedRange;

//     led.brightness(color, map(fxDelay + 2, 0, 1025, 0, 100));
//     Serial.print("fxDelay: "); Serial.println(fxDelay);

// } else if ((mode == 3 || mode == 4) && selectedRange > 1) {
//     brightness = map(selectedRange + 2, 0, 1025, 0, 100);

//     led.brightness(color, brightness);
//     Serial.print("brit: "); Serial.println(brightness);
// }
// switch (selectedColor) {
//         case 1:
//             color[0] = 255;
//             color[1] = 0;
//             color[2] = 0;
//             break;
//         case 2:
//             color[0] = 0;
//             color[1] = 255;
//             color[2] = 0;
//             break;
//         case 3:
//             color[0] = 0;
//             color[1] = 0;
//             color[2] = 255;
//             break;
//         case 4:
//             color[0] = 255;
//             color[1] = 0;
//             color[2] = 255;
//             break;
//         case 5:
//             color[0] = 255;
//             color[1] = 255;
//             color[2] = 0;
//             break;
//         case 7:
//             color[0] = 0;
//             color[1] = 255;
//             color[2] = 255;
//             break;
//         case 8:
//             color[0] = 255;
//             color[1] = 165;
//             color[2] = 0;
//             break;
//         case 9:
//             color[0] = 255;
//             color[1] = 105;
//             color[2] = 0;
//             break;
//         case 10:
//             color[0] = 0;
//             color[1] = 128;
//             color[2] = 128;
//             break;
//         default:
//             color[0] = 255;
//             color[1] = 255;
//             color[2] = 255;
//     }
