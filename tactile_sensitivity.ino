/////////////////////////////////////////////////////////////////////////////////////////
// Code for SID, the Biodesign project of group 7 in BME 370 at UT Austin              //
// Contributors: Walker Tait, Angela Pak, Jacob Stambaugh, Mansi Gattani, Shruti Kumar //
// Contact Email: walkertait@utexas.edu                                                //
// Last Updated: 10/21/20 9:42 pm                                                      //
/////////////////////////////////////////////////////////////////////////////////////////

// SERIAL CONNECTION ON PINS 0&1 (RX&TX) FOR BLUETOOTH MODULE
#include <SoftwareSerial.h>
SoftwareSerial HM10(0, 1);
const int bluetooth = 0;

// VIBRATIONAL AMPLITUDE LEVELS
const int level1 = 65;
const int level2 = 60;
const int level3 = 55;
const int level4 = 50;
const int level5 = 45;
const int level6 = 40;
const int level7 = 35;
const int level8 = 30;
const int level9 = 25;
const int level10 = 20;
const int levels[10] = {level1, level2, level3, level4, level5, level6, level7, level8, level9, level10};
int levelCounter = 4;
int currentLevel = levels[levelCounter];

// PINS FOR VIBRATION MOTORS BY FINGER
const int pointer = 3;
const int middle = 5;
const int ring = 6;
const int pinky = 9;
int fingers[4] = {pointer, middle, ring, pinky};

// BUTTON INTERRUPT ON PIN 2
const int button = 2;
volatile bool buttonPressed = false;

// VIBRATION ROUND VARIABLES
float overallResults[100];   // MAX NUMBER OF ROUNDS AT ONE TIME IS 100
int overallCounter = 0;
const int vibrationsPerRound = 10;
int roundResults[vibrationsPerRound];
const float passing = 70;

void setup() {
  // WHERE CALIBRATION SEQUENCE CODE WILL GO IN THE FUTURE
  // SETTING PWM PINS FOR VIBRATION MOTORS
  pinMode(pointer, OUTPUT);
  pinMode(middle, OUTPUT);
  pinMode(ring, OUTPUT);
  pinMode(pinky, OUTPUT);
  // SETTING BUTTON PIN TO BE INPUT PULLUP TO STOP FLOATING
  pinMode(button, INPUT_PULLUP);
  // SETTING BLUETOOTH MODULE PIN AND STARTING SERIAL CONNECTION
  pinMode(bluetooth, OUTPUT);
  Serial.begin(9600);
  waitToStart();
}

// FUNCTION TO DELAY START OF TRAINING UNTIL ANY OF FOUR BUTTONS ARE PRESSED
void waitToStart() {
  while (digitalRead(button) == 1) {}
  delay(1000);
}

// MAIN LOOP FOR VIBRATION TRAINING
void loop() {
  for (int i = 0; i < vibrationsPerRound; i++) {
    // LOOP FOR SENDING AN INDIVIDUAL VIBRATION TO A RANDOM FINGER
    int finger = fingers[random(0, 4)];
    int vibrationTime = random(2000,5001);   // VIBRATION SENT FOR RANDOM TIME BETWEEN 2 AND 5 SECONDS
    analogWrite(finger, currentLevel);    // SEND VIBRATION TO FINGER
    delayFunction(vibrationTime);
    analogWrite(finger, 0);   // STOP SENDING VIBRATION
    delay(5000);    // RECOVERY TIME
    if (buttonPressed == true) {    // RECORD IF VIBRATION WAS DETECTED
      roundResults[i] = 1;
    } else {
      roundResults[i] = 0;
    }
    buttonPressed = false;
  }
  // CALCULATE AND DISPLAY ACCURACY OF ROUND ON WEB APP (BLUETOOTH)
  float percentCorrect = calcResults(roundResults);
  Serial.println("Round " + String((overallCounter + 1)) + " = " + String(percentCorrect) + "%");
  // CHANGE VIBRATIONAL AMPLITUDE LEVEL BASED ON RESULTS FROM THIS ROUND
  if (percentCorrect >= passing) {
    changeLevel(true);
  } else {
    changeLevel(false);
  }
  overallResults[overallCounter] = percentCorrect;    // STORE ROUND RESULTS
  overallCounter++;
  if(overallCounter > 99) {
    stopExecution();    // END TRAINING IF THERE HAS BEEN 100 ROUNDS 
  }
}

// FUNCTION TO CHECK BUTTON WHILE VIBRATION IS SENT
void delayFunction(int delayTime) {
  unsigned long curTime = millis();
  while (millis() - curTime < delayTime) {
    if (digitalRead(button) == 0) {
      // BUTTON HAS BEEN PRESSED SO RETURN TO MAIN LOOP AND STOP VIBRATION
      buttonPressed = true;
      return;
    }
  }
}

// FUNCTION TO CALCULATE THE RESULTS OF A ROUND
float calcResults(int results[]) {
  int sum = 0;
  for (int i = 0; i < sizeof(results) / sizeof(int); i++) {
    if (results[i] == 1) {
      sum++;
    }
  }
  return (float) (sum / (sizeof(results) / sizeof(int)))*100;
}

// FUNCTION TO CHANGE THE VIBRATIONAL AMPLITUDE LEVEL
void changeLevel(bool direction) {
  if (direction == true) {
    if (currentLevel == level10) {
      stopExecution();    // END TRAINING IF > 70% ACCURATE ON LOWEST VIBRATIONAL AMPLITUDE
    }
    levelCounter++;   // DECREASE THE VIBRATIONAL AMPLITUDE IF >= 70%
  } else {
    levelCounter--;   // INCREASE THE VIBRATIONAL AMPLITUDE IF < 70%
  }
  currentLevel = levels[levelCounter];
}

// FUNCTION TO END THE TRAINING SESSION
void stopExecution() {
  while (1);
}
