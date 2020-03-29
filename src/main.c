#include <stdlib.h>
#include <stdbool.h>

#define MAX 8 // Maximum size the board can be
#define WIDTH 6 //Width of boxes that are drawn

//Colours
#define WHITE 0xFFFF
#define BLACK 0x0000

//GLOBALS
volatile int pixel_buffer_start; //Pointer to the current pixel buffer
volatile int* ps2_ptr = (int*) 0xFF200100; //Pointer to PS2

//Globals for game
int currX,currY,prevX,prevY,remX,remY;
int realX,realY,clearX,clearY,byeX,byeY;

// Types of moves a user can input
typedef enum Move{
    FLAG,
    QUESTION,
    UNCOVER
}Move;

// Grids can be one of the following states
typedef enum Status{
    HIDDEN,
    FLAGGED,
    QUESTIONED,
    MINE_EXPOSED,
    SAFE_EXPOSED
} Status;

typedef struct gridSquare{
    Status currentStatus;
    bool isSafe; // if false there is a mine
    int minesAdjacent;
}GridSquare;

// Checks if a given move is in bounds
bool inBounds(int size, int row, int col){return (row >= 0 && col >= 0 && row <size && col <size);}

// Checks how many mines are adjacents at a given position
int getAdjacentMines(GridSquare **board, int size, int row, int col);

/* Forward Declarations*/
// Generates an initial board with randomized mines
void initializeBoard_random(GridSquare** board, int size, int mineNumber);

// Updates board depending on the user's move
void playMove(GridSquare **board, int size, int row, int col, Move move);

// If the users plays a safe multiple squares may be uncovered
void safeChain(GridSquare **board, int size, int row, int col);

//Plots a particular pixel the specified colour
void plotPixel(int x, int y, short int colour);

//Clears the whole screen to black
void clearScreen();

//Swaps the value of two integers
void swap(int* a, int*b);

//Draws a line with specified colour between two points
void drawLine(int x0, int y0, int x1, int y1, short int color);

//Waits to sync the vga buffer and switches to back buffer
void waitForSync();

//Draws a box on the screen at x,y of predefined width and height and colout
void drawBox(int x,int y,short int color);

//Draws the grid lines
void drawGridLines();

//Clears the grid lines
void clearGridLines();


int main(int argc, char** argv){
     /*BELOW IS SETUP FOR THE SCREEN*/
    // board really of type Status, but using int for now
    // in case of name changes
    GridSquare board[MAX][MAX];

    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    waitForSync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clearScreen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clearScreen();


    //SETTING UP POINTERS FOR PS2

    int keyData, keyValid;
    bool run = true;
    bool upPress = false;
    bool downPress = false;
    bool leftPress = false;
    bool rightPress = false;

    //Setting up coordinates 
    currX = 0;
    currY = 0;
    prevX = 0;
    prevY = 0;
    remX = 0;
    remY = 0;

    while(run){
        keyData = *ps2_ptr;
        keyValid = keyData & 0x8000;
        if(keyValid==0x8000){

            //Left arrow key is pressed
            if((keyData & 0xFF) == 0x6B){
                if(!leftPress){
                    remX = prevX;
                    prevX = currX;
                    remY = prevY;
                    prevY = currY;
                    if(currX!=0) currX -= 1;
                    else currX = 7;
                    leftPress = true;
                }
                else{
                    leftPress = false;
                }
            }

            //Right arrow key is pressed
            if((keyData & 0xFF) == 0x74){
                if(!rightPress){
                    remX = prevX;
                    prevX = currX;
                    remY = prevY;
                    prevY = currY;
                    if(currX != 7) currX += 1;
                    else currX = 0;
                    rightPress = true;
                }
                else{
                    rightPress = false;
                }
            }

            //Down arrow is pressed
            if((keyData & 0xFF) == 0x72){
                if(!downPress){
                    remX = prevX;
                    prevX = currX;
                    remY = prevY;
                    prevY = currY;
                    if(currY != 7) currY += 1;
                    else currY = 0;
                    downPress = true;
                }
                else{
                    downPress = false;
                }
            }

            //Up arrow is pressed 
            if((keyData & 0xFF) == 0x75){
                if(!upPress){
                    remX = prevX;
                    prevX = currX;
                    remY = prevY;
                    prevY = currY;
                    if(currY != 0) currY -= 1;
                    else currY = 7;
                    upPress = true;
                }
                else{
                    upPress = false;
                }
            }

        }

        realX = 39 + 15 + currX*30;
        realY = 15 + currY*30;
        clearX = 39 + 15 + remX*30;
        clearY = 15 + remY*30;
        byeX = 39 + 15 + prevX*30;
        byeY = 15 + prevY*30;

        clearGridLines();
        drawBox(clearX,clearY,BLACK);
        drawBox(byeX,byeY,BLACK);
        drawBox(realX,realY,WHITE);
        drawGridLines();
        waitForSync();
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer

    }
    return 0;
}



