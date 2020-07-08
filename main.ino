#include <RGBLed.h>

RGBLed led(10, 11,  9, COMMON_CATHODE);

 // 11 verde
 // 10 rosu
 // 9 albastru

const byte ledPin = 13;
const byte potPin = 0;
const byte interruptPin = 2;
const byte muxInPin = 2;

volatile int brightness = 0;
volatile int mode = 0;

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
                      B11110000 }; 

// holds incoming values from 74HC4067                  
byte muxValues[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};

void setup() {
  Serial.begin(9600);
  
  DDRD = B11111111;
  pinMode(ledPin, OUTPUT);
  pinMode(muxInPin, INPUT);
  
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), modeSelect, LOW);
}


void setPin(int outputPin)
{
  PORTD = controlPins[outputPin];
  pinMode(interruptPin, INPUT_PULLUP);
}


void printStatus() {
  Serial.println();
  Serial.print("Mode is: ");
  Serial.println(mode);
  Serial.print("Brightness is: ");
  Serial.println(brightness);
}

void loop() {
  switch(mode) {
    case 1: 
    led.off();
      Serial.println("mode 1 is selected");
     solidColorFlashFast();
      break;
    case 2:
        led.off();
      Serial.println("mode 2 is selected");
      solidColorFlashSlow();
      break;
    case 3:
        led.off();
      Serial.println("mode 3 is selected");
      breathe(1000);
      break;
    case 4:
        led.off();
     colorWheel(25);
      Serial.println("mode 4 is selected");
      break;
    case 5:
        led.off();
      led.brightness(255,0,0, brightness);
     Serial.println("mode 5 is selected");
      break;
      
  }
}

void modeSelect() {
  brightness = map(analogRead(potPin), 0, 1023, 0, 255);
  Serial.println(brightness);
  led.brightness(255,255,255, map(brightness+2, 0, 257, 0, 100)); 
  
  for (int i = 0; i < 16; i++)
  {
    setPin(i); // choose an input pin on the 74HC4067
    if(digitalRead(3) && brightness > 0 ) {
      mode = i;
    } else {
      // TO DO color select via mode
    }
  }
}

void solidColorFlashSlow() { // mode 2 - solid color flash slow
  led.flash(RGBLed::GREEN, 600, 300); 
}

void solidColorFlashFast() { // mode 3 - solid color flash fast
  led.flash(RGBLed::GREEN, 50, 50); 
}


void breathe(int delay) {
  led.fadeIn(RGBLed::GREEN, 10, delay/2);
  led.fadeOut(RGBLed::GREEN, 10, delay/2);
}

void colorWheel(int dly) { // mode 1 - coloWheel
  for(int hue=0;hue<360;hue++)
  {
    writeHSV(hue,1,1); //We are using Saturation and Value constant at 1
    delay(dly); //each color will be shown for 10 milliseconds
  }
}


void writeHSV(int h, double s, double v) {
  //this is the algorithm to convert from RGB to HSV
  double r=0; 
  double g=0; 
  double b=0;

  double hf=h/60.0;

  int i=(int)floor(h/60.0);
  double f = h/60.0 - i;
  double pv = v * (1 - s);
  double qv = v * (1 - s*f);
  double tv = v * (1 - s * (1 - f));

  switch (i)
  {
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
  int red=constrain((int)255*r,0,brightness);
  int green=constrain((int)255*g,0,brightness);
  int blue=constrain((int)255*b,0,brightness);

  led.setColor(red,green,blue);
}
