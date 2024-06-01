

#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>

AsyncDelay animationTimer;


// player variables/upgrades (inital values are defaults)
int playerMaxHP = 3;
int playerCurrentHP = playerMaxHP;
int playerMaxRoll = 6;
int playerRollPlus = 0;
bool rerollPerk = false;
bool remainingReroll = false;
int playerRoll;

// current monster variables
int monsterHP = 3;
int monsterMaxRoll = 6;
int monsterRollPlus = 0;
int monsterRoll;

// function variables
#define DICE_COLOR          0xEA6292    // Dice digits color
bool rightButtonPressed;
bool leftButtonPressed;
bool switchChange;
int rollNumber;
int rightButtonPin = 5;
int leftButtonPin = 4;
int switchPin = 7;
bool endlessMode = false;
bool gameStarted = false;
bool combatOver = false;
// ISR
void rightISR() {
  rightButtonPressed = true;
}
void leftISR() {
  leftButtonPressed = true;
}
void switchISR() {
  switchChange = true;
}
// Dice array
int dicePixels[6][6] = {  // Pixel pattern for dice roll
  { 2, 0, 0, 0, 0, 0 } ,      // Roll = 1
  { 4, 9, 0, 0, 0, 0 } ,      //        2
  { 0, 4, 7, 0, 0, 0 } ,      //        3
  { 1, 3, 6, 8, 0, 0 } ,      //        4
  { 0, 2, 4, 5, 9, 0 } ,      //        5
  { 0, 2, 4, 5, 7, 9 } ,      //        6
};
// Monster arrays

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(50);
  // Seed the random function with light sensor value
  attachInterrupt(digitalPinToInterrupt(rightButtonPin), rightISR, RISING);
  attachInterrupt(digitalPinToInterrupt(leftButtonPin), leftISR, RISING);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, CHANGE);
  
  while(!Serial); 
  delay(500);
  Serial.println("Welcome to Dice Slayer!");
  delay(1000);
  Serial.println("Please select a mode by changing the switch.");
  delay(1000);
  Serial.println("Press the right button to begin!");
}

void loop() {
  if (!gameStarted) { // check if game has not started
    if (switchChange) {
    delay(5);
    endlessMode = digitalRead(switchPin); // set game mode based on switch
    
    Serial.print("You have selected: ");
    if (endlessMode) Serial.println("Endless Mode");
    if (!endlessMode) Serial.println("Story Mode");

    switchChange = false;
  }
  if (rightButtonPressed) {
    delay(300);
    Serial.println("Let the game begin!");
    gameStarted = true;
    rightButtonPressed = false;
  }
  }
  if (gameStarted) {
    if (!endlessMode) { // Story mode
    
  }
  }
  
  
}

///////////////////////////////////////////////////////////////////////////////
// Circuit Playground Dice
// 
// Modified to be used with a button input and to be only called once rather than looped.
//
// Author: Carter Nelson
// MIT License (https://opensource.org/licenses/MIT)

int rollDice() {
  randomSeed(CircuitPlayground.lightSensor());
   bool loop = false;

  if (rightButtonPressed) {
    rightButtonPressed = false;
    animationTimer.start(1000, AsyncDelay::MILLIS);
  }
  
  // Compute a random number from 1 to 6
  rollNumber = random(1,7);

  if (!(animationTimer.isExpired())) {
    loop = true;
  }

  // Display status on NeoPixels
  while (loop) {
    int animationRollNumber = random(1,7);
    // Make some noise and show the dice roll number
    //CircuitPlayground.playTone(random(400,2000), 20, false);        
    CircuitPlayground.clearPixels();
    for (int p=0; p<animationRollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixels[animationRollNumber-1][p], DICE_COLOR);
    }    
    delay(200);    
    if (animationTimer.isExpired()) {
      loop = false;
    }
  }

  if (animationTimer.isExpired()) {
    // Show the dice roll number
    CircuitPlayground.clearPixels();
    for (int p=0; p<rollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixels[rollNumber-1][p], DICE_COLOR);
    }
  }
}