#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "constants.h"
//#include "graphics.h"
//#include "map.h"
//#include "player.h"
//#include "ray.h"
#include "wall.h"
//#include "texture.h"

bool isGameRunning = false;
int ticksLastFrame;

void setup(void) 
{
    //---------------------
    // load some textures from the textures.h
    loadWallTextures();
    //----------------------
}



void processInput(void) 
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) 
    {
        case SDL_QUIT: 
        {
            isGameRunning = false;
            break;
        }
        case SDL_KEYDOWN: 
        {
            if (event.key.keysym.sym == SDLK_ESCAPE ) { isGameRunning = false;     }
            if (event.key.keysym.sym == SDLK_UP     ) { player.walkDirection = +1; }
            if (event.key.keysym.sym == SDLK_DOWN   ) { player.walkDirection = -1; }
            if (event.key.keysym.sym == SDLK_RIGHT  ) { player.turnDirection = +1; }
            if (event.key.keysym.sym == SDLK_LEFT   ) { player.turnDirection = -1; }
            break;
        }
        case SDL_KEYUP: 
        {
            if (event.key.keysym.sym == SDLK_UP    ) { player.walkDirection = 0; }
            if (event.key.keysym.sym == SDLK_DOWN  ) { player.walkDirection = 0; }
            if (event.key.keysym.sym == SDLK_RIGHT ) { player.turnDirection = 0; }
            if (event.key.keysym.sym == SDLK_LEFT  ) { player.turnDirection = 0; }
            break;
        }        
    }
}

void update(void) 
{
    // Compute how long we have until the reach the target frame time in milliseconds
    // while( !SDL_TICKS_PASSED(SDL_GetTicks(), ticksLastFrame + FRAME_TIME_LENGTH)    );


    // // Compute how long we have until the reach the target frame time in milliseconds
    int timeToWait = FRAME_TIME_LENGTH - ( SDL_GetTicks() - ticksLastFrame );

    // Only delay execution if we are running too fast
    if( timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH)
    { 
        SDL_Delay(timeToWait);
    }




    // 'current tick' - 'ticks last frame' = delta
    float deltaTime = ( SDL_GetTicks() - ticksLastFrame ) / 1000.0f;

    ticksLastFrame = SDL_GetTicks();

    movePlayer(deltaTime);
    castAllRays();
}


void render(void) 
{
    // set all color buffer values to black 
    clearColorBuffer(0xFF000000);    
    //---

    renderWallProjection();

    //---
    // minimap
    renderMap();
    renderRays();
    renderPlayer();
    //---

    

    renderColorBuffer();
}

void releaseResources(void) {
    freeWallTextures();
    destroyWindow();
}

int main(int argc, char* argv[]) 
{
    isGameRunning = initializeWindow();

    setup();

    //game loop - process input, update, render
    while (isGameRunning) 
    {
        processInput();
        update();
        render();
    }

    releaseResources();

    return EXIT_SUCCESS;
}
