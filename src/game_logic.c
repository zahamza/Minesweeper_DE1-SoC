#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define MAX 8 // Maximum size the board can be

#define MAX_ADJACENT_MINES 4


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

/* Forward Declarations*/
// Generates an initial board with randomized mines
void initializeBoard_random(GridSquare board[][MAX], int size, int mineNumber);

// Updates board depending on the user's move
void playMove(GridSquare board[][MAX], int size, int row, int col, Move move);

// If the users plays a safe multiple squares may be uncovered
void safeChain(GridSquare board[][MAX], int size, int row, int col);

void printboard(GridSquare board[][MAX], int size);


int main(int argc, char** argv){
    // board really of type Status, but using int for now
    // in case of name changes
    int size = 5;
    GridSquare board[MAX][MAX];


    initializeBoard_random(board, size, 6);
    printboard(board, size);
    playMove(board, size, 4,4, UNCOVER);
    printboard(board, size);
    return 0;
}


void printboard(GridSquare board[][MAX], int size){
    for(int row = 0; row < size; row++){
        for (int col = 0; col<size; col++){
            if(board[row][col].currentStatus == SAFE_EXPOSED) printf("e");
            else printf("h");
            if(board[row][col].isSafe) printf("s");
            else printf("b");
            printf("%d", board[row][col].minesAdjacent);
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
    return;
}

void initializeBoard_random(GridSquare board[][MAX], int size, int mineNumber){
    int minesPlaced = 0;

    // sets every grid as safe
    for(int row = 0; row<size; row++){
        for(int col = 0; col<size; col++){
            // initailize grid
            GridSquare* square = &board[row][col];
            board[row][col].isSafe = true;
            square -> currentStatus = HIDDEN;
            square -> minesAdjacent = 0;
        }
    }

    // places mines randomly
    while (minesPlaced < mineNumber){
        srand(time(NULL)); // set seed for random

        int randomRow = rand()%size;
        int randomCol = rand()%size;
        GridSquare* square = &board[randomRow][randomCol];

        if(square -> isSafe){

            // checking in all adjacent squares the adjacent
            // mine placments
            bool pastMaxAdjacent = false;

            for(int deltaRow = -1; deltaRow < 2; deltaRow ++){
                for (int deltaCol = -1; deltaCol < 2; deltaCol ++){
                    if (deltaCol == 0 && deltaRow == 0) continue;

                    int currRow = randomRow+deltaRow; int currCol = randomCol+deltaCol;
                    if (!inBounds(size, currRow, currCol)) continue;

                    if(board[currRow][currCol].minesAdjacent > MAX_ADJACENT_MINES){
                        pastMaxAdjacent = true;
                        break;
                    }
                }
                if (pastMaxAdjacent) break;
            }
            // we don't set a mine here if true, breaking while loop
            if (pastMaxAdjacent) break;


            square -> isSafe = false; // sets 
            minesPlaced ++; // incriment mine counter            
            // incriment adjacent square adjacentMines
            for(int deltaRow = -1; deltaRow < 2; deltaRow ++){
                for (int deltaCol = -1; deltaCol < 2; deltaCol ++){
                    if (deltaCol == 0 && deltaRow == 0) continue;

                    int currRow = randomRow+deltaRow; int currCol = randomCol+deltaCol;
                    if (!inBounds(size, currRow, currCol)) continue;

                    board[currRow][currCol].minesAdjacent += 1;
                }
            }

        }
    }
        

}


void playMove(GridSquare board[][MAX], int size, int row, int col, Move move){
    GridSquare* currentSq = &board[row][col];

    Status currState = currentSq->currentStatus;;

    switch (move){

    case UNCOVER: 
        if (currentSq -> isSafe){
            // multiple tiles may need to change
            if(currentSq->minesAdjacent == 0) safeChain(board, size, row, col); 
            else currentSq->currentStatus = SAFE_EXPOSED;
        }
        else{ // mine triggered
            currentSq->currentStatus = MINE_EXPOSED;
        } 
        break;

    case FLAG: 
        // inverses a flagged
        if(currState == FLAGGED){
            currentSq ->currentStatus = HIDDEN;
        } 
        else if (currState == HIDDEN || currState == QUESTIONED){ // making it flagged
            currentSq -> currentStatus = FLAGGED;
        }
        break;

    case QUESTION: 
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
void safeChain(GridSquare board[][MAX], int size, int row, int col){
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

            // printf("[%d,%d]: %d\n", currRow, currCol, currentSq->minesAdjacent);

            // if adjacent is already updated, move to next 
            if(currentSq->currentStatus == SAFE_EXPOSED) continue;

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

