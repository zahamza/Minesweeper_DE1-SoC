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


int getAdjacentMines(GridSquare **board, int size, int row, int col){
    int mineNumber = 0;

    for(int deltaRow = -1; deltaRow < 2; deltaRow ++){
        for (int deltaCol = -1; deltaCol < 2; deltaCol ++){
            if (deltaCol == 0 && deltaRow == 0) continue;

            int currRow = row+deltaRow; int currCol = col+deltaCol;
            if (!inBounds(size, currRow, currCol)) continue;

            if(!(board[currRow][currCol].isSafe)) mineNumber++;
        }
    }
    return mineNumber;
}


void initializeBoard_random(GridSquare** board, int size, int mineNumber){
    int minesPlaced = 0;

    // sets every grid as safe
    for(int row = 0; row<size; row++){
        for(int col = 0; col<size; col++){
            GridSquare* square = &board[row][col];
            square -> isSafe = true;
            square -> currentStatus = HIDDEN;
        }
    }

    // places mines randomly
    while (minesPlaced < mineNumber){
        int randomRow = rand()%size;
        int randomCol = rand()%size;
        GridSquare* square = &board[randomRow][randomCol];

        if(square -> isSafe){
            square -> isSafe = false; // sets 
            minesPlaced ++;
        }
    }

    // gets number of mines adjacent
    for(int row = 0; row<size; row++){
        for(int col = 0; col<size; col++){
            board[row][col].minesAdjacent = getAdjacentMines(board, size, row, col);
        }
    }

}


void playMove(GridSquare ** board, int size, int row, int col, Move move){
    GridSquare* currentSq = &board[row][col];

    switch (move){

    case UNCOVER:
        if (currentSq -> isSafe){
            // multiple tiles may need to change
            safeChain(board, size, row, col); 
        }
        else{ // mine triggered
            currentSq->currentStatus = MINE_EXPOSED;
        } 
        break;

    case FLAG:
        Status currState = currentSq->currentStatus;
        // inverses a flagged
        if(currState == FLAGGED){
            currentSq ->currentStatus = HIDDEN;
        } 
        else if (currState == HIDDEN || currState == QUESTIONED){ // making it flagged
            currentSq -> currentStatus = FLAGGED;
        }
        break;

    case QUESTION:
        Status currState = currentSq->currentStatus;
        if (currState == HIDDEN || currState == FLAGGED){
            currentSq -> currentStatus = QUESTIONED;
        }
        else if(currState == QUESTIONED){
            currentSq -> currentStatus = HIDDEN;
        }
        break;

    default:
        break;
    }
    

}

// Recursive function that exposes selected square and adjacent squares
// and so on and so forth
void safeChain(GridSquare **board, int size, int row, int col){
    /* Expose appropriate squares */
    GridSquare* currentSq = &board[row][col];

    currentSq->currentStatus = SAFE_EXPOSED; // update selected

    // iterate through the 8 squares adjacent
    for(int deltaRow = -1; deltaRow < 2; deltaRow ++){
        for (int deltaCol = -1; deltaCol < 2; deltaCol ++){
            if (deltaCol == 0 && deltaRow == 0) continue;

            int currRow = row+deltaRow; int currCol = col+deltaCol;

            if (!inBounds(size, currRow, currCol)) continue;

            currentSq = &board[currRow][currCol];

            // if adjacent is already updated, move to next 
            if(currentSq->currentStatus = SAFE_EXPOSED) continue;

            if(currentSq->isSafe){
                if(currentSq->minesAdjacent == 0){
                    // Chain if there are 0 adjacent mines on the
                    // adjacent square (call flip square itself as well)
                    safeChain(board, size, currRow, currCol);
                }else{
                    currentSq->currentStatus = SAFE_EXPOSED; // update adjacent square
                }
            }

        }
    }
    return;
}


/*Drawing Functions*/
void plotPixel(int x, int y, short int colour){
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = colour;
}

//Iterate through every pixel and set the colour to black
void clearScreen(){
    for(int i = 0; i<320; i++){

        for(int j=0; j<240; j++){

            plotPixel(i,j,0x0000);
        }
    }
}

void swap(int* a, int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void drawLine(int x0, int y0, int x1, int y1, short int color){
    bool steep = abs(y1-y0) > abs(x1-x0);
    if(steep){
        swap(&x0 , &y0);
        swap(&x1,&y1);
    }
    if(x0>x1){
        swap(&x0,&x1);
        swap(&y0,&y1);
    }

    int delx = x1 - x0;
    int dely = abs(y1-y0);
    int error = -delx/2;
    int y = y0;
    int ystep;

    if(y0<y1){
       ystep = 1; 
    }
    else{
        ystep = -1;
    }

    for(int x = x0; x <= x1; x++){
        if(steep){
            plotPixel(y,x,color);
        }
        else{
            plotPixel(x,y,color);
        }
        error = error + dely;
        if(error >= 0){
            y += ystep;
            error -= delx;
        }
    }
}

void waitForSync(){
    volatile int *pixel_ctrl_ptr = 0xFF203020;
    register int status;

    *pixel_ctrl_ptr = 1;

    status = *(pixel_ctrl_ptr+3);
    while((status & 0x01) != 0){
        status = *(pixel_ctrl_ptr+3);
    }
}

void drawBox(int x,int y,short int color){
    for(int a = x-WIDTH; a<= x+WIDTH; a++){
        for(int b = y-WIDTH; b <= y+WIDTH; b++){
            plotPixel(a,b,color);
        }
    }
}

void drawGridLines(){
    for(int i = 39; i<=279; i+=30){
        drawLine(i,0,i,239,WHITE);
    }
    for(int j = 0; j<239; j+=30){
         drawLine(39,j,279,j,WHITE);
    }
}
