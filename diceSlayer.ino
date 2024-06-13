#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>
#include <time.h>
#include <string>
#include <climits>
using namespace std;

AsyncDelay animationTimer;

// predeclare variables so they are accessable 

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

String upgradeNames[4] = { "Upgrade die by 2", "Add +1 to roll", "Increase max HP by 1", "Increase max reroll by 1" };

// Player Class

class Player { 
public:
  // Constructor
  Player(int maxHP, int maxRoll, int rollPlus, int maxReroll)
    : maxHP(maxHP), currentHP(maxHP), maxRoll(maxRoll), rollPlus(rollPlus), rerollPerk(false), maxReroll(maxReroll) {}

  // method to deal damage to player
  void takeDamage(int damage) {  
    currentHP -= damage;
    if (currentHP < 0) currentHP = 0;
  }
  // method to heal player
  void heal(int amount) {  
    currentHP += amount;
    if (currentHP > maxHP) currentHP = maxHP;
  }
  // method to print stats
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
  // method to roll player dice
  int rollDice() {
    resetButtons();
    int roll = rollDiceAnimation(maxRoll, diceColor); // get roll value from global roll function, plays animation and results in int
    Serial.print("You rolled a ");
    Serial.print(roll);
    Serial.println("!");
    delay(1000);
    Serial.print("With your plus of ");
    Serial.print(rollPlus);
    Serial.print(" your total is: ");
    int totalRoll = roll + rollPlus;
    Serial.println(totalRoll);
    delay(1000);
    if (rerollPerk && remainingReroll > 0) { // activates if the player has rerolls
      Serial.print("You have the reroll perk and ");
      Serial.print(remainingReroll);
      Serial.println(" remaining rerolls.");
      delay(500);
      Serial.println("If you would like to roll again and add to your total, press the left button.");
      delay(500);
      Serial.println("If you would like to continue without adding to your roll, press the right button.");
      resetButtons();
      delay(50);
      while (!leftButtonPressed || !rightButtonPressed) { // will run until player has pressed the right button
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
    return totalRoll; // returns roll total to compare to monster
  }

  void rollUpgrades() { // method to roll upgrade choices to player
    int lowerBound = 0;
    if (maxRoll == 10) { // ensures max roll can not be higher than can be displayed (10 neopixels)
      lowerBound = 1;
    }
    int upgradeLeft = random(lowerBound, 4);
    int upgradeRight = random(lowerBound, 4);
    while (upgradeLeft == upgradeRight) { // ensures both upgrades are different
      upgradeRight = random(lowerBound, 4);
    }
    delay(1000);
    Serial.println("As a reward, you are presented the choice of two upgrades: ");
    delay(500);
    Serial.print(upgradeNames[upgradeLeft]);
    Serial.println("  ---  Select this upgrade with the left button.");
    delay(500);
    Serial.print(upgradeNames[upgradeRight]);
    Serial.println("  ---  Select this upgrade with the right button.");
    while (!leftButtonPressed && !rightButtonPressed);
    int upgrade;
    if (leftButtonPressed) {
      resetButtons();
      upgrade = upgradeLeft;
    } else if (rightButtonPressed) {
      resetButtons();
      upgrade = upgradeRight;
    }
    int lastValue;
    Serial.print("Congratulations! ");
    switch (upgrade) {
      case 0:  // increase player die by 2
        maxRoll += 2;
        Serial.print("You now roll a die with ");
        Serial.print(maxRoll);
        Serial.println(" sides!");
        break;
      case 1:  // increase roll plus by 1
        rollPlus++;
        Serial.println("You now add an additional point to your roll.");
        break;
      case 2:  // increase max hp by 1
        maxHP++;
        Serial.println("Your max HP has increased by 1.");
        break;
      case 3:  // increase max rerolls by 1
        maxReroll++;
        rerollPerk = true;
        Serial.println("Your max rerolls have increased by 1.");
        break;
    }
    delay(1000);
    printStats();
  }

  void monsterSlain() { // used in endless to determine leveling
    monstersSlayed++;
    delay(1000);
    Serial.print("Monsters Slain: ");
    Serial.println(monstersSlayed);
    int neededForLevelup = monstersNeededForLevelUp(); // gets value necessary from array in function
    if (monstersSlayed >= neededForLevelup) { // levels up if player has killed enough monsters
      levelUp();
    } else { // alerts the player how long away from level up
      delay(1000);
      int remaining = neededForLevelup - monstersSlayed; 
      Serial.print("You need to slay ");
      Serial.print(remaining);
      if (remaining == 1) {
        Serial.println(" more monster to level up.");
      } else {
        Serial.println(" more monsters to level up.");
      }
    }
  }
  // getters
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

  int getLevel() {
    return level;
  }

  bool hasRerollPerk() {
    return rerollPerk;
  }

  // setters
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

private:
  int maxHP;
  int currentHP;
  int maxRoll;
  int rollPlus;
  bool rerollPerk;
  int remainingReroll;
  int maxReroll;
  int diceColor = 0xEA6292;
  int level = 1;
  int monstersSlayed;

  int monstersNeededForLevelUp() {
    static int levelBreakpoints[11] = { 0, 1, 3, 5, 8, 11, 15, 19, 24, 31, 40 };
    if (level >= 10) return INT_MAX; // returns int limit, making it impossible to go over level 10
    return levelBreakpoints[level];
  }

  void levelUp() {
    level++;
    delay(1000);
    Serial.print("Congratulations! You reached level ");
    Serial.println(level);
    rollUpgrades();
  }

  int doReroll(int currentTotal) { // rolls when player chooses to reroll and decrements remaining rerolls
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
  Monster(int hp, int maxRoll, int rollPlus)
    : hp(hp), maxRoll(maxRoll), rollPlus(rollPlus) {}

  // method to deal damage to monster
  void takeDamage(int damage) {  
    hp -= damage;
    if (hp < 0) hp = 0;
  }
  // monster dice roll method
  int rollDice() {
    return rollDiceAnimation(maxRoll, diceColor) + rollPlus;
  }
  // getters
  int getHP() {
    return hp;
  }

  int getMaxRoll() {
    return maxRoll;
  }

  int getRollPlus() {
    return rollPlus;
  }

  void rollUpgrades(int playerLevel) { // method to generate monsters based on player level, used in endless only

    if (playerLevel == 1) { // give base monster no upgrades
      return;
    }

    int lowerBound = 0;


    for (int i = 0; i < playerLevel-1; i++) {
      if (maxRoll == 10) { // ensures max roll can not be higher than can be displayed (10 neopixels)
        lowerBound = 1;
      }
      int upgrade = random(lowerBound, 4);
      int nothingChance;
      Serial.println("monster upgrade");

      // as the player levels, decrease the odds of monster upgrades being nothing
      // example cases: player is level 4, all 4 upgrades will be rolled flat (25% chance of nothing)
      // player is level 5, rolls a 33% chance for that original nothing to stay nothing (making it much less likely for it to be nothing)

      if (upgrade == 3) {
        if (playerLevel >= 5) {
          nothingChance = random(0, 3);
          if (nothingChance != 1) {
            upgrade = random(lowerBound, 3);
          }
        } else if (playerLevel > 7) {
          nothingChance = random(0, 5);
          if (nothingChance != 2) {
            upgrade = random(lowerBound, 3);
          } else if (playerLevel = 10)
            nothingChance = random(0, 9);
          if (nothingChance != 5) {
            upgrade = random(lowerBound, 3);
          }
        }
      }

      switch (upgrade) {
        case 0:  // increase player die by 2
          maxRoll += 2;
          break;
        case 1:  // increase roll plus by 1
          rollPlus++;
          break;
        case 2:  // increase hp by 1
          hp++;
          break;
        case 3:  // rolls no upgrade
        // nothing 
          break;
      }
    }
  }

private:
  int hp;
  int maxRoll;
  int rollPlus;
  int diceColor = 0xF10404;
};
// initalize players and monsters globally.
Player player(3, 6, 0, 0);
Monster monster(0, 0, 0);
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

int dicePixelsD10[10][10] = {
  // Pixel pattern for dice roll
  { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  // Roll = 1
  { 4, 9, 0, 0, 0, 0, 0, 0, 0, 0 },  //        2
  { 0, 4, 7, 0, 0, 0, 0, 0, 0, 0 },  //        3
  { 1, 3, 6, 8, 0, 0, 0, 0, 0, 0 },  //        4
  { 0, 2, 4, 5, 9, 0, 0, 0, 0, 0 },  //        5
  { 0, 2, 4, 5, 7, 9, 0, 0, 0, 0 },  //        6
  { 0, 1, 4, 5, 6, 8, 9, 0, 0, 0 },  //        7
  { 0, 1, 3, 4, 5, 6, 8, 9, 0, 0 },  //        8
  { 0, 1, 2, 3, 4, 5, 6, 8, 9, 0 },  //        9
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },  //        10
};

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(10);
  attachInterrupt(digitalPinToInterrupt(rightButtonPin), rightISR, RISING);
  attachInterrupt(digitalPinToInterrupt(leftButtonPin), leftISR, RISING);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, CHANGE);
  randomSeed(time(0) + CircuitPlayground.lightSensor()); // creates random seed by combining current time and light sensor data
  endlessMode = digitalRead(switchPin);

  while (!Serial);
  delay(500);
  Serial.println("Welcome to Dice Slayer!");
  delay(1000);
  Serial.println("Please select a mode by changing the switch.");
  delay(1000);
  Serial.println("Press the right button to begin!");
  delay(500);
  Serial.print("You are currently on: ");
    if (endlessMode) Serial.println("Endless Mode");
    else if (!endlessMode) Serial.println("Story Mode");
}

void loop() {
  if (!gameStarted) {  // check if game has not started
    if (switchChange) {
      delay(5);
      endlessMode = digitalRead(switchPin);  // set game mode based on switch

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
    if (!endlessMode) {   // Story mode
      if (!combatOver) {  // if combat is still active
        if (enteringRoom) { // if first time entering a room
          player.setRemainingRerolls(player.getMaxReroll()); // refresh player rerolls
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
        if (!enteringRoom) { // after first round of combat
          delay(1000);
          Serial.println("The monster goes for another strike...");
        }
        enteringRoom = false;
        int monsterRoll = monster.rollDice();
        while (!animationTimer.isExpired()); // wait for roll animation
        Serial.print("It rolled a ");
        Serial.print(monsterRoll);
        Serial.println("!");
        delay(1000);
        resetButtons();
        Serial.println("Press the right button to fight back!");
        while (!rightButtonPressed); // wait for player to continue 
        resetButtons();
        int playerRoll = player.rollDice();

        findWinner(playerRoll, monsterRoll); // compare player and monster dice rolls

      } else if (combatOver) {
        resetButtons();
        roomNumber++;
        if (roomNumber > 5) {
          gameOver = true;
          Serial.println("You beat all 5 rooms and have finished story mode!");
          delay(1000);
          Serial.println("Please press the reset button to play again!");
          while (gameOver);
        }
        player.rollUpgrades(); // upgrade player
        player.heal(player.getMaxHP()); // heal player to full hp
        delay(1000);

        combatOver = false;
        enteringRoom = true;
        resetButtons();
        Serial.println("Press the right button to enter the next room.");
        while (!rightButtonPressed); // wait for player input
        resetButtons();
      }
    } else if (endlessMode) { // endless mode
      if (!combatOver) {  // if combat is still active
        if (enteringRoom) { // if first time entering a room
          player.setRemainingRerolls(player.getMaxReroll()); // refresh player rerolls
          delay(1000);
          Serial.print("You enter into room number: ");
          Serial.println(roomNumber);
          // change what base monster is used based on player level, gets stronger as player levels
          if (player.getLevel() <= 3) {
            monster = Monster(2, 4, 0);
          } else if (player.getLevel() <= 7) {
            monster = Monster(2, 6, 0);
          } else {
            monster = Monster(3, 6, 1);
          }
          // generate monster based on level
          monster.rollUpgrades(player.getLevel());
          delay(1000);
          Serial.print("In front of you, you see a monster");
          for (int i = 0; i < 3; i++) {
            delay(1000);
            Serial.print(".");
          }
          Serial.println("and it attacks!");
        }
        if (!enteringRoom) { // after first round of combat
          delay(1000);
          Serial.println("The monster goes for another strike...");
        }
        enteringRoom = false;
        int monsterRoll = monster.rollDice();
        while (!animationTimer.isExpired()); // wait for roll animation
        Serial.print("It rolled a ");
        Serial.print(monsterRoll);
        Serial.println("!");
        delay(1000);
        resetButtons();
        Serial.println("Press the right button to fight back!");
        while (!rightButtonPressed); // wait for player input
        resetButtons();
        int playerRoll = player.rollDice();

        findWinner(playerRoll, monsterRoll); // compare player and monster dice rolls

      } else if (combatOver) {
        resetButtons();
        roomNumber++;
        player.monsterSlain(); // monster is slain
        player.heal(player.getMaxHP()); // heal player to full
        delay(1000);
        combatOver = false;
        enteringRoom = true;
        resetButtons();
        Serial.println("Press the right button to enter the next room.");
        while (!rightButtonPressed); // wait for player input
        resetButtons();
      }
    }
  }
}










void resetButtons() { // resets flags for button presses after checks
  delay(10); // debounce
  leftButtonPressed = false;
  rightButtonPressed = false;
}

void findWinner(int playerRoll, int monsterRoll) { // finds the winner between player and monster
  if (playerRoll >= monsterRoll) { // player rolled higher or equal to monster
    monster.takeDamage(1); // damage monster
    Serial.println("You rolled higher than the monster!");
    delay(1000);
    Serial.println("You attack the monster and it takes 1 point of damage!");
    delay(1000);
    if (monster.getHP() > 0) { // if monster alive
      Serial.print("The monster has ");
      Serial.print(monster.getHP());
      Serial.println(" health remaining.");
    } else if (monster.getHP() == 0) { // if monster dead
      Serial.println("You slayed the monster!");
      combatOver = true;
    }
  } else if (monsterRoll > playerRoll) { // if monster rolled higher than player
    player.takeDamage(1); // damage player
    Serial.println("The monster rolled higher than you!");
    delay(1000);
    Serial.println("The monster attacks you and deals 1 point of damage!");
    delay(1000);
    if (player.getHP() > 0) { // if player alive
      Serial.print("You have ");
      Serial.print(player.getHP());
      Serial.println(" health remaining.");
    } else if (player.getHP() == 0) { // if player dead (player loses)
      gameOver = true;
      Serial.println("You have died to the monster!");
      delay(1000);
      Serial.print("You made it to room ");
      Serial.print(roomNumber);
      Serial.println(" before dying.");
      delay(1000);
      Serial.println("Please press the reset button to play again!");
      while (gameOver);
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
  animationTimer.start(1000, AsyncDelay::MILLIS);

  // Compute a random number from 1 to max roll
  rollNumber = random(1, maxRoll + 1);

  if (!(animationTimer.isExpired())) {
    loop = true;
  }

  if (switchChange) {
    delay(5);
    mute = digitalRead(switchPin);  // set game mode based on switch
    if (mute) Serial.println("Sound muted!");
    else if (!mute) Serial.println("Sound unmuted!");

    switchChange = false;
  }

  // Display status on NeoPixels
  while (loop) {
    int animationRollNumber = random(1, maxRoll + 1);
    // Make some noise and show the dice roll number
    if (!mute) {
      CircuitPlayground.playTone(random(400,2000), 20, false);
    }
    CircuitPlayground.clearPixels();
    for (int p = 0; p < animationRollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixelsD10[animationRollNumber - 1][p], diceColor);
    }

    delay(100);
    if (animationTimer.isExpired()) {
      loop = false;
    }
  }

  if (animationTimer.isExpired()) {
    // Show the dice roll number
    CircuitPlayground.clearPixels();
    for (int p = 0; p < rollNumber; p++) {
      CircuitPlayground.setPixelColor(dicePixelsD10[rollNumber - 1][p], diceColor);
    }
  }
  return rollNumber;
}