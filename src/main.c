#include <stdlib.h>
#include <stdbool.h>

#define MAX 26 // Maximum size the board can be
#define WIDTH 4 //Width of boxes that are drawn

//Colours
#define WHITE 0xFFFF
#define BLACK 0x0000

//GLOBALS
volatile int pixel_buffer_start; //Pointer to the current pixel buffer

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


int main(int argc, char** argv){
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
    drawGridLines();
    waitForSync();
    return 0;
}



