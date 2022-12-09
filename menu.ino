/*
EEPROM
--------------------------------------------------
byte        Desc
0           byte for the length of highscore
1           difficulty level
2           LCD contrast
3           LCD brightness
4           Matrix brightness
5           Sounds on/off
...         Free space for game variables
128         Highscore

9->13
8->2
--------------------------------------------------
*/
#include <LiquidCrystal.h>
#include<EEPROM.h>
#include "LedControl.h"

const byte matrixSize =8;

const byte lengthHighScore = 0;
const byte difficulty = 1;
const byte lcdContrast = 2;
const byte lcdBrightness = 3;
const byte matrixBrightness = 4;
const byte sounds = 5;
const byte startHighScore = 128;

const byte dinPin =12;
const byte clockPin =11;
const byte loadPin =10;
const byte rs = 13;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 2;
const byte d6 = 5;
const byte d7 = 4;
const byte buzzerPin = 3;

//const int pinSW = 2; // digital pin connected to switch output
const int pinX = A1; // A0 - analog pin connected to X output
const int pinY = A0;
const int LCD_Contrast = 9;
const int LCD_Backlight= 6;

int xValue = 0;
int yValue = 0;
int swValue = 0;
int lastSwValue = 0;
bool joyMoved = false;
int buzzerTone1 =1000;
int buzzerTone2 =700;
int buzzerTone3 =500;
int buzzerTone4 =300;

byte buttonPressed = 0;
unsigned long lastDebounceTimeForButton = 0;
unsigned long timeForLetter =300;
unsigned long lastTimeForLetter = 0;
unsigned long timeForPause =1500;
unsigned long lastTimeForPause = 0;
unsigned long debounceDelay =100;
unsigned long lastShown = 0;
unsigned long debounceDelayShown =20000;
unsigned long TimeInteraction = 300;
unsigned long lastTimeInteraction =0;

int difficultyLevel = 0;
int lcdContrastLevel = 0;
int lcdBrightnessLevel =0;
int matrixBrightnessLevel = 0;
bool soundLevel = true;

LedControl lc = LedControl(dinPin, clockPin, loadPin,1);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char name[6]="aaaaaa";
int letterPosition = 0;
int letterValue = (int)name[letterPosition];

byte LedPinValue=0;
int brightness;
int incomingByte=0;
byte messageShown = 0; // if the initial message was shown
int timeForExposure = 3000; // the time in ms for message 
int timeExposed;
bool clear=false; // if the lcd should be cleared
int state = 0; // 0-menu, 1-startgame, 2-highScore, 3-settings, 4-about, 5-howtoplay
String options[] = {"Start game","High score","Settings","About","How to play"};
int position = 0; // the index of options[] that will be displayed
int state2 = 0;
int position2 = 0;
String options2[] = {"Enter name","Difficulty","LCD contrast","LCD brightness","Matrix brightness","Sound on/off"};
bool shown =false;//is the variable that says if the text should be displayed or not
bool read = false;//if the data was read
String Word1;//a word
int value; //value read from EEPROM
int buzzInterval = 100;

//highscore variables
int numberOfChr = 0;
char my_string[100];



void setup() {
 // String cc="High scores: marius:1000, ion:2000           \n";
  //EEPROM.update(0,cc.length());
  //EEPROM.put(128,"High scores: marius:1000, ion:2000           \n"); 
  setLCD();//set the initial values for contrast and brightness
  soundLevel = EEPROM.read(sounds);
  pinMode(pinX,INPUT);
  pinMode(pinY,INPUT);
  lcd.begin(16, 2);
  lc.shutdown(0, false);
  lc.setIntensity(0, 1);
  lc.clearDisplay(0);
  initializeMatrix();
  Serial.begin(9600);
}

void loop() {
  initializeMatrix();
  
  //analogWrite(pinContrast,0);
  //tone(buzzerPin, buzzerTone,500);
  if(state == -1 && messageShown == 0)
    message();

  if(state ==-1 && clear==true && millis() - timeExposed > timeForExposure){
    lcd.clear();
    clear = false;
    state = 0;
  }
  
  switch(state)
  {
    case 0:menu();break;
    case 1:startGame();break;
    case 2:highscore();break;
    case 3:settings();break;
    case 4:about();break;
    case 5:howToPlay();break;
  }
}
void menu()
{
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
 
  if(joyMoved==false) {
    if(yValue<400 && position>0){
      if(soundLevel == true)
        tone(buzzerPin, buzzerTone1,buzzInterval);
      position--;
      joyMoved=true;
      shown = false;
    }
    if(yValue > 600 && position<4){
      if(soundLevel == true)
        tone(buzzerPin, buzzerTone1,buzzInterval);
      position++;
      joyMoved=true;
      shown = false;
    }
  }
  if(joyMoved==true && yValue>400 && yValue < 600)
    joyMoved = false;
  
  if(shown == false){
    waitInteraction();
    lcd.clear();
    lcd.print(options[position]);
    shown = true;
  }
  if(xValue<400){
    if(soundLevel == true)
      tone(buzzerPin, buzzerTone2,buzzInterval);
    state = position+1;//because state = 0 is for menu
 //  writeToRow2("High scores: marius:1000, ion:2000           ");
    joyMoved=true;
    shown = false;
    waitInteraction();
  }
}

void startGame()
{
  xValue = analogRead(pinX);
  if(xValue>600){
    if(soundLevel == true)
      tone(buzzerPin, buzzerTone3,buzzInterval);
    state = 0;//because state = 0 is for menu
    shown = false;
    waitInteraction();
  }
}

void highscore()
{
  xValue = analogRead(pinX);
  if(read == false)
  {
    numberOfChr = EEPROM.read(lengthHighScore); // number characters
    for(int i=0;i<numberOfChr-1;i++){
      my_string[i] = EEPROM.read(startHighScore+i);
      //Serial.print(i);
     // Serial.print(' ');
     // Serial.println(EEPROM.read(startHighScore+i));
    }
    read = true;
  }
  if(shown == false){
    lcd.clear();
    writeToRow2(my_string,numberOfChr);
    lastShown = millis();
    shown = true;
  }
  if(shown == true && millis()-lastShown>debounceDelayShown)
    shown = false;
  
  if(xValue>600){
    if(soundLevel == true)
      tone(buzzerPin, buzzerTone3,buzzInterval);
    state = 0;//because state = 0 is for menu
    shown = false;
    waitInteraction();
  }
}
//String options2[] = {"Enter name","Difficulty","LCD contrast","LCD brightness","Matrix brightness","Sound on/off"};
void settings()
{
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  if(state2 !=0)
  {
   // read = false;
    switch(state2)
    {
      case 1:EnterName();break;
      case 2:Difficulty();break;
      case 3:LCD_contrast();break;
      case 4:LCD_brightness();break;
      case 5:Matrix_brightness();break;
      case 6:Sound();break;
    }
  }
  if(joyMoved==false) {
    if(yValue<400 && position2 >0){
      if(soundLevel == true)
        tone(buzzerPin, buzzerTone1,buzzInterval);
      position2--;
      joyMoved=true;
      shown = false;
    }
    if(yValue > 600 && position2<5){
      if(soundLevel == true)
        tone(buzzerPin, buzzerTone1,buzzInterval);
      position2++;
      joyMoved=true;
      shown = false;
    }
  }
  
  if(joyMoved==true && yValue>400 && yValue < 600 && xValue>400 && xValue < 600)
    joyMoved = false;

  if(shown == false){
    waitInteraction();
    lcd.clear();
    lcd.print(options2[position2]);
    shown = true;
  }
  if(xValue<400 && joyMoved==false){
    if(soundLevel == true)
      tone(buzzerPin, buzzerTone2,buzzInterval);
    state2 = position2+1;
    joyMoved=true;
    shown = false;
    waitInteraction();
  }
  if(xValue>600 && joyMoved==false){
    if(soundLevel == true)
      tone(buzzerPin, buzzerTone3,buzzInterval);
    state = 0;//because state = 0 is for menu
    shown = false;
    joyMoved=true;
    waitInteraction();
  }
}

void about()
{
  xValue = analogRead(pinX);
  if(xValue>600){
    state = 0;//because state = 0 is for menu
    shown = false;
    tone(buzzerPin, buzzerTone3,buzzInterval);
    waitInteraction();
  }
}
void howToPlay()
{
  xValue = analogRead(pinX);
  if(xValue>600){
    state = 0;//because state = 0 is for menu
    shown = false;
    tone(buzzerPin, buzzerTone3,buzzInterval);
    waitInteraction();
  }
}
void message()
{
  lcd.clear();
  String cuv = "Welcome to the game                ";
  writeToRow1(cuv);
  messageShown = 1;
  timeExposed = millis();
  clear = true;
}

//A function that will write on the second row a given word
void writeToRow1(String word)
{
  lcd.setCursor(16, 1);
  lcd.autoscroll();
  int thisChar = 0;
  unsigned long delay =600;
  unsigned long waitedTime = 0;
  while(thisChar<word.length())
  {
    if(millis()-waitedTime>delay){
      lcd.print(word[thisChar]);
      waitedTime = millis();
      thisChar++;
    }
  }
  lcd.noAutoscroll();
}

void writeToRow2(char cuv[],int number)
{
  lcd.setCursor(0, 1);
  char ecran[] = "                ";
  int ct = 15;
  int index =0;
  int index2 =1;
  bool finish = false;
  for(int i = 0;i<numberOfChr;i++)
  {
    xValue = analogRead(pinX);
    
    if(xValue>600){
      break;
    }
    if(finish == false)
      index=0;
    else
      {
        index = index2;
        index2++;
      }
    for(int j=ct;j<16;j++)
    {
      ecran[j] = cuv[index];
      index++;
    }
    lcd.clear();
    lcd.print(ecran);
    while(millis()-lastTimeForLetter<timeForLetter)
    {

    }
    lastTimeForLetter = millis();
    if(ct == 0 && finish == false)
      finish=true;
    
    if(ct>0)
      ct--;
  }
  //delay(1500);
  if(xValue<600){
    while(millis()-lastTimeForPause<timeForPause)
    {

    }
  }
  lastTimeForPause = millis();
}

void EnterName()
{
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  Serial.println("Sunt in enter");
  
  if(joyMoved==false) {
    if(yValue<400 && letterValue<(int)'z'){
      tone(buzzerPin, buzzerTone1,buzzInterval);
      letterValue++;
      joyMoved=true;
      shown = false;
    }
    if(yValue > 600 && letterValue>(int)'a'){
      tone(buzzerPin, buzzerTone1,buzzInterval);
      letterValue--;
      joyMoved=true;
      shown = false;
    }
  }
  if(joyMoved==false) {
    if(xValue<400 && letterPosition<6){
      tone(buzzerPin, buzzerTone4,buzzInterval);
      letterPosition++;
      letterValue = name[letterPosition];
      joyMoved=true;
      shown = false;
    }
    if(xValue>600 && letterPosition == 0){
      tone(buzzerPin, buzzerTone4,buzzInterval);
      state2 = 0;//because state = 0 is for menu
      joyMoved=true;
      shown = false;
      waitInteraction();
    }
    if(xValue > 600 && letterPosition>0){
      letterPosition--;
      letterValue = name[letterPosition];
      joyMoved=true;
      shown = false;
      tone(buzzerPin, buzzerTone3,buzzInterval);
    }
  }
  if(shown == false && state2 !=0)
  {
    name[letterPosition] = (char)letterValue;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("      Name");
    lcd.setCursor(0, 1);
    lcd.print(name);
    shown = true;
    waitInteraction();
  }
  if(joyMoved==true && yValue>400 && yValue < 600 && xValue>400 && xValue < 600)
    joyMoved = false;
}
void Difficulty()
{
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
 
  if(read == false){
    difficultyLevel = EEPROM.read(difficulty);
    read = true;
  }
  
  if(joyMoved==false) {
    if(yValue<400 && difficultyLevel<16){
      difficultyLevel++;
      joyMoved=true;
      shown = false;
    }
    if(yValue > 600 && difficultyLevel>0){
      difficultyLevel--;
      joyMoved=true;
      shown = false;
    }
  }
  if(joyMoved==true && yValue>400 && yValue < 600)
    joyMoved = false;

  if(shown == false)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    Dificulty");
    char diff[14];
    for(int i = 0;i<difficultyLevel;i++)
      diff[i]='X';
    for(int i=difficultyLevel;i<15;i++)
      diff[i]=' ';
    lcd.setCursor(0,1);
    lcd.print(diff);
    shown = true;
  }

  if(xValue>600){
    EEPROM.update(difficulty,difficultyLevel);
    state2 = 0;//because state = 0 is for menu
    joyMoved=true;
    shown = false;
    tone(buzzerPin, buzzerTone3,buzzInterval);
    waitInteraction();
  }
}
void LCD_contrast()
{
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
 
  if(read == false){
    lcdContrastLevel = EEPROM.read(lcdContrast);
    read = true;
  }
  
  if(joyMoved==false) {
    if(yValue<400 && lcdContrastLevel<16){
      lcdContrastLevel++;
      joyMoved=true;
      shown = false;
    }
    if(yValue > 600 && lcdContrastLevel>0){
      lcdContrastLevel--;
      joyMoved=true;
      shown = false;
    }
  }
  if(joyMoved==true && yValue>400 && yValue < 600)
    joyMoved = false;

  if(shown == false)
  {
    lcd.clear();
    analogWrite(LCD_Contrast,map(lcdContrastLevel, 0, 16, 0, 255));
    Serial.println(map(lcdContrastLevel, 0, 16, 0, 255));
    lcd.setCursor(0,0);
    lcd.print("   LCD Contrast");
    char diff[14];
    for(int i = 0;i<lcdContrastLevel;i++)
      diff[i]='X';
    for(int i=lcdContrastLevel;i<15;i++)
      diff[i]=' ';
    lcd.setCursor(0,1);
    lcd.print(diff);
    shown = true;
  }

  if(xValue>600){
    EEPROM.update(lcdContrast,lcdContrastLevel);
    state2 = 0;//because state = 0 is for menu
    joyMoved=true;
    shown = false;
    tone(buzzerPin, buzzerTone3,buzzInterval);
    waitInteraction();
  }
}
/*
const int LCD_Contrast = 11;
const int LCD_Backlight= 10;

*/
void LCD_brightness()
{
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
 
  if(read == false){
    lcdBrightnessLevel = EEPROM.read(lcdBrightness);
    read = true;
  }
  
  if(joyMoved==false) {
    if(yValue<400 && lcdBrightnessLevel<16){
      lcdBrightnessLevel++;
      joyMoved=true;
      shown = false;
    }
    if(yValue > 600 && lcdBrightnessLevel>0){
      lcdBrightnessLevel--;
      joyMoved=true;
      shown = false;
    }
  }
  if(joyMoved==true && yValue>400 && yValue < 600)
    joyMoved = false;

  if(shown == false)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   LCD Brightness");
    char diff[14];
    for(int i = 0;i<lcdBrightnessLevel;i++)
      diff[i]='X';
    for(int i=lcdBrightnessLevel;i<15;i++)
      diff[i]=' ';
    lcd.setCursor(0,1);
    lcd.print(diff);
    shown = true;
    analogWrite(LCD_Backlight,map(lcdBrightnessLevel, 0, 16, 0, 255));
  } 

  if(xValue>600){
    EEPROM.update(lcdBrightness,lcdBrightnessLevel);
    state2 = 0;//because state = 0 is for menu
    joyMoved=true;
    shown = false;
    tone(buzzerPin, buzzerTone3,buzzInterval);
    waitInteraction();
  }
}
void Matrix_brightness()
{
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
 
  if(read == false){
    matrixBrightnessLevel = EEPROM.read(matrixBrightness);
    read = true;
  }
  
  if(joyMoved==false) {
    if(yValue<400 && matrixBrightnessLevel<16){
      matrixBrightnessLevel++;
      joyMoved=true;
      shown = false;
    }
    if(yValue > 600 && matrixBrightnessLevel>0){
      matrixBrightnessLevel--;
      joyMoved=true;
      shown = false;
    }
  }
  if(joyMoved==true && yValue>400 && yValue < 600)
    joyMoved = false;

  if(shown == false)
  {
    Serial.println(matrixBrightnessLevel);
    lc.setIntensity(0, matrixBrightnessLevel);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Matrix Brightness");
    char diff[15];
    for(int i = 0;i<matrixBrightnessLevel;i++)
      diff[i]='X';
    for(int i=matrixBrightnessLevel;i<16;i++)
      diff[i]=' ';
    lcd.setCursor(0,1);
    lcd.print(diff);
    shown = true;
  } 

  if(xValue>600){
    EEPROM.update(matrixBrightness,matrixBrightnessLevel);
    state2 = 0;//because state = 0 is for menu
    joyMoved=true;
    shown = false;
    tone(buzzerPin, buzzerTone3,buzzInterval);
    waitInteraction();
  }

}


void Sound()
{
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
 
  if(read == false){
    soundLevel = EEPROM.read(sounds);
    read = true;
  }
  
  if(joyMoved==false) {
    if(yValue<400 && soundLevel==false){
      soundLevel=true;
      joyMoved=true;
      shown = false;
    }
    if(yValue > 600 && soundLevel==true){
      soundLevel=false;
      joyMoved=true;
      shown = false;
    }
  }
  if(joyMoved==true && yValue>400 && yValue < 600)
    joyMoved = false;

  if(shown == false)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("     Sound");
    lcd.setCursor(0,1);
    if(soundLevel==false)
      lcd.print("OFF");
    else
      lcd.print("ON");
    shown = true;
  } 

  if(xValue>600){
    EEPROM.update(sounds,soundLevel);
    state2 = 0;//because state = 0 is for menu
    joyMoved=true;
    shown = false;
    waitInteraction();
  }

}
void initializeMatrix()
{
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, true); // turns on LED at col, row
    }
  }
}
void waitInteraction()
{
  lastTimeInteraction =millis();
  while(millis()-lastTimeInteraction<TimeInteraction)
  {}
}
void setLCD()
{
  lcdContrastLevel = EEPROM.read(lcdContrast);
  lcdBrightnessLevel = EEPROM.read(lcdBrightness);
  Serial.println(lcdContrastLevel);
  Serial.println(lcdBrightnessLevel);
  analogWrite(LCD_Contrast,map(lcdContrastLevel, 0, 16, 0, 255));
  analogWrite(LCD_Backlight,map(lcdContrastLevel, 0, 16, 0, 255));
}
/*
unsigned long TimeInteraction = 300;
unsigned long lastTimeInteraction =0;
while(thisChar<16)
    {
      if(millis()-waitedTime>delay){
        lcd.print(word[thisChar]);
        waitedTime = millis();
        thisChar++;
      }
    }
  for(int j=0;j<numberOfLoops;j++)
  {
    lcd.clear();
    for(int i=0;i<16;i++)
      lcd.print(word[i]);

    for(int i=0;i<word.length()-1;i++)
      word[i] = word[i+1];
  }
*/

  
