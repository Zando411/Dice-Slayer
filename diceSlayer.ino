#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>
#include <time.h>

AsyncDelay animationTimer;

int rollDiceAnimation(int maxRoll, int diceColor);

// function variables
volatile bool rightButtonPressed;
volatile bool leftButtonPressed;
volatile bool switchChange;
int rollNumber;
int rightButtonPin = 5;
int leftButtonPin = 4;
int switchPin = 7;
bool endlessMode = false;
bool gameStarted = false;
bool combatOver = false;
int roomNumber = 1;

// PLayer Class

class Player {
  public:
  // Constructor
  Player(int maxHP, int maxRoll, int rollPlus, int maxReroll):
  maxHP(maxHP), currentHP(maxHP), maxRoll(maxRoll), rollPlus(rollPlus), rerollPerk(false), maxReroll(maxReroll) {}


  void takeDamage(int damage) { // method to deal damage to player
    currentHP -= damage;
    if (currentHP < 0) currentHP = 0;
  }

  void heal(int amount) { // method to heal player
    currentHP += amount;
    if (currentHP > maxHP) currentHP = maxHP; 
  }

  int rollDice() {
    int roll = rollDiceAnimation(maxRoll, diceColor);
    Serial.print("You rolled a ");
    Serial.print(roll);
    Serial.println("!");
    delay(500);
    Serial.print("With your plus of ");
    Serial.print(rollPlus);
    Serial.print(" your total is: ");
    int totalRoll = roll + rollPlus;
    Serial.println(totalRoll);
    delay(500);
    if (rerollPerk && remainingReroll > 0) {
      Serial.print("You have the reroll perk and ");
      Serial.print(remainingReroll);
      Serial.println(" remaining rerolls.");
      delay(500);
      Serial.println("If you would like to roll again and add to your total, press the left button.");
      delay(500);
      Serial.println("If you would like to continue without adding to your roll, press the right button.");
      while (!leftButtonPressed || !rightButtonPressed) {
        if (leftButtonPressed) {
          rightButtonPressed = false;
          totalRoll = doReroll(totalRoll);
          leftButtonPressed = false;
          Serial.print("Your new roll total is: ");
          Serial.print(totalRoll);
          Serial.println("!");
          delay(500);
          if (remainingReroll > 0) {
            Serial.println("If you want to roll again, press the left button.");
            delay(500);
            Serial.println("If you want to continue, press the right button.");
          } else if (remainingReroll = 0) {
            Serial.println("You are out of rerolls!");
            delay(500);
            Serial.println("Press the right button to continue.");
          }
      } else if (rightButtonPressed) {
        rightButtonPressed = false;
        break;
      }
      }
    }
    return totalRoll;
  }

  int getCurrentHP() {
    return currentHP;
  }

  int getMaxHP() {
    return maxHP;
  }

  int getMaxRoll() {
    return maxRoll;
  }

  int getRollPlus() {
    return rollPlus;
  }

  int getMaxReroll() {
    return maxReroll;
  }

  bool hasRerollPerk() {
    return rerollPerk;
  }

  void setRerollPerk(bool perk) {
    rerollPerk = perk;
  }

  void setRemainingRerolls(int rerolls) {
    remainingReroll = rerolls;
  }

  private:
  int maxHP;
  int currentHP;
  int maxRoll;
  int rollPlus;
  bool rerollPerk;
  int remainingReroll;
  int maxReroll;
  int diceColor = 0xEA6292;

  int doReroll(int currentTotal) {
    int rerollRoll = rollDiceAnimation(maxRoll, diceColor);
      Serial.print("You rolled a ");
      Serial.print(rerollRoll);
      Serial.println("!");
      currentTotal += rerollRoll;
      remainingReroll--;
      return currentTotal;
  }
  
};


// monster class

class Monster {
  public:
  // Constructor
  Monster(int hp, int maxRoll, int rollPlus):
  hp(hp), maxRoll(maxRoll), rollPlus(rollPlus) {}


  void takeDamage(int damage) { // method to deal damage to monster
    hp -= damage;
    if (hp < 0) hp = 0;
  }

  int rollDice() {
    return rollDiceAnimation(maxRoll, diceColor) + rollPlus;
  }

  int getHP() {
    return hp;
  }

  int getMaxRoll() {
    return maxRoll;
  }

  int getRollPlus() {
    return rollPlus;
  }

  private:
  int hp;
  int maxRoll;
  int rollPlus;
  int diceColor = 0xF10404;
};




Player player(3, 6, 0, 0); // initialize player
Monster monster(0, 0, 0); // initalize monster
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
int dicePixelsD6[6][6] = {  // Pixel pattern for dice roll
  { 2, 0, 0, 0, 0, 0 } ,      // Roll = 1
  { 4, 9, 0, 0, 0, 0 } ,      //        2
  { 0, 4, 7, 0, 0, 0 } ,      //        3
  { 1, 3, 6, 8, 0, 0 } ,      //        4
  { 0, 2, 4, 5, 9, 0 } ,      //        5
  { 0, 2, 4, 5, 7, 9 } ,      //        6
};

int dicePixelsD8[8][8] = {  // Pixel pattern for dice roll
  { 2, 0, 0, 0, 0, 0, 0, 0 } ,      // Roll = 1
  { 4, 9, 0, 0, 0, 0, 0, 0 } ,      //        2
  { 0, 4, 7, 0, 0, 0, 0, 0 } ,      //        3
  { 1, 3, 6, 8, 0, 0, 0, 0 } ,      //        4
  { 0, 2, 4, 5, 9, 0, 0, 0 } ,      //        5
  { 0, 2, 4, 5, 7, 9, 0, 0 } ,      //        6
  { 0, 1, 4, 5, 6, 8, 9, 0 } ,      //        7
  { 0, 1, 3, 4, 5, 6, 8, 9 } ,      //        8
};

int dicePixelsD10[10][10] = {  // Pixel pattern for dice roll
  { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ,      // Roll = 1
  { 4, 9, 0, 0, 0, 0, 0, 0, 0, 0 } ,      //        2
  { 0, 4, 7, 0, 0, 0, 0, 0, 0, 0 } ,      //        3
  { 1, 3, 6, 8, 0, 0, 0, 0, 0, 0 } ,      //        4
  { 0, 2, 4, 5, 9, 0, 0, 0, 0, 0 } ,      //        5
  { 0, 2, 4, 5, 7, 9, 0, 0, 0, 0 } ,      //        6
  { 0, 1, 4, 5, 6, 8, 9, 0, 0, 0 } ,      //        7
  { 0, 1, 3, 4, 5, 6, 8, 9, 0, 0 } ,      //        8
  { 0, 1, 2, 3, 4, 5, 6, 8, 9, 0 } ,      //        9
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } ,      //        10
};

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(50);
  // Seed the random function with light sensor value
  attachInterrupt(digitalPinToInterrupt(rightButtonPin), rightISR, RISING);
  attachInterrupt(digitalPinToInterrupt(leftButtonPin), leftISR, RISING);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, CHANGE);
  randomSeed(time(0) + CircuitPlayground.lightSensor());

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
    else if (!endlessMode) Serial.println("Story Mode");

    switchChange = false;
  }
  if (rightButtonPressed) {
    delay(300);
    Serial.println("Let the game begin!");
    delay(1000);
    gameStarted = true;
    rightButtonPressed = false;
  }
  }
  if (gameStarted) {
    if (!endlessMode) { // Story mode
      if (!combatOver) { // if combat is still active
        delay(1000);
        Serial.print("You enter into room number: ");
        Serial.println(roomNumber);
        switch (roomNumber) {
          case 1:
          {
          monster = Monster(2, 6, 0);
          break;
          }
          
          case 2:
          {
          monster = Monster(3, 6, 1);
          break;
          }
          case 3:
          {
          monster = Monster(3, 8, 0);
          break;
          }
          case 4:
          {
          monster = Monster(4, 6, 1);
          break;
          }
          case 5:
          {
          monster = Monster(3, 10, 2);
          break;
          }
         }
        delay(1000);
        Serial.print("In front of you, you see a monster");
        for (int i = 0; i < 3; i++) {
          delay(1000);
          Serial.print(".");
        }
        Serial.println("and they attack!");
        int monsterRoll = monster.rollDice();
        while(!animationTimer.isExpired());
        Serial.print("It rolled a ");
        Serial.print(monsterRoll);
        Serial.println("!");
        delay(2000);
        Serial.println("Press the right button to fight back!");

        while (!rightButtonPressed);
        leftButtonPressed = false;
        rightButtonPressed = false;
        player.setRerollPerk(true);
        player.setRemainingRerolls(5);
        int playerRoll = player.rollDice();
        Serial.println("combat time");
        while (!rightButtonPressed);
        
      }
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

int rollDiceAnimation(int maxRoll, int diceColor) {
   bool loop = false;
  
  // rightButtonPressed = false;
  animationTimer.start(1000, AsyncDelay::MILLIS);

  
  // Compute a random number from 1 to 6
  rollNumber = random(1, maxRoll+1);

  if (!(animationTimer.isExpired())) {
    loop = true;
  }

  // Display status on NeoPixels
  while (loop) {
    int animationRollNumber = random(1,maxRoll+1);
    // Make some noise and show the dice roll number
    //CircuitPlayground.playTone(random(400,2000), 20, false);        
    CircuitPlayground.clearPixels();
    switch (maxRoll) {
    case 6:
    for (int p=0; p<animationRollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixelsD6[animationRollNumber-1][p], diceColor);
    }    
    break;
    case 8:
    for (int p=0; p<animationRollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixelsD8[animationRollNumber-1][p], diceColor);
    }    
    break;
    case 10:
    for (int p=0; p<animationRollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixelsD10[animationRollNumber-1][p], diceColor);
    }    
    break;
    }
    
    delay(100);    
    if (animationTimer.isExpired()) {
      loop = false;
    }
  }

  if (animationTimer.isExpired()) {
    // Show the dice roll number
    CircuitPlayground.clearPixels();
    switch (maxRoll) {
    case 6:
    for (int p=0; p<rollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixelsD6[rollNumber-1][p], diceColor);
    }
    break;
    case 8:
    for (int p=0; p<rollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixelsD8[rollNumber-1][p], diceColor);
    }
    break;
    case 10:
    for (int p=0; p<rollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixelsD10[rollNumber-1][p], diceColor);
    }
    break;
    }
  }
  return rollNumber;
}