/*Drawing Functions*/
#include <stdlib.h>
#include <stdbool.h>

#define MAX 26 // Maximum size the board can be
#define WIDTH 4 //Width of boxes that are drawn

//Colours
#define WHITE 0xFFFF
#define BLACK 0x0000

//GLOBALS
volatile int pixel_buffer_start; //Pointer to the current pixel buffer



// int main(int argc, char** argv){

//     volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
//     // declare other variables(not shown)
//     // initialize location and direction of rectangles(not shown)

//     /* set front pixel buffer to start of FPGA On-chip memory */
//     *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
//                                         // back buffer
//     /* now, swap the front/back buffers, to set the front buffer location */
//     waitForSync();
//     /* initialize a pointer to the pixel buffer, used by drawing functions */
//     pixel_buffer_start = *pixel_ctrl_ptr;
//     clearScreen(); // pixel_buffer_start points to the pixel buffer
//     /* set back pixel buffer to start of SDRAM memory */
//     *(pixel_ctrl_ptr + 1) = 0xC0000000;
//     pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
//     clearScreen();
//     drawGridLines();
//     waitForSync();
//     return 0;
// }


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

void clearGridLines(){
    for(int i = 39; i<=279; i+=30){
        drawLine(i,0,i,239,BLACK);
    }
    for(int j = 0; j<239; j+=30){
         drawLine(39,j,279,j,BLACK);
    }
}

void drawGridBox(int xpos, int ypos, char type){
    //short int colour;
    int* arr = NULL;
    //int currentX,currentY,k;
    int startX = 40 + 30*xpos;
    int startY = 1 + 30*ypos;
    if(type=='0'){
        arr = zeros;
    }
    if(type == '1'){
        arr = ones;
    }
    if(type == '2'){
       arr = twos;
    }
    if(type=='3'){
        arr = threes;
    }
    if(type=='4'){
        arr = fours;
    }
    if(type=='q'){
        arr = questions;
    }
    if(arr!=NULL){
        for(int j=0; j<28; j++){
            for(int i=0; i<28; i++){
                int currentX = startX + i;
                int currentY = startY + j;
                int k = j*28 + i;
                short int colour = arr[k];
                plotPixel(currentX,currentY,colour);
            
            }
        }
    }
}

void convertArray(int* data, int* final){
    for(int i=0; i<3136; i++){
        if(i%2==0){
            final[i/2] = (data[i] << 8) + (data[i+1]);
        }
    }
}

void clearGridBox(int xpos, int ypos){
    int currentX,currentY;
    int startX = 40 + 30*xpos;
    int startY = 1 + 30*ypos;

    for(int j=0; j<28; j++){
        for(int i=0; i<28; i++){
            currentX = startX + i;
            currentY = startY + j;
            plotPixel(currentX,currentY,BLACK);
            
        }
    }

}