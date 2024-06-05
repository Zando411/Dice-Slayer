# Dice-Slayer
Dice Slayer is a simple game about slaying monsters through dice rolls. As you progress through rooms, you get stronger, but so do your opponents. Choose between a story mode with a set amount of rooms, or endless mode with rooms as long as you can survive. How far can you go?

How to play: 
On program start, the player will be given the choice between story mode and endless mode.

## Story mode: 
- Progress through 5 rooms that increase in difficulty. 
- Every room contains a monster to fight
- Players will roll dice against the monster, whichever rolls higher wins. The player wins ties.
- Whoever rolls lower loses one health, and whoever reaches 0 first loses!
- If the player loses, their run is over and they must restart the game. If the monster loses, the player progresses to the next room
- As players progress they will gain unique upgrades that are randomized, making every run different. 
- Game ends either when the player defeats all story mode rooms, or when the player dies.

## Endless Mode:
- Same rules as story mode, but endless! Score is counted based on how many rooms with randomized monsters the player beats
- Game ends when player dies
- Scaling leveling for when player gets upgrades (slay different amounts of monsters the higher in level you go)
- Monsters will be same level as player with their own randomized upgrades, with an added chance of one of their upgrade slots not being filled
- Max level of 10, encourages players to try different ability upgrade combinations and not go on forever without good decisions or good builds

## Upgrades:
- +1 to roll (stacking, 1 becomes 2 etc.)
- +2 to max roll (rolls d8 instead of d6, stacks to d10) 
- +1 max HP (stacking)
- +1 reroll (can reroll the die once for every stack, adds to your previous roll. )

## Inputs:

### Right Button (D5):
Controls the overall game flow. This input is used to progress the game forward and initiate rolls. The player will be prompted on when to press this button.

### Left Button (D4):
This button is the secondary decision button. This button is used to reroll the die if the player has the reroll upgrade, and to select the left upgrade. The player will be prompted on when to press this button.

### Switch (D7): 
The switch is used to initalize which mode the player is playing, either endless or story mode. During gameplay, this switch is also used to mute the sound.

### Light Sensor(A8):
This sensor is used as a part of the random seed, which ensures the rolls and gameplay are random every game.

## Outputs: 

### Neopixels:
The neopixels are used to show both the player and monster dice rolls. They will be white for the player and red for the monster, and the amount that light up will correspond with the dice roll.

### Speaker: 
The speaker plays a tone while the dice is rolling, which adds to the user experience.

### Serial Monitor:
The text of the gameplay is printed through the serial monitor, and displays the important information of the game. It prints the dice rolls, player and monster HP, player stats, and what actions the player needs to take, the upgrades available, and much more!.

