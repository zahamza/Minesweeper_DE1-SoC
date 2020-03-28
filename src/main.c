#include <stdlib.h>
#include <stdbool.h>

#define MAX 26 // Maximum size the board can be


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
    MARKED,
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


int main(int argc, char** argv){
    // board really of type Status, but using int for now
    // in case of name changes
    GridSquare board[MAX][MAX];
    
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

    switch (move)
    {
    case UNCOVER:
        if (currentSq -> isSafe){
            safeChain(board, size, row, col); 
        }
        else
        {
            currentSq->currentStatus = MINE_EXPOSED;            
        }
        
        break;
    default:
        break;
    }
    

}


void safeChain(GridSquare **board, int size, int row, int col){
    /* Expose appropriate squares */
    return;
}