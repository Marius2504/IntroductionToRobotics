// declare all the joystick pins
const int pinSW = 2; // digital pin connected to switch output
const int pinX = A1; // A0 - analog pin connected to X output
const int pinY = A0; // A1 - analog pin connected to Y output
const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

const int segSize = 8;
const int noOfDigits = 10;

int xValue = 0;
int yValue = 0;
int swValue = 0;
int lastSwValue = 0;
byte buttonState = 0;
byte blinkState = LOW;
byte buttonPressed = 0;

int segments[segSize] = { pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP};
int currentSeg = pinDP;
byte vectorSeg[segSize] = {0,0,0,0,0,0,0,0};

unsigned int blinkDelay =200;
unsigned int debounceDelay =100;
unsigned int debounceController =500;
unsigned int longDebounceDelay =3000;
unsigned long lastDebounceTimeForBlink = 0;
unsigned long lastDebounceTimeForButton = 0;
unsigned long lastDebounceTimeForController = 0;
unsigned long buttonPressedTime = 0; // time while button is being pressed


int currentState = 1;
byte currentStateOnOff=LOW;
bool joyMoved = false;
bool joyMovedState2 = false;
int minThreshold = 400;
int maxThreshold = 600;

void setup() {
  for (int i = 0; i < segSize; i++) 
    pinMode(segments[i], OUTPUT);
  
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinX,INPUT);
  pinMode(pinY,INPUT);
  Serial.begin(9600);
}

void loop() {
  if(currentState == 1)
    state1();
  else
    state2();
}

void state1()
{
  swValue = !digitalRead(pinSW);
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if(joyMoved==false && (xValue<400 || xValue > 600 || yValue<400 || yValue > 600 )){
    check();
    joyMoved = true;
    lastDebounceTimeForController = millis();
  }
  if(joyMoved==true && xValue>400 && xValue < 600 && yValue>400 && yValue < 600 )
    joyMoved = false;

  if(buttonPressed ==0 && swValue != lastSwValue&&  millis() - lastDebounceTimeForButton > debounceDelay)
  {
    lastDebounceTimeForButton = millis();
    buttonPressed = 1;
    buttonPressedTime = millis();
  }

  if(swValue == lastSwValue && buttonPressed==1 && millis() - lastDebounceTimeForButton > debounceDelay)
  {
    if(millis() - buttonPressedTime > longDebounceDelay)
      resetLeds();
    else
      currentState = 2;
    lastDebounceTimeForButton = millis();
    buttonPressed=0;
    Serial.println("aloha");
  }

  if(millis()-lastDebounceTimeForBlink > blinkDelay)
  {
    digitalW();
    digitalWrite(currentSeg,blinkState);
    blinkState = !blinkState;
    lastDebounceTimeForBlink = millis();
  }
}
void state2()
{
  digitalW();
  swValue = !digitalRead(pinSW);
  xValue = analogRead(pinX);
  if((xValue<400 || xValue>600)&&joyMovedState2==false)
  {
    joyMovedState2=true;
    for(int i = 0; i < segSize ; i++)
      if(segments[i] == currentSeg)
      {
        if(vectorSeg[i] == 0)
          vectorSeg[i] =1;
        else
          vectorSeg[i] = 0;
      }
  }
  if((xValue>400 && xValue<600)&&joyMovedState2==true)
    joyMovedState2 =false;
    
  if(swValue != lastSwValue&&  millis() - lastDebounceTimeForButton > debounceDelay)
  {
    Serial.println("da3123");
    lastDebounceTimeForButton = millis();
    currentState = 1;
  }

}
void digitalW()
{
  for(int i = 0; i < segSize ; i++)
      digitalWrite(segments[i], vectorSeg[i]);
}

void resetLeds()
{
  for(int i = 0; i < segSize ; i++)
    vectorSeg[i] = 0;
  currentSeg=pinDP;
}
void check()
{
  switch(currentSeg)
  {
    case pinA:
    if(yValue<400)
      currentSeg=pinG; 
    if(xValue<400)
      currentSeg=pinF;
    else if(xValue>600)
      currentSeg=pinB;
    break;

    case pinB:
    if(xValue < 400)
      currentSeg=pinF;
    if (yValue<400)
      currentSeg=pinG;
    else if(yValue>600)
      currentSeg=pinA;
    break;

    case pinC:
    if(yValue > 600)
      currentSeg=pinG;
    else if(yValue<400)
      currentSeg=pinD;
    if(xValue<400)
      currentSeg=pinE;
    else if(xValue > 600)
      currentSeg=pinDP;
    break;

    case pinD:
    if(yValue>600)
      currentSeg=pinG; 
    if(xValue<400)
      currentSeg=pinE;
    else if(xValue>600)
      currentSeg=pinC;
    break;

    case pinE:
    if(yValue > 600)
      currentSeg=pinG;
    else if(yValue<400)
      currentSeg=pinD;
    if(xValue>600)
      currentSeg=pinC;
    break;

    case pinF:
    if(xValue >600)
      currentSeg=pinB;
    if (yValue<400)
      currentSeg=pinG;
    else if(yValue>600)
      currentSeg=pinA;
    break;

    case pinG:
    if(yValue < 400)
        currentSeg = pinD;
    else if (yValue > 600)
        currentSeg = pinA;
    break;

    case pinDP:
    if(xValue < 400)
        currentSeg = pinC;
    break;

  }
}
             
