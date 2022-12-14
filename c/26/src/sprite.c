#include "sprite.h"
#include "utils.h"
#include "ray.h"

#define NUM_SPRITES 7


static sprite_t sprites[NUM_SPRITES] = 
{
    { .x = 640, .y = 630, .texture =  8 }, // barrel 
    { .x = 660, .y = 690, .texture =  8 }, // barrel 
    { .x = 250, .y = 600, .texture = 10 }, // table 
    { .x = 250, .y = 600, .texture = 9  }, // light 
    { .x = 250, .y = 600, .texture = 11 }, // guard
    { .x = 300, .y = 400, .texture = 11 }, // guard
    { .x =  90, .y = 100, .texture = 12 }  // armor
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
        // printf("sprites[i].y: %f, player.y: %f, sprites[i].y - player.y: %f\n",sprites[i].y, player.y, sprites[i].y - player.y);
        // printf("sprites[i].x: %f, player.x: %f, sprites[i].x - player.x: %f\n",sprites[i].x, player.x, sprites[i].x - player.x);
        // printf("player.rotationAngle: %f, atan2(sprites[i].y - player.y, sprites[i].x - player.x): %f\n",player.rotationAngle,atan2(sprites[i].y - player.y, sprites[i].x - player.x));
        // printf("player.rotationAngle - atan2(sprites[i].y - player.y, sprites[i].x - player.x): %f \n",player.rotationAngle - atan2(sprites[i].y - player.y, sprites[i].x - player.x));

        // We need to get the angle between the player and the sprite so we can define if the sprite is
        //  inside the FOV
        //
        //    [x]
        //     ____ ----> opp (x)
        //     \  |
        //  hyp \ | adj (y)
        //       \|
        //        P  -------> get this angle = arctan(opp,adj) = arctan(len_x , len_y )
        //
        //We have 2 sides of a triangle, indicated by
        //  length X and length Y. E.g.: 
        // sprites[i].y: 630.000000, player.y: 472.856110, sprites[i].y - player.y: 157.143890
        // sprites[i].x: 640.000000, player.x: 612.142639, sprites[i].x - player.x: 27.857361
        // player.rotationAngle: 1.376799, atan2(sprites[i].y - player.y, sprites[i].x - player.x): 1.395346
        // player.rotationAngle - atan2(sprites[i].y - player.y, sprites[i].x - player.x) = -0.018547

        float angleSpritePlayer = player.rotationAngle - atan2(sprites[i].y - player.y, sprites[i].x - player.x);
        
        //printf("angleSpritePlayer: %f\n",angleSpritePlayer);

        // Make sure the angle is always between 0 and 180 degrees
        // angleSpritePlayer > 180
        // note: the angle measurement is equivalent to say if things are at your left or right. When the 
        //  angle is more than 180deg you can say the sprite changed from right to left of the player
        if (angleSpritePlayer > PI) 
        {
            angleSpritePlayer -= TWO_PI;
        }
        if (angleSpritePlayer < -PI)
        {
            angleSpritePlayer += TWO_PI;
        }
        angleSpritePlayer = fabs(angleSpritePlayer);

        // If sprite angle is less than half the FOV plus a small margin
        //  we don't want sprited disappearing right in front of our eyes, so sprites stop being rendered
        //  only when its edges are all out of sight
        const float EPSILON = 0.2;
		if (angleSpritePlayer < (FOV_ANGLE / 2) + EPSILON) 
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


    //-------
    // Sort sprites by distance using a naive bubble-sort algorithm
    for (int i = 0; i < numVisibleSprites - 1; i++) 
    {
        for (int j = i + 1; j < numVisibleSprites; j++) 
        {
            //  closer                        farther
            if (visibleSprites[i].distance < visibleSprites[j].distance) 
            {
                sprite_t temp = visibleSprites[i];
                //swap places, lower indexes will have objects farther away, and they will
                //  be drawn first
                visibleSprites[i] = visibleSprites[j];
                visibleSprites[j] = temp;
            }
        }
    }  
    //-------  
 
    //-------
    // Rendering all the visible sprites
    for (int i = 0; i < numVisibleSprites; i++) 
    {
        sprite_t sprite = visibleSprites[i];

        // Calculate the perpendicular distance of the sprite to prevent fish-eye effect
        float perpDistance = sprite.distance * cos(sprite.angle);

        // Calculate the sprite projected height and width (the same, as sprites are squared)
        float spriteHeight = (TILE_SIZE / perpDistance) * DIST_PROJ_PLANE;
        float spriteWidth = spriteHeight;

        // Sprite top Y
        float spriteTopY = (WINDOW_HEIGHT / 2) - (spriteHeight / 2);
        spriteTopY = (spriteTopY < 0) ? 0 : spriteTopY;

        // Sprite bottom Y
        float spriteBottomY = (WINDOW_HEIGHT / 2) + (spriteHeight / 2);
        spriteBottomY = (spriteBottomY > WINDOW_HEIGHT) ? WINDOW_HEIGHT : spriteBottomY;

        
        //-----

        // Calculate the sprite X position in the projection plane
        float spriteAngle = atan2(sprite.y - player.y, sprite.x - player.x) - player.rotationAngle;
        //printf("spriteAngle: %f\n",spriteAngle);
        
        float spriteScreenPosX = tan(spriteAngle) * DIST_PROJ_PLANE;
        //printf("tan(spriteAngle): %f, DIST_PROJ_PLANE: %f, spriteScreenPosX: %f\n",tan(spriteAngle),DIST_PROJ_PLANE,spriteScreenPosX);
        //printf("spriteWidth: %f\n",spriteWidth);
        // SpriteLeftX
        // the formula can be translated as: 
        //  (middle of the screen) + (X axis offset position relative to the middle of the screen) 
        //     - (offset center of sprite)
        // so suppose we have: WINDOW_WIDTH = 1280px , spriteScreenPosX: 111px, spriteWidth = 500px
        //  (WINDOW_WIDTH / 2) + spriteScreenPosX - (spriteWidth / 2) = 
        //  (1280 / 2) + 111 - (500 / 2) = (640) + 111 - (250) =>
        //  (640 -> center of the screen) + (sprite is 111px to the right) => this is the center of the sprite
        //  but the sprite is 500px wide, and should start being rendered 250px to the left, so we subtract
        //  250px from the center position of the sprite
        float spriteLeftX = (WINDOW_WIDTH / 2) + spriteScreenPosX - (spriteWidth / 2);

        // SpriteRightX
        float spriteRightX = spriteLeftX + spriteWidth;

        // Query the width and height of the texture
        int textureWidth = upng_get_width(textures[sprite.texture]);
        int textureHeight = upng_get_height(textures[sprite.texture]);

        // Loop all the x values
        for (int x = spriteLeftX; x < spriteRightX; x++) 
        {
            //strech or compress sprite's pixel width
            float texelWidth = (textureWidth / spriteWidth);


            int textureOffsetX = (x - spriteLeftX) * texelWidth;

            // Loop all the y values
            for (int y = spriteTopY; y < spriteBottomY; y++) 
            {
                if (x > 0 && x < WINDOW_WIDTH && 
                    y > 0 && y < WINDOW_HEIGHT) 
                {
                    // e.g.: WINDOW_HEIGHT = 800, spriteHeight = 384.14, y = 207
                    // (WINDOW_HEIGHT / 2) = 400 , (spriteHeight / 2) = 192.07
                    // y + (spriteHeight / 2) - (WINDOW_HEIGHT / 2) = 207 + (192.07) - 400
                    // => 207 - 207 = 0  -> as we can see -207 is a constant for this sprite
                    // and as y increases we advance in the y axis
                    int distanceFromTop = y + (spriteHeight / 2) - (WINDOW_HEIGHT / 2);
                    // printf("spriteHeight: %f\n",spriteHeight);
                    // printf("y:%d , (spriteHeight / 2): %f, (WINDOW_HEIGHT / 2): %d\n",y,(spriteHeight / 2), (WINDOW_HEIGHT / 2));
                    // printf("distanceFromTop:%d\n",distanceFromTop);
                    // getchar();
                    
                    // distanceFromTop * (textureHeight / spriteHeight) = 0 * (64 / 381.70)
                    // 0 * 0.1676, 1 * 0.1676, ... , n * 0.1676
                    // printf("distanceFromTop: %d, textureHeight: %d, spriteHeight: %f  \n",distanceFromTop,textureHeight,spriteHeight);
                    int textureOffsetY = distanceFromTop * (textureHeight / spriteHeight);
                    // printf("textureOffsetY: %d\n",textureOffsetY);
                    // getchar();


                    color_t* spriteTextureBuffer = (color_t*) upng_get_buffer(textures[sprite.texture]);
                    color_t texelColor = spriteTextureBuffer[(textureWidth * textureOffsetY) + textureOffsetX];
                
                    //don't draw the transparency colour (0xFFFF00FF) - 0xAABBGGRR
                    if (sprite.distance < rays[x].distance && texelColor != 0xFFFF00FF)
                    {
                        drawPixel(x, y, texelColor);
                    }
                }
                
            }
        }
    } // Rendering all the visible sprites - end
    //-------
}
