# Minesweeper_DE1-SoC
Final project for ECE243, Computer Organization. Designed to work with the DE1-SoC computer system. <br>
<br>
**Tested using:** [CPUlator Computer System Simulator](https://cpulator.01xz.net/?sys=arm-de1soc)
<br>To run just copy and paste code from main.c into CPUlator's text editor. Then click on the drop down menu and select C. Hit complie and load. Click continue to start running

<br>

![Minesweeper Run](/images/MinesweeperScreenshot.png)


**Project Description:**
We have created a version of Minesweeper to be used in conjunction with the De1-Soc. You lose if you uncover a square and a mine is revealed. If you uncover a square and there is no mine revealed, the square will display the number of mines directly adjacent to it. To win you must uncover all safe squares and flag all bombs
<br>
<br>
**Controls:**
<br>
**Note:** You should/can drag ps2keyboard, HEX display, and the VGA display modules closer to each other on CPUlator

*   A position is ‘selected’ if red square is hovering over it
*   Use PS2-Keyboards arrow keys (🠄🠆🠅🠇) to control where to select
*   To Interact with grids we use the KEYs (or push buttons)
    *   KEY[0] to reveal/uncover a square
    *   KEY[1] to flag a square
    *   KEY[2] to question/mark a square
*   If a square is marked as questioned or flagged it can still be uncovered. They can also be set back to uncovered by pressing their respective KEY

<br>

**Hardware used during game run:**
*   PS2-Keyboard for user inputs that control the game 
*   KEY/Push-buttons for grid interaction 
*   VGA display for actual game interface
*   Private Arm A9 timer is used to count seconds since the game has started
*   HEX Display is used to display time since user started the game (MM:SS)

<br>

**Features we can/will add:**
1. Keyboard inputs to place in conjuction with KEYS/push_buttons

