#include "sprite.h"
#include "utils.h"

#define NUM_SPRITES 1

static sprite_t sprites[NUM_SPRITES] = 
{
    { .x = 640, .y = 630, .texture = 9 }, // barrel 
};

void renderMapSprites(void) 
{
    for (int i = 0; i < NUM_SPRITES; i++) 
    {
        drawRect(
            sprites[i].x * MINIMAP_SCALE_FACTOR,
            sprites[i].y * MINIMAP_SCALE_FACTOR,
            2,
            2,
            (sprites[i].visible) ? 0xFF00FFFF : 0xFF444444
        );
    }
}

void renderSpriteProjection(void) 
{
    sprite_t visibleSprites[NUM_SPRITES];
    int numVisibleSprites = 0;

    // Find sprites that are visible (inside the FOV)
    for (int i = 0; i < NUM_SPRITES; i++) 
    {
        //printf("sprites[i].y: %f, player.y: %f, sprites[i].y - player.y: %f\n",sprites[i].y, player.y, sprites[i].y - player.y);
        //printf("sprites[i].x: %f, player.x: %f, sprites[i].x - player.x: %f\n",sprites[i].x, player.x, sprites[i].x - player.x);
        float angleSpritePlayer = player.rotationAngle - atan2(sprites[i].y - player.y, sprites[i].x - player.x);
        
        // Make sure the angle is always between 0 and 180 degrees
        // angleSpritePlayer > 180
        if (angleSpritePlayer > PI) 
        {
            angleSpritePlayer -= TWO_PI;
        }
        if (angleSpritePlayer < -PI)
        {
            angleSpritePlayer += TWO_PI;
        }
        angleSpritePlayer = fabs(angleSpritePlayer);

        // If sprite angle is less than half the FOV plus a small error margin
		if (angleSpritePlayer < (FOV_ANGLE / 2)) 
        {
            sprites[i].visible = true;
            sprites[i].angle = angleSpritePlayer;
            sprites[i].distance = distanceBetweenPoints(sprites[i].x, sprites[i].y, player.x, player.y);
			visibleSprites[numVisibleSprites] = sprites[i];
            numVisibleSprites++;
        } 
        else 
        {
            sprites[i].visible = false;
        }
    }
 
    // Rendering all the visible sprites
    for (int i = 0; i < numVisibleSprites; i++) 
    {
        // TODO: draw the pixels of the sprite in the correct position in the screen
    }
}
