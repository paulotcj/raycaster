#include "graphics.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static color_t* colorBuffer = NULL;
static SDL_Texture* colorBufferTexture;

//-------------------------------------------
// SDL stuff - start
bool initializeWindow(void) 
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int fullScreenWidth = display_mode.w;
    int fullScreenHeight = display_mode.h;


    window = SDL_CreateWindow
    (
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        fullScreenWidth,
        fullScreenHeight,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) 
    {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    
    if (!renderer) 
    {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // allocate the total amount of bytes in memory to hold our colorbuffer
    colorBuffer = (color_t*)malloc(sizeof(color_t) * WINDOW_WIDTH * WINDOW_HEIGHT);

    // create an SDL_Texture to display the colorbuffer
    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    return true;
}


void destroyWindow(void) 
{
    free(colorBuffer);
    SDL_DestroyTexture(colorBufferTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

//-------------------------------------------

void clearColorBuffer(color_t color) 
{
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
    {
        colorBuffer[i] = color;
    }
        
}


void renderColorBuffer(void) 
{
    //Update the given texture rectangle with new pixel data
    SDL_UpdateTexture(
        colorBufferTexture, //texture
        NULL,               //sdl rect - represents the are to update
        colorBuffer,        //raw pixel data
        (int)(WINDOW_WIDTH * sizeof(color_t)) //pitch - number of bytes in a row of pixel data
    );

    //Copy a portion of the texture to the current rendering target
    SDL_RenderCopy(
        renderer, 
        colorBufferTexture, 
        NULL, 
        NULL);
    SDL_RenderPresent(renderer);
}

void drawPixel(int x, int y, color_t color) 
{
    colorBuffer[(WINDOW_WIDTH * y) + x] = color;
}

void drawRect(int x, int y, int width, int height, color_t color) 
{
    for (int i = x; i <= (x + width); i++) 
    {
        for (int j = y; j <= (y + height); j++) 
        {
            drawPixel(i, j, color);
        }
    }
}


//test this

void drawLine(int x0, int y0, int x1, int y1, color_t color) 
{
    // e.g.: x0 = 3 , y0 = 4, x1 = 11, y1 = 13

    //(x1 - x0) = (11-3) = 8
    //(y1 - y0) - (13-4) = 9
    int deltaX = (x1 - x0);
    int deltaY = (y1 - y0);

    //in this example longestSideLength = 9
    int longestSideLength = (abs(deltaX) >= abs(deltaY)) ? abs(deltaX) : abs(deltaY);

    //in this example: xIncrement = 8/9 = 0.888888   ,  yIncrement = 9/9 = 1
    float xIncrement = deltaX / (float)longestSideLength;
    float yIncrement = deltaY / (float)longestSideLength;

    float currentX = x0;
    float currentY = y0;

    // risking sound obvious, the longestSideLength is the longest side on either axis, so we
    //  step pixel by pixel. On the longest side we are going to step one full pixel in the direction of
    //  the end of the line and paint it, and the shortest lenght will use the proportional increment
    //  of its own axis (e.g.: 0.1 , 0.534, 0.9333), and then we will paint it using the nearest
    //  pixel coordinate (using round function)
    for (int i = 0; i < longestSideLength; i++) 
    {
        drawPixel(round(currentX), round(currentY), color);
        currentX += xIncrement;
        currentY += yIncrement;
    }
}