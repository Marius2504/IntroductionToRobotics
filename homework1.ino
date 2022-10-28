const int potPinRed = A0;
const int potPinGreen = A1;
const int potPinBlue = A2;

const int ledPinRed = 9;
const int ledPinGreen = 10;
const int ledPinBlue = 11;

int potRedValue = 0;
int potGreenValue = 0;
int potBlueValue = 0;

const int maxAnalogValuePot = 1023;  //maximum value for pots
const int maxAnalogValueLed = 255;   //maximum value for leds
const int minAnalogValuePot = 0;    //minimum value for pots
const int minAnalogValueLed = 0;    //minimum value for leds

void setup() {

  pinMode(potPinRed, INPUT);
  pinMode(potPinGreen, INPUT);
  pinMode(potPinBlue, INPUT);

  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  potRedValue = analogRead(potPinRed);
  potGreenValue = analogRead(potPinGreen);
  potBlueValue = analogRead(potPinBlue);

  analogWrite(ledPinRed, map(potRedValue, minAnalogValuePot, maxAnalogValuePot, minAnalogValueLed, maxAnalogValueLed));
  analogWrite(ledPinGreen, map(potGreenValue, minAnalogValuePot, maxAnalogValuePot, minAnalogValueLed, maxAnalogValueLed));
  analogWrite(ledPinBlue, map(potBlueValue, minAnalogValuePot, maxAnalogValuePot, minAnalogValueLed, maxAnalogValueLed));
}
