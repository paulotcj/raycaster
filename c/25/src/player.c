#include "player.h"
#include "utils.h"

player_t player = {
    .x = WINDOW_WIDTH / 2,
    .y = WINDOW_HEIGHT / 2,
    .width = 20,
    .height = 20,
    .turnDirection = 0, // -1 if left, +1 if right
    .walkDirection = 0, // -1 if back, +1 if front
    .rotationAngle = PI, //start looking left
    .walkSpeed = 100,
    .turnSpeed = 45 * (PI / 180) // 45 * 0.01745 = 0.785398163397448
};

void movePlayer(float deltaTime)
{
    player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime; //( +1 or -1 ) * turnSpeed * deltaTime
    normalizeAngle(&player.rotationAngle);

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


void renderMapPlayer(void) 
{
    // SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    // //note -10 is the offset the player position to the center of the square
    // // given that the player.width and height is typically 20
    // SDL_Rect playerRect = 
    // {
    //     (player.x-10) * MINIMAP_SCALE_FACTOR,
    //     (player.y-10) * MINIMAP_SCALE_FACTOR,
    //     player.width * MINIMAP_SCALE_FACTOR,
    //     player.height * MINIMAP_SCALE_FACTOR
    // };
    // SDL_RenderFillRect(renderer, &playerRect);

    // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    // SDL_RenderDrawLine
    // (
    //     renderer,
    //     MINIMAP_SCALE_FACTOR * player.x,
    //     MINIMAP_SCALE_FACTOR * player.y,
    //     MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 15,
    //     MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle) * 15
    // );


    drawRect(    
        (player.x-10) * MINIMAP_SCALE_FACTOR,
        (player.y-10) * MINIMAP_SCALE_FACTOR,
        player.width * MINIMAP_SCALE_FACTOR,
        player.height * MINIMAP_SCALE_FACTOR,
        0xFFFFFFFF
    );

    drawLine(
        MINIMAP_SCALE_FACTOR * player.x,
        MINIMAP_SCALE_FACTOR * player.y,
        MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 15,
        MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle) * 15,
        0xFF00FF00
    );


}

