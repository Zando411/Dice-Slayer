#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>
#include <time.h>
#include <string>
#include <climits>
using namespace std;

AsyncDelay animationTimer;

int rollDiceAnimation(int maxRoll, int diceColor);
void resetButtons();

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
bool enteringRoom = true;
bool gameOver = false;
bool mute = false;


// upgrades

String upgradeNames[4] = {"Upgrade die by 2", "Add +1 to roll", "Increase max HP by 1", "Increase max reroll by 1"};

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

  void printStats() {
    Serial.println("Your current stats are: ");
    Serial.print("Max HP: ");
    Serial.println(maxHP);
    Serial.print("Max Roll: ");
    Serial.println(maxRoll);
    Serial.print("Roll Plus: ");
    Serial.println(rollPlus);
    Serial.print("Max Rerolls: ");
    Serial.println(maxReroll);
  }
  
  int rollDice() {
    resetButtons();
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
      resetButtons();
      delay(50);
      while (!leftButtonPressed || !rightButtonPressed) {
        if (leftButtonPressed && remainingReroll > 0) {
          resetButtons();
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
          } else if (remainingReroll == 0) {
            Serial.println("You are out of rerolls!");
            delay(500);
            Serial.println("Press the right button to continue.");
          }
      } else if (rightButtonPressed) {
        resetButtons();
        break;
      }
      }
    }
    return totalRoll;
  }

  void rollUpgrades() {
        int lowerBound = 0;
        if (maxRoll == 10) {
          lowerBound = 1;
        }
        int upgradeLeft = random(lowerBound,4);
        int upgradeRight = random(lowerBound,4);
        while (upgradeLeft == upgradeRight) {
          upgradeRight = random(lowerBound,4);
        }

        Serial.println("As a reward, you are presented the choice of two upgrades: ");
        Serial.print(upgradeNames[upgradeLeft]);
        Serial.println("  ---  Select this upgrade with the left button.");
        Serial.print(upgradeNames[upgradeRight]);
        Serial.println("  ---  Select this upgrade with the right button.");
        
        while (!leftButtonPressed && !rightButtonPressed);
        int upgrade;
        if (leftButtonPressed) {
          resetButtons();
          upgrade = upgradeLeft;
        }  else if (rightButtonPressed) {
          resetButtons();
          upgrade = upgradeRight;
        }
        int lastValue;
        Serial.print("Congratulations! ");
        switch (upgrade) {
        case 0:                             // increase player die by 2
        maxRoll += 2;
        Serial.print("You now roll a die with ");
        Serial.print(maxRoll);
        Serial.println(" sides!");
        break;
        case 1:                             // increase roll plus by 1
        rollPlus++;
        Serial.println("You now add an additional point to your roll.");
        break;
        case 2:                             // increase max hp by 1
        maxHP++;
        Serial.println("Your max HP has increased by 1.");
        break;
        case 3:                             // increase max rerolls by 1
        maxReroll++;
        rerollPerk = true;
        Serial.println("Your max rerolls have increased by 1.");
        break;
        }
  }

  int getHP() {
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

  void setMaxHP(int newMaxHP) {
    maxHP = newMaxHP;
  }

  void setMaxRoll(int newMaxRoll) {
    maxRoll = newMaxRoll;
  }
  
  void setRollPlus(int newRollPlus) {
    rollPlus = newRollPlus;
  }

  void setMaxReroll(int newMaxReroll) {
    maxReroll = newMaxReroll;
  }


  void setRerollPerk(bool perk) {
    rerollPerk = perk;
  }

  void setRemainingRerolls(int rerolls) {
    remainingReroll = rerolls;
  }

  void monsterSlain() {
    monstersSlayed++;
    if (monstersSlayed >= monstersNeededForLevelUp()) {
      levelUp();
    }
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
  int level;
  int monstersSlayed;

  int monstersNeededForLevelUp() {
    static int levelBreakpoints[11] = {0, 1, 3, 5, 8, 11, 15, 19, 24, 31, 40};
    if (level >= 10) return INT_MAX;
    return levelBreakpoints[level];

  }

  void levelUp() {
    level++;
    Serial.print("Congratulations! You reached level ");
    Serial.println(level);
    player.rollUpgrades();
  }

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
    resetButtons();
  }
  }
  if (gameStarted) {
    if (!endlessMode) { // Story mode
      if (!combatOver) { // if combat is still active
        if (enteringRoom) {
        player.setRemainingRerolls(player.getMaxReroll());
        delay(1000);
        Serial.print("You enter into room number: ");
        Serial.println(roomNumber);
        switch (roomNumber) {
          case 1:
          {
          monster = Monster(2, 4, 0);
          break;
          }
          
          case 2:
          {
          monster = Monster(3, 6, 0);
          break;
          }
          case 3:
          {
          monster = Monster(3, 8, 1);
          break;
          }
          case 4:
          {
          monster = Monster(4, 6, 2);
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
        Serial.println("and it attacks!");
        }
        if (!enteringRoom) {
          delay(1000);
          Serial.println("The monster goes for another strike...");
        }
        enteringRoom = false;
        int monsterRoll = monster.rollDice();
        while(!animationTimer.isExpired());
        Serial.print("It rolled a ");
        Serial.print(monsterRoll);
        Serial.println("!");
        delay(1000);
        resetButtons();
        Serial.println("Press the right button to fight back!");

        while (!rightButtonPressed);
        resetButtons();
        int playerRoll = player.rollDice();

        if (playerRoll >= monsterRoll) {
          monster.takeDamage(1);
          Serial.println("You rolled higher than the monster!");
          delay(1000);
          Serial.println("You attack the monster and it takes 1 point of damage!");
          delay(1000);
          if (monster.getHP() > 0) {
            Serial.print("The monster has ");
            Serial.print(monster.getHP());
            Serial.println(" health remaining.");
          } else if (monster.getHP() == 0) {
            Serial.println("You slayed the monster!");
            combatOver = true;
          }
        } else if (monsterRoll > playerRoll) {
          player.takeDamage(1);
          Serial.println("The monster rolled higher than you!");
          delay(1000);
          Serial.println("The monster attacks you and deals 1 point of damage!");
          delay(1000);
          if (player.getHP() > 0) {
            Serial.print("You have ");
            Serial.print(player.getHP());
            Serial.println(" health remaining.");
          } else if (player.getHP() == 0) {
            gameOver = true;
            while (gameOver) {
            Serial.println("You have died to the monster!");
            }
          }
        } 
      } else if (combatOver) {
        resetButtons();
        roomNumber++; 
        player.setRemainingRerolls(player.getMaxReroll());
        player.heal(player.getMaxHP());
        if (roomNumber > 5) {
          Serial.println("YOU HAVE WON THE GAME!");
          gameOver = true;
          while (gameOver) {
          Serial.println("game over");
          delay(1000);
          }
        }
        player.rollUpgrades();
        delay(1000);
        player.printStats();
        delay(1000);
        combatOver = false;
        enteringRoom = true;
        resetButtons();
        Serial.println("Press the right button to enter the next room.");
        while(!rightButtonPressed);
        resetButtons();
        }
      }
  }
}
  
  








void resetButtons() {
  delay(10);
  leftButtonPressed = false;
  rightButtonPressed = false;
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
  animationTimer.start(1000, AsyncDelay::MILLIS);

  // Compute a random number from 1 to max roll
  rollNumber = random(1, maxRoll+1);

  if (!(animationTimer.isExpired())) {
    loop = true;
  }

  if (switchChange) {
    delay(5);
    mute = digitalRead(switchPin); // set game mode based on switch

    switchChange = false;
  }

  // Display status on NeoPixels
  while (loop) {
    int animationRollNumber = random(1,maxRoll+1);
    // Make some noise and show the dice roll number
    if (!mute) {
    //CircuitPlayground.playTone(random(400,2000), 20, false); 
    } 
    CircuitPlayground.clearPixels();
    for (int p=0; p<animationRollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixelsD10[animationRollNumber-1][p], diceColor);
    }
    
    delay(100);    
    if (animationTimer.isExpired()) {
      loop = false;
    }
  }

  if (animationTimer.isExpired()) {
    // Show the dice roll number
    CircuitPlayground.clearPixels();
    for (int p=0; p<rollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixelsD10[rollNumber-1][p], diceColor);
    }
  }
  return rollNumber;
}