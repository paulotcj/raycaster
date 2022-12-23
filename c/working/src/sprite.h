#ifndef SPRITE_H
#define SPRITE_H

#include "constants.h"
#include "graphics.h"
#include "player.h"
#include "texture.h"
typedef struct 
{
    float x;
    float y;
    float distance;
    float angle;
    int texture;
} sprite_t;

void renderSpriteProjection(void);
void renderMapSprites(void);

#endif
