#include <stdio.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include "constants.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1}, // 0 
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 1
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 2
    {1, 0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 0, 1}, // 3
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 4
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 1}, // 5
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1}, // 6
    {1, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1}, // 7
    {1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 2, 0, 0, 1}, // 8
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 9
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 10
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 11
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  // 12 (13 rows)
  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,20 (21 columns)  
};

struct Player
{
    float x;
    float y;
    float width;
    float height;
    int turnDirection; // -1 for left, +1 for right
    int walkDirection; // -1 for back, +1 for front
    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} player;


struct Ray
{
    float rayAngle;
    float wallHitX;
    float wallHitY;
    float distance;
    int wasHitVertical;
    int isRayFacingUp;
    int isRayFacingDown;
    int isRayFacingLeft;
    int isRayFacingRight;
    int wallHitContent;
} rays[NUM_RAYS];

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int isGameRunning = FALSE;
int ticksLastFrame;

//---
Uint32* colorBuffer = NULL;
SDL_Texture* colorBufferTexture;
Uint32* wallTexture = NULL;
//---

//-------------------------------------------

// SDL stuff - start
int initializeWindow() 
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return FALSE;
    }

    window = SDL_CreateWindow
    (
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) 
    {
        fprintf(stderr, "Error creating SDL window.\n");
        return FALSE;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    
    if (!renderer) 
    {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return FALSE;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return TRUE;
}

void destroyWindow() 
{
    free(wallTexture);
    free(colorBuffer);
    SDL_DestroyTexture(colorBufferTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// SDL stuff - END
//-------------------------------------------

void setup() 
{
    //----------------------
    //player setup
    player.x = WINDOW_WIDTH / 2;
    player.y = WINDOW_HEIGHT / 2;
    player.width = 20;
    player.height = 20;
    player.turnDirection = 0; // -1 if left, +1 if right
    player.walkDirection = 0; // -1 if back, +1 if front
    player.rotationAngle = PI;  //start looking left
    player.walkSpeed = 100;
    player.turnSpeed = 45 * (PI / 180); // 45 * 0.01745 = 0.785398163397448
    //----------------------

    //----------------------
    //color and texture
    // allocate the total amount of bytes in memory to hold our colorbuffer
    colorBuffer = (Uint32*)malloc(sizeof(Uint32) * WINDOW_WIDTH * WINDOW_HEIGHT);

    // create an SDL_Texture to display the colorbuffer
    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    // manually create a blue texture with black pixels in every x & y multiples of 8
    wallTexture = (Uint32*)malloc( sizeof(Uint32) * TEXTURE_WIDTH * TEXTURE_HEIGHT );
    
    //create a texture
    for (int x = 0; x < TEXTURE_WIDTH; x++) 
    {
        for (int y = 0; y < TEXTURE_HEIGHT; y++) 
        {
            //0x????????=> 0x(00),(00),(00),(00) => 0x(alpha)(red)(gree)(blue)
            wallTexture[TEXTURE_WIDTH * y + x] = (x % 8 && y % 8) ? 0xFF0000FF : 0xFF000000;
        }
    }    
    //----------------------

}

int mapHasWallAt(float x, float y) 
{
    if (x < 0 || x > WINDOW_WIDTH || 
        y < 0 || y > WINDOW_HEIGHT) 
    {
        return TRUE;
    }
    
    //note: X and Y are given in pixels, we need to translate this to tiles coordinates
    int mapGridIndexX = floor(x / TILE_SIZE);
    int mapGridIndexY = floor(y / TILE_SIZE);
    return map[mapGridIndexY][mapGridIndexX] != 0;
}

void movePlayer(float deltaTime)
{
    player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime; //( +1 or -1 ) * turnSpeed * deltaTime
    float moveStep = player.walkDirection * player.walkSpeed * deltaTime; //( +1 or -1 ) * walkSpeed * deltaTime

    float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
    float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

    //move player if the new coordinate (according with its movement) results in no wall
    // note: may add a 'slide' feature, where player will be able to move in one axis
    if (!mapHasWallAt(newPlayerX, newPlayerY)) {
        player.x = newPlayerX;
        player.y = newPlayerY;
    }
}

void renderPlayer()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect playerRect = 
    {
        player.x * MINIMAP_SCALE_FACTOR,
        player.y * MINIMAP_SCALE_FACTOR,
        player.width * MINIMAP_SCALE_FACTOR,
        player.height * MINIMAP_SCALE_FACTOR
    };
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawLine
    (
        renderer,
        MINIMAP_SCALE_FACTOR * player.x,
        MINIMAP_SCALE_FACTOR * player.y,
        MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 15,
        MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle) * 15
    );
}

float normalizeAngle(float angle)
{
    angle = remainder(angle, TWO_PI);
    if( angle < 0 )
    {
        //2*pi would reset the angle to the start (or end, depending of your preference) position
        // but that might translate into a hiccup
        // by doing (2*pi) + angle  the transition is seamless
        angle = TWO_PI + angle;
    }

    return angle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void castRay(float rayAngle, int stripId)
{
    rayAngle = normalizeAngle(rayAngle);


    //angles: for the purpose of this application the angle 0 will be 2 * PI
    // or 6.283185307179586
    // So UP is everything between: 2*PI or zero, and PI (6.28 or 0, and 3.14), 
    //and by exclusion, down when the flag UP is not set
    //
    // Right: Right is when we have angles between 0deg and 90deg, and, between 270 and 360
    // to put differently, between 0 and 0.5*PI or between 1.5*PI and 2*PI
    int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
    int isRayFacingUp = !isRayFacingDown;

    int isRayFacingRight = rayAngle < 0.5 * PI || rayAngle > 1.5 * PI;
    int isRayFacingLeft = !isRayFacingRight;

    // printf("---------------------------\n");
    // printf("player.rotationAngle: %f \n", player.rotationAngle);
    // printf("isRayFacingDown: %d \n", isRayFacingDown);
    // printf("isRayFacingUp: %d \n", isRayFacingUp);
    // printf("isRayFacingRight: %d \n", isRayFacingRight);
    // printf("isRayFacingLeft: %d \n", isRayFacingLeft);

    float xintercept, yintercept;
    float xstep, ystep;

    ///////////////////////////////////////////
    // HORIZONTAL RAY-GRID INTERSECTION CODE
    ///////////////////////////////////////////    
    int foundHorzWallHit = FALSE;
    float horzWallHitX = 0;
    float horzWallHitY = 0;
    int horzWallContent = 0;

    // Find the y-coordinate of the closest horizontal grid intersection
    //  take the player coordinates and divide it by the tile size and round down the division
    //  to find which tile (row) is directly above the player. Keep in mind the tile (rows) start
    //  from top to bottom, so it's 32,64,96,...
    // Now, in case the player is facing down, the nearest ray intersection is, the same calculation
    //  as above, but we add ONE TILE    
    yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
    yintercept += isRayFacingDown ? TILE_SIZE : 0;

    // Find the x-coordinate of the closest horizontal grid intersection
    //  note: we have the yintercept, which in a triangle this would be the hypotenuse
    //   and we are trying to find the adjacent side, and we have the angle
    //  So the formula: tan(x) = opp / ajd -> adj = opp / tan(x)    
    xintercept = player.x + ( yintercept - player.y ) / tan(rayAngle);

    // Calculate the increment xstep and ystep
    ystep = TILE_SIZE;
    ystep *= isRayFacingUp ? -1 : 1;

    xstep = TILE_SIZE / tan(rayAngle);
    // if the ray is supposed to face left but the xstep is positive, then multiply by -1, so the orientation is correct
    // if the ray is supposed to face right but the xstep is negative, then multiply by -1, so the orientation is correct
    xstep *= (isRayFacingLeft && xstep > 0  ) ? -1 : 1;
    xstep *= (isRayFacingRight && xstep < 0 ) ? -1 : 1;


    float nextHorzTouchX = xintercept;
    float nextHorzTouchY = yintercept;

    // Increment xstep and ystep until we find a wall
    while (
        nextHorzTouchX >= 0 && nextHorzTouchX <= WINDOW_WIDTH && 
        nextHorzTouchY >= 0 && nextHorzTouchY <= WINDOW_HEIGHT
    ) 
    {
        float xToCheck = nextHorzTouchX;
        float yToCheck = nextHorzTouchY + (isRayFacingUp ? -1 : 0); // if ray is facing up, force one pixel up so we are inside a grid cell

        if( mapHasWallAt( xToCheck, yToCheck ) )
        {
            // found a wall hit
            horzWallHitX = nextHorzTouchX;
            horzWallHitY = nextHorzTouchY;
            horzWallContent = map[(int)floor(yToCheck / TILE_SIZE)][(int)floor(xToCheck / TILE_SIZE)];
            foundHorzWallHit = TRUE;
            break;
        }
        else
        {
            nextHorzTouchX += xstep;
            nextHorzTouchY += ystep;
        }
    }

    ///////////////////////////////////////////
    // VERTICAL RAY-GRID INTERSECTION CODE
    ///////////////////////////////////////////
    int foundVertWallHit = FALSE;
    float vertWallHitX = 0;
    float vertWallHitY = 0;
    int vertWallContent = 0;

    // Find the x-coordinate of the closest vertical grid intersection
    xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
    xintercept += isRayFacingRight ? TILE_SIZE : 0;

    // Find the y-coordinate of the closest vertical grid intersection
    yintercept = player.y + (xintercept - player.x) * tan(rayAngle);

    // Calculate the increment xstep and ystep
    xstep = TILE_SIZE;
    xstep *= isRayFacingLeft ? -1 : 1;

    ystep = TILE_SIZE * tan(rayAngle);
    ystep *= (isRayFacingUp && ystep > 0) ? -1 : 1;
    ystep *= (isRayFacingDown && ystep < 0) ? -1 : 1;

    float nextVertTouchX = xintercept;
    float nextVertTouchY = yintercept;

    // Increment xstep and ystep until we find a wall
    while (
        nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH && 
        nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT
        ) 
    {
        float xToCheck = nextVertTouchX + (isRayFacingLeft ? -1 : 0); // if ray is facing left, force one pixel left so we are inside a grid cell
        float yToCheck = nextVertTouchY;
        
        if (mapHasWallAt(xToCheck, yToCheck)) 
        {
            // found a wall hit
            vertWallHitX = nextVertTouchX;
            vertWallHitY = nextVertTouchY;
            vertWallContent = map[(int)floor(yToCheck / TILE_SIZE)][(int)floor(xToCheck / TILE_SIZE)];
            foundVertWallHit = TRUE;
            break;
        } 
        else 
        {
            nextVertTouchX += xstep;
            nextVertTouchY += ystep;
        }
    }   


    // Calculate both horizontal and vertical hit distances and choose the smallest one
    //  if you found a wall hit, calculate the distance
    float horzHitDistance = foundHorzWallHit
        ? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY)
        : FLT_MAX;
    float vertHitDistance = foundVertWallHit
        ? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
        : FLT_MAX;

    // only store the smallest distance
    if (vertHitDistance < horzHitDistance) 
    {
        rays[stripId].distance = vertHitDistance;
        rays[stripId].wallHitX = vertWallHitX;
        rays[stripId].wallHitY = vertWallHitY;
        rays[stripId].wallHitContent = vertWallContent;
        rays[stripId].wasHitVertical = TRUE;
    } 
    else 
    {
        rays[stripId].distance = horzHitDistance;
        rays[stripId].wallHitX = horzWallHitX;
        rays[stripId].wallHitY = horzWallHitY;
        rays[stripId].wallHitContent = horzWallContent;
        rays[stripId].wasHitVertical = FALSE;
    }
    
    rays[stripId].rayAngle = rayAngle;
    rays[stripId].isRayFacingDown = isRayFacingDown;
    rays[stripId].isRayFacingUp = isRayFacingUp;
    rays[stripId].isRayFacingLeft = isRayFacingLeft;
    rays[stripId].isRayFacingRight = isRayFacingRight;     


}

void castAllRays()
{
    // start first ray subtracting half of the FOV
    //  note: we are tying to find the angle of the first ray, so if the player has a field of view of 60deg
    //   we divide it by 2, so it means it's 30deg to the left to where the player is looking plus 30deg
    //   to the right. In this case we start scanning from the left to right, so we subtract the angle
    //   so imagine the player is looking at 45deg, we then subtract 30 from FOV/2 which indicates the
    //   first ray is cast at 15deg
    float rayAngle = player.rotationAngle - (FOV_ANGLE / 2);

    // loop all columns casting the rays
    for(int stripID = 0 ; stripID < NUM_RAYS ; stripID++)
    {
        castRay(rayAngle, stripID);
        rayAngle += FOV_ANGLE / NUM_RAYS;
    }
}

void renderMap()
{
    for(int i = 0; i < MAP_NUM_ROWS; i++)
    {
        for(int j = 0; j < MAP_NUM_COLS; j++)
        {
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;
            int tileColor = map[i][j] != 0 ? 255 : 0;

            
            SDL_Rect mapTileRect = 
            {
                tileX     * MINIMAP_SCALE_FACTOR,
                tileY     * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR
            };



            SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
            SDL_RenderFillRect(renderer , &mapTileRect);

            SDL_SetRenderDrawColor(renderer, 0, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &mapTileRect);


        }
    }
}


void renderRays() 
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < NUM_RAYS; i++) 
    {
        SDL_RenderDrawLine(
            renderer,
            MINIMAP_SCALE_FACTOR * player.x,
            MINIMAP_SCALE_FACTOR * player.y,
            MINIMAP_SCALE_FACTOR * rays[i].wallHitX,
            MINIMAP_SCALE_FACTOR * rays[i].wallHitY
        );
    }
}

void processInput() 
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) 
    {
        case SDL_QUIT: 
        {
            isGameRunning = FALSE;
            break;
        }
        case SDL_KEYDOWN: 
        {
            if (event.key.keysym.sym == SDLK_ESCAPE ) { isGameRunning = FALSE;     }
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

void update() 
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

void generate3DProjection() 
{
    for (int i = 0; i < NUM_RAYS; i++) 
    {
        // get the perpendicular distance to the wall to fix fishbowl distortion
        float perpDistance = rays[i].distance * cos(rays[i].rayAngle - player.rotationAngle);

        // enable this and comment the code above to enable fishbowl view
        //float perpDistance = ray.distance;

        // calculate the distance to the projection plane
        //   note: we are trying to calculate the distance or the adjacent side of a 90deg triangle
        //    we have the angle and we have the opposite size (WINDOW_WIDTH / 2), so the formula is:
        //    adj = opp/tan(x)        
        float distanceProjPlane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);

        // projected wall height
        //  the equality formula is: 'actual wall height'/'distance to wall' = 'projected wall height'/'distance form player to proj. plane'
        //  since we want to find 'projected wall height', we reorganize the equation to:
        //   'projected wall height' = 'actual wall height'/'distance to wall' * 'distance form player to proj. plane'        
        float projectedWallHeight = (TILE_SIZE / perpDistance) * distanceProjPlane;

        int wallStripHeight = (int)projectedWallHeight;

        int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
        wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

        int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
        wallBottomPixel = wallBottomPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomPixel;

        // set the color of the ceiling
        for (int y = 0; y < wallTopPixel; y++)
        {
            colorBuffer[(WINDOW_WIDTH * y) + i] = 0xFF333333;        
        }

        // calculate texture offset X
        int textureOffsetX;
        if (rays[i].wasHitVertical)
        {
            textureOffsetX = (int)rays[i].wallHitY % TEXTURE_HEIGHT;
        }
        else
        {
            textureOffsetX = (int)rays[i].wallHitX % TEXTURE_WIDTH;        
        }

        // render the wall from wallTopPixel to wallBottomPixel
        for (int y = wallTopPixel; y < wallBottomPixel; y++) 
        {
            // calculate texture offset Y
            int distanceFromTop = y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2);
            int textureOffsetY = distanceFromTop * ((float)TEXTURE_HEIGHT / wallStripHeight);

            // set the color of the wall based on the color from the texture
            Uint32 texelColor = wallTexture[(TEXTURE_WIDTH * textureOffsetY) + textureOffsetX];
            colorBuffer[(WINDOW_WIDTH * y) + i] = texelColor;
        }

        // set the color of the floor
        for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++)
        {
            colorBuffer[(WINDOW_WIDTH * y) + i] = 0xFF777777;
        }
    }
}


void clearColorBuffer(Uint32 color) 
{
    for (int x = 0; x < WINDOW_WIDTH; x++)
    {
        for (int y = 0; y < WINDOW_HEIGHT; y++)
        {
            colorBuffer[(WINDOW_WIDTH * y) + x] = color;
        }
    }
}

void renderColorBuffer() 
{
    //Update the given texture rectangle with new pixel data
    SDL_UpdateTexture(
        colorBufferTexture, //texture
        NULL,               //sdl rect - represents the are to update
        colorBuffer,        //raw pixel data
        (int)(WINDOW_WIDTH * sizeof(Uint32)) //pitch - number of bytes in a row of pixel data
    );

    //Copy a portion of the texture to the current rendering target
    SDL_RenderCopy(
        renderer, 
        colorBufferTexture, 
        NULL, 
        NULL);
}

void render() 
{
    //---
    //set clear color and clear frame
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    //---

    generate3DProjection();

    renderColorBuffer();

    // set all color buffer values to black 
    clearColorBuffer(0xFF000000);    

    renderMap();
    renderRays();
    renderPlayer();

    SDL_RenderPresent(renderer);
}

int main() 
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

    destroyWindow();

    return 0;
}
