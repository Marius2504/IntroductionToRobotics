const int buttonPin =2;
const int ledPinRedCar = 3;
const int ledPinYellowCar =5;
const int ledPinGreenCar = 6;
const int ledPinGreenHuman = 9;
const int ledPinRedHuman = 10;
const int buzzerPin = 11;

int buzzerTone = 2500;
byte buttonState = 0;
byte lastButtonState = 0;
byte ledPinRedCarValue = 0;
byte ledPinYellowCarValue = 0;
byte ledPinGreenCarValue = 1;
byte ledPinGreenHumanValue = 0;
byte ledPinRedHumanValue = 1;
unsigned int debouncyDelay =50;
unsigned int lastDebounceTime = 0;
unsigned int waitGreen = 8000;
unsigned int waitForYellow = 3000;
unsigned int waitGreenFlickering = 4000;
unsigned int state = 1; // the state of the machine
byte buttonPressed = 0; //if the button was pressed
unsigned long lastDebounceTimeForButton = 0; // last time when the button was pressed
unsigned long lastDebounceTimeForBuzzer = 0; // last time when the buzzer played
unsigned int buzzerRate = 200; // gap time between buzz sound
unsigned int buzzerSoundTime = 50; // the amount of time when the buzzer is playing
void setup() {
  pinMode(buttonPin,INPUT_PULLUP);
  pinMode(ledPinRedCar,OUTPUT);
  pinMode(ledPinYellowCar,OUTPUT);
  pinMode(ledPinGreenCar,OUTPUT);
  pinMode(ledPinGreenHuman,OUTPUT);
  pinMode(ledPinRedHuman,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
  digitalWrite(ledPinGreenCar,ledPinGreenCarValue); // set the value of light for cars
  digitalWrite(ledPinRedHuman,ledPinRedHumanValue); // set the value of the light for humnas
  
  Serial.begin(9600);
}

void loop() {
  buttonState = !digitalRead(buttonPin);
  if(buttonPressed == 0 && buttonState != lastButtonState && millis()-lastDebounceTime > debouncyDelay)
  {
    lastButtonState = buttonState;
    lastDebounceTime = millis();
    lastDebounceTimeForButton = lastDebounceTime;
    buttonPressed = 1;
  }
  if(buttonPressed == 1 && state ==1 && millis()-lastDebounceTimeForButton >waitGreen)
  {
    state = 2;
    lastDebounceTimeForButton = millis();
    state1();
  }
  if(state == 2 && millis()-lastDebounceTimeForButton >waitForYellow)
  {
    state =3;
    lastDebounceTimeForButton = millis();
    state2();
  }
  if(state == 3 && millis()-lastDebounceTimeForButton >waitGreen)
  {
    state =4;
    lastDebounceTimeForButton = millis();
    state3();
  }

  if(state == 4 && millis()-lastDebounceTimeForButton >waitGreenFlickering)
  {
    state =0;
    lastDebounceTimeForButton = millis();
    state4();
  }
  if(buttonPressed == 1 && (state == 3 || state == 4) && millis()-lastDebounceTimeForBuzzer > buzzerRate)
  {
    lastDebounceTimeForBuzzer = millis();
    tone(buzzerPin,buzzerTone,buzzerSoundTime);
    if(state == 4)
    {
      if(ledPinGreenHumanValue == 0)
        ledPinGreenHumanValue = 1;
      else
        ledPinGreenHumanValue = 0;
    }
  }
  if(state == 0)
    {
      lastButtonState = 0;
      buttonPressed = 0;
      state =1;
    }
  //tone(buzzerPin,buzzerTone,500);
  digitalWrite(ledPinRedCar,ledPinRedCarValue);
  digitalWrite(ledPinYellowCar,ledPinYellowCarValue);
  digitalWrite(ledPinGreenCar,ledPinGreenCarValue);
  digitalWrite(ledPinGreenHuman,ledPinGreenHumanValue);
  digitalWrite(ledPinRedHuman,ledPinRedHumanValue);
  
}

void state1()
{
  ledPinRedCarValue = 0;
  ledPinYellowCarValue = 1;
  ledPinGreenCarValue = 0;
  ledPinGreenHumanValue = 0;
  ledPinRedHumanValue = 1;
}
void state2()
{
  ledPinRedCarValue = 1;
  ledPinYellowCarValue = 0;
  ledPinGreenCarValue = 0;
  ledPinGreenHumanValue = 1;
  ledPinRedHumanValue = 0;
  buzzerRate =200;
}
void state3()
{
  ledPinRedCarValue = 1;
  ledPinYellowCarValue = 0;
  ledPinGreenCarValue = 0;
  ledPinGreenHumanValue = 1;
  ledPinRedHumanValue = 0; 
  buzzerRate =100; 
}
void state4()
{
  ledPinRedCarValue = 0;
  ledPinYellowCarValue = 0;
  ledPinGreenCarValue = 1;
  ledPinGreenHumanValue = 0;
  ledPinRedHumanValue = 1;
  
}
