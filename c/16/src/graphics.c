#include "graphics.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* colorBuffer = NULL;
SDL_Texture* colorBufferTexture;

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
    colorBuffer = (uint32_t*)malloc(sizeof(uint32_t) * WINDOW_WIDTH * WINDOW_HEIGHT);

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

void clearColorBuffer(uint32_t color) 
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
        (int)(WINDOW_WIDTH * sizeof(uint32_t)) //pitch - number of bytes in a row of pixel data
    );

    //Copy a portion of the texture to the current rendering target
    SDL_RenderCopy(
        renderer, 
        colorBufferTexture, 
        NULL, 
        NULL);
    SDL_RenderPresent(renderer);
}

void drawPixel(int x, int y, uint32_t color) 
{
    colorBuffer[(WINDOW_WIDTH * y) + x] = color;
}