#include <stdlib.h>
#include <stdbool.h>

volatile int pixel_buffer_start; // global variable

// forward declarations
void clear_screen(void);
void draw_line(int x1, int y1, int x2, int y2, short int color);
void swap(int* a, int* b);
void wait_for_vsync();

void plot_pixel(int x, int y, short int line_color);
void draw_screen(short* cBox, int* xBox, int* yBox,
                int size, int dimension);

short getRandomColor(short* colorBox);
void draw();
void draw_box(int x, int y, short int color, int dimension);
void update_boxes(int* xBox, int* yBox, int* dx_Box, int* dy_Box, int size, int dimension);

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer

    // initialize location and direction of rectangles
    int n = 8; // number of boxes
    int dimension = 8; // box pixel size
    short int colour[4] = {0xFFFF,0xF800,0x07E0,0x001F};
    short colorBox[n];
    int dx_Box[n]; int dy_Box[n];
    int xBox[n]; int yBox[n];

    // randomize deltas
    for(int i = 0; i<n; i++){
        dx_Box[i] = (rand()%2)*2 -1;
        dy_Box[i] = (rand()%2)*2 -1;
        colorBox[i] = getRandomColor(colour);
        xBox[i] = rand()%(320-2*dimension) + dimension; // XMax and yMax
        yBox[i] = rand()%(240-2*dimension) + dimension;
    }

    while (true)
    {
        /* Erase any boxes and lines that were drawn in the last iteration */
        clear_screen();

        // drawing the boxes and lines 
        draw_screen(colorBox, xBox, yBox, n, dimension);
        // code for updating the locations of boxes 
        update_boxes(xBox, yBox, dx_Box, dy_Box, n, dimension);

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

// code for subroutines
void update_boxes(int* xBox, int* yBox, int* dx_Box, int* dy_Box, int size, int dimension){
    int shift = dimension/2;
    int xMax = 319; int yMax = 239;
    for(int i = 0; i < size; i++){
        if(xBox[i] >= (xMax - shift) || xBox[i] <= dimension){
            dx_Box[i] = -1*dx_Box[i]; // swap direction
        }
        if(yBox[i] >= (yMax - shift) || yBox[i] <= shift){
            dy_Box[i] = -1*dy_Box[i]; // swap direction
        }

        xBox[i] += dx_Box[i];
        yBox[i] += dy_Box[i];
    }
}

void draw_screen(short* cBox, int* xBox, int* yBox,
                int size, int dimension){
    short white = 0xFFFF;
    for(int i = 0; i < size; i++){
        //drawingBox
        draw_box(xBox[i],yBox[i], cBox[i], dimension);

        // drawing connection lines
        if(i == size - 1){
            draw_line(xBox[i], yBox[i], xBox[0], yBox[0], white);
        }else{
            draw_line(xBox[i], yBox[i], xBox[i+1], yBox[i+1], white);
        }
    }
}

short getRandomColor(short* color){
    int numberOfColors = 4;
    return color[rand()%numberOfColors];
}

void draw_box(int x, int y, short int color, int dimension){
    int shift = dimension/2;
    for(int i = x - shift; i <=x+shift; i++){
        for (int j = y - shift; j<= y+shift; j++){
            plot_pixel(i,j, color);
        }
    }
}


void wait_for_vsync(){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    register int status;

    *pixel_ctrl_ptr = 1; // start synchronization process
                            // sends a request
    status = *(pixel_ctrl_ptr+3);
    
    // wait until status bit reaches 1
    // meaning sync is complete
    while((status & 0x1) != 0){
        status = *(pixel_ctrl_ptr+3);
    }

}


void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// plots a black pixel on every pixel of the VGA display
void clear_screen(void){
    int pixelsWidth = 320; int pixelsHeight = 240;
    
    short int black = 0x0000;

    for(int x = 0; x < pixelsWidth; x++){
        for(int y = 0; y < pixelsHeight; y++){
            plot_pixel(x, y, black);
        }
    }

}

void draw_line(int x1, int y1, int x2, int y2, short int color){
    bool isSteep = abs(y2-y1) > abs(x2-x1);

    if(isSteep){
        swap(&x1, &y1);
        swap(&x2, &y2);
    }
    if(x1 > x2){
        swap(&x1, &x2);
        swap(&y2, &y1);
    }

    int deltaX = x2 - x1;
    int deltaY = abs(y2 - y1);

    int y = y1;

    int yStep;

    if(y1 < y2) yStep = 1;
    else yStep = -1;

    int error = -(deltaX / 2);

    for(int x = x1; x < x2; x++){
        if(isSteep) plot_pixel(y, x, color);
        else plot_pixel(x, y, color);

        error += deltaY;
        if (error >= 0){
            y += yStep;
            error -= deltaX;
        }
    }

}

void swap(int*a, int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}
