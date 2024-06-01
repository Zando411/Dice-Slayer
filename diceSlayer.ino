///////////////////////////////////////////////////////////////////////////////
// Circuit Playground D6 Dice
// 
// Roll them bones.
//
// Author: Carter Nelson
// MIT License (https://opensource.org/licenses/MIT)

#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>

AsyncDelay animationTimer;

#define DICE_COLOR          0xEA6292    // Dice digits color

unsigned long rollStartTime;
bool rightButton;
uint8_t rollNumber;


void buttonCallback() {
  rightButton = true;
}

uint8_t dicePixels[6][6] = {  // Pixel pattern for dice roll
  { 2, 0, 0, 0, 0, 0 } ,      // Roll = 1
  { 4, 9, 0, 0, 0, 0 } ,      //        2
  { 0, 4, 7, 0, 0, 0 } ,      //        3
  { 1, 3, 6, 8, 0, 0 } ,      //        4
  { 0, 2, 4, 5, 9, 0 } ,      //        5
  { 0, 2, 4, 5, 7, 9 } ,      //        6
};

void setup() {
  Serial.begin(9600);
  
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(100);
  // Seed the random function with light sensor value
  attachInterrupt(digitalPinToInterrupt(5), buttonCallback, RISING);
  // Initialize the global states
}

void loop() {
  if (rightButton) {
    delay(5);
    rollDice();
  }
  Serial.println(rollNumber);
}

int rollDice() {
  randomSeed(CircuitPlayground.lightSensor());
   bool loop = false;

  if (rightButton) {
    rollStartTime = millis();
    rightButton = false;
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