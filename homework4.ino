//DS= [D]ata [S]torage - data
//STCP= [ST]orage [C]lock [P]in latch
//SHCP= [SH]ift register [C]lock [P]in clock

const int latchPin = 11; // STCP to 12 on Shift Register
const int clockPin = 10; // SHCP to 11 on Shift Register
const int dataPin = 12; // DS to 14 on Shift Register
const int pinX = A0;
const int pinY = A1;
const int pinSW=2;
const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;

const byte regSize = 8; // 1 byte aka 8 bits

int displayDigits[] = {
  segD1, segD2, segD3, segD4
};

const int encodingsNumber = 16;

int byteEncodings[encodingsNumber] = {
//A B C D E F G DP 
  B11111100, // 0 
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
  B11101110, // A
  B00111110, // b
  B10011100, // C
  B01111010, // d
  B10011110, // E
  B10001110  // F
};

int encodingArray[encodingsNumber][regSize] = {
  {1, 1, 1, 1, 1, 1, 0, 0},  // 0 B11111100
  {0, 1, 1, 0, 0, 0, 0, 0},  // 1
  {1, 1, 0, 1, 1, 0, 1, 0},  // 2
  {1, 1, 1, 1, 0, 0, 1, 0},  // 3
  {0, 1, 1, 0, 0, 1, 1, 0},  // 4
  {1, 0, 1, 1, 0, 1, 1, 0},  // 5
  {1, 0, 1, 1, 1, 1, 1, 0},  // 6
  {1, 1, 1, 0, 0, 0, 0, 0},  // 7
  {1, 1, 1, 1, 1, 1, 1, 0},  // 8
  {1, 1, 1, 1, 0, 1, 1, 0},  // 9
  {1, 1, 1, 0, 1, 1, 1, 0},  // A
  {0, 0, 1, 1, 1, 1, 1, 0},  // b
  {1, 0, 0, 1, 1, 1, 0, 0},  // C
  {0, 1, 1, 1, 1, 0, 1, 0},  // d
  {1, 0, 0, 1, 1, 1, 1, 0},  // E
  {1, 0, 0, 0, 1, 1, 1, 0},  // F
};

const int displayCount = 4;
int minThreshold = 400;
int maxThreshold = 600;
unsigned int blinkDelay =500;
unsigned int debounceDelay =200;
unsigned long lastDebounceTimeForBlink = 0;
unsigned long lastDebounceTimeForButton = 0;
unsigned int longDebounceDelay =3000;
int registers[regSize];
int values[4]={0,0,0,0};//valorile de pe ecran
int currentDigit = 0;
bool blinkState = true;
int xValue = 0;
int yValue = 0;
int swValue = 0;
int lastSwValue = 0;
bool joyMoved = false;
unsigned long buttonPressedTime = 0;
void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(pinX,INPUT);
  pinMode(pinY,INPUT);
  pinMode(pinSW,INPUT_PULLUP);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  Serial.begin(9600);
}
int currentState = 1;
byte buttonPressed = 0;
void loop() {
  if(currentState == 1)
    state1();
  else
    state2();
}

void state1()
{
  xValue = analogRead(pinX);
  swValue = !digitalRead(pinSW);

  if(joyMoved == false){
    if(xValue<minThreshold && currentDigit<3)
      currentDigit++;
    if(xValue>maxThreshold && currentDigit>0)
      currentDigit--;
    joyMoved =!joyMoved;
  }
  if(joyMoved==true && xValue>minThreshold && xValue < maxThreshold)
    joyMoved = false;

  writeNumber();
  if(millis()-lastDebounceTimeForBlink > blinkDelay){
    blinkState = !blinkState;
    lastDebounceTimeForBlink = millis();
  }
  if(buttonPressed ==0 && swValue != lastSwValue&&  millis() - lastDebounceTimeForButton > debounceDelay)
  {
    lastDebounceTimeForButton = millis();
    buttonPressedTime = millis();
    buttonPressed = 1;
  }
  if(swValue == lastSwValue && buttonPressed==1 && millis() - lastDebounceTimeForButton > debounceDelay)
  {
    if(millis() - buttonPressedTime > longDebounceDelay)
      resetLeds();
    else
    {
      currentState = 2;
      blinkState = true;
    }
    lastDebounceTimeForButton = millis();
    buttonPressed=0;
  }
  
}

void state2()
{
  yValue = analogRead(pinY);
  swValue = !digitalRead(pinSW);
  if(joyMoved == false){
    if(yValue<minThreshold && values[currentDigit]>0)
      values[currentDigit]--;
    if(yValue>maxThreshold && values[currentDigit]<15)
      values[currentDigit]++;
    joyMoved =!joyMoved;
  }
  if(joyMoved==true && yValue>minThreshold && yValue < maxThreshold)
    joyMoved = false;
  writeNumber();
  if(swValue != lastSwValue&&  millis() - lastDebounceTimeForButton > debounceDelay)
  {
    lastDebounceTimeForButton = millis();
    currentState =1;
  }
}
void resetLeds()
{
  for(int i=0; i<displayCount; i++)
    values[i] = 0;
  currentDigit = 0;
}

void activateDisplay(int digit) {
  for(int i=0; i<displayCount; i++) 
    digitalWrite(displayDigits[i], HIGH);
  digitalWrite(displayDigits[digit], LOW);
}

void writeReg(int encoding) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
  
}

void writeNumber() {
  for(int i=0;i<displayCount;i++)
  {
    if(i == currentDigit)
      writeReg(byteEncodings[values[i]]+blinkState);
    else
      writeReg(byteEncodings[values[i]]);
    activateDisplay(i);
    delay(5);
  }
}
