#include "wall.h"

// Change the color intensity based on a factor value between 0 and 1
void changeColorIntensity(color_t* color, float factor) 
{
    // The code below is just a mask the will extract the information regarding each component
    //  of the colour. The color is in the format 0xAARRGGBB.
    // For instance take the following colour: 0x55000000 -> alpha = 0x55 r=0x00 g=0x00 b=0x00 ->
    //  it's black with alpha component of 85. If we pass this number through a bitwise AND operator
    //  we will get: 0x55000000 & 0xFF000000 = 0x55000000, but if we had the random number 0x55112233
    //  and use the AND bitwise operator we would get: 0x55112233 & 0xFF000000 = 0x55000000
    color_t a = (*color & 0xFF000000);
    color_t r = (*color & 0x00FF0000) * factor;
    color_t g = (*color & 0x0000FF00) * factor;
    color_t b = (*color & 0x000000FF) * factor;

    // e.g.: 0x0f = 15, 0xf0 = 240 and 0x0f | 0xf0 = 255 which is 0xff ; and the same principle
    // applies below
    *color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}




void renderWallProjection(void) 
{
    for (int x = 0; x < NUM_RAYS; x++) 
    {
        // Calculate the perpendicular distance to avoid the fish-eye distortion
        // get the perpendicular distance to the wall to fix fishbowl distortion
        //  Notes: the first thing here is to get the proper dimensions and angles of the triangle
        //   since we want to adjust the ray to its correct size, being the correct size the distance
        //   from player to the wall, we need to find the angle of the ray with the origin in the
        //   player angle, and this is: rays[i].rayAngle - player.rotationAngle
        //
        //   Now, from the triangle, we have the player distance (ADJ), ray angle (theta/X), and ray distance
        //   also know as distorted distance (HYP).
        //   Considering the formula: cos(x) = ADJ / HYP  ,  and considering we want to adjust HYP, we
        //   can rearranje the formula as: ADJ = HYP * COS(x) => 'correct dist' = 'distorted dist' * COS(x)
        float perpDistance = rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle);

        // enable this and comment the code above to enable fishbowl view
        // float perpDistance = rays[i].distance;

        // calculate the distance to the projection plane
        //   note: we are trying to calculate the distance or the adjacent side of a 90deg triangle
        //    we have the angle and we have the opposite size (WINDOW_WIDTH / 2), so the formula is:
        //    adj = opp/tan(x)        
        //float distanceProjPlane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);

        // Calculate the projected wall height
        // projected wall height
        //  the equality formula is: 'actual wall height'/'distance to wall' = 'projected wall height'/'distance form player to proj. plane'
        //  since we want to find 'projected wall height', we reorganize the equation to:
        //   'projected wall height' = 'actual wall height'/'distance to wall' * 'distance form player to proj. plane'        
        float wallHeight = (TILE_SIZE / perpDistance) * DIST_PROJ_PLANE;

        // Find the wall top Y value


        //  note: for the wallTopPixel, we want to place the rectangle aligned with the center of the projection
        //   so that would be:  A = WINDOW_HEIGHT / 2    , but the rectangle also needs to be centered, so to find
        //   its center we do: B = wallStripHeight / 2   , now we need to do  C = A - B
        //   for instance suppose you have a WINDOW_HEIGHT = 1000px, and wallStripHeight = 300px
        //   you would have (1000/2) - (300/2) = 500 - 150 = 350. 
        //   That means, from the Y axis we would have 350 blank pixels, draw 300, then another 350 blank pixels
        //   resulting in the 'strip' aligned with the center of the screen
        int wallTopY = (WINDOW_HEIGHT / 2) - (wallHeight / 2);
        wallTopY = wallTopY < 0 ? 0 : wallTopY;

        // Find the wall bottom Y value
        int wallBottomY = (WINDOW_HEIGHT / 2) + (wallHeight / 2);
        wallBottomY = wallBottomY > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomY;

	// Draw the ceiling
        // set the color of the ceiling
        //  Notes: colorBuffer is an one dimensional array. So if we have an window W: 1280 and H: 832
        //  colorbuffer is 1,064,960. And if we want to colour the first column we could use an operation like
        //  this: colorBuffer[(WINDOW_WIDTH * y) + i] = 0xFF333333
        for (int y = 0; y < wallTopY; y++) 
	{
            // suppose this is column 0 and the first wall top pixel is at position 100 and we start at 
            // the pixel 0, we would have:  (1280 * 0) + 0 = 0, then: (1280 * 1) + 0 = 1280
            // then: (1280 * 2) + 0 = 2560.
            // As you can see, we are working basically as a scanline, when we set the 1st pixel we need to
            //  jump an entire window width in order to reach the next pixel to colour
            drawPixel(x, y, 0xFF444444);
        }

        // Draw the textured wall
        // calculate texture offset X
        //  e.g.: rays[i].wallHitY % TEXTURE_HEIGHT => 211 % 32 = 19 pixels offset
        int textureOffsetX;
        if (rays[x].wasHitVertical)
        {
            textureOffsetX = (int)rays[x].wallHitY % TILE_SIZE;
        }
        else
        {
            textureOffsetX = (int)rays[x].wallHitX % TILE_SIZE;        
        }

        // Get the correct texture id number from the map content
        int texNum = rays[x].wallHitContent - 1;

        // Query the texture width and height from the upng
        int textureWidth = upng_get_width(textures[texNum]);
        int textureHeight = upng_get_height(textures[texNum]);


        //Note: In the texture map, we found which column we are using with the help
        //  of the variable textureOffsetX
        // Now we need to map this to the Y axis. 

        // int count = 0, readkey;
        // render the wall from wallTopPixel to wallBottomPixel
        // Render the wall from wallTopY to wallBottomY
        for (int y = wallTopY; y < wallBottomY; y++)
        {
            // calculate texture offset Y
            //  what we are trying to find here is the pixel in which we should start 'coloring'. It should be the
            //  first pixel of the wallstrip. And since the wall strip is always centered in the screen we should go
            //  something like the example below: (WINDOW_HEIGHT / 2) - (wallStripHeight / 2) => 
            //  (832 / 2) - (123 / 2) = (416) - (61) = 355
            //  And then if y = 0 we paint the pixel at index 0, y = 1 -> pixel at index 1 ...
            int distanceFromTop = y + (wallHeight / 2) - (WINDOW_HEIGHT / 2);
            
	    
	    //typically: ((float)TEXTURE_HEIGHT / wallStripHeight) = 64 / 123 = 0.520325
            // so for y=0,...,y=n =>  0 * 0.52 = 0 ; 1 * 0.52 = 0.52 ; 2 * 0.52 = 1.04 ; 3 * 0.52 = 1.560976
            // and since textureOffsetY is int, the values will be 0 , 0, 1, 1 , 2 , .... (the values are rounded down)
            int textureOffsetY = distanceFromTop * ((float)textureHeight / wallHeight);

            // if (count < 10)
            // {
            //     printf("\n----------------------\n");
            //     //------
            //     // printf("   wallStripHeight: %d  - wallStripHeight / 2: %d \n", wallStripHeight, (wallStripHeight / 2));
            //     // printf("   WINDOW_HEIGHT: %d  - WINDOW_HEIGHT / 2: %d \n", WINDOW_HEIGHT, (WINDOW_HEIGHT / 2));
            //     //------
            //     // printf("   y: %d  - (wallStripHeight / 2) - (WINDOW_HEIGHT / 2): %d \n", y, ((wallStripHeight / 2) - (WINDOW_HEIGHT / 2)));
            //     // printf("distanceFromTop: %d \n", distanceFromTop);
            //     //------

            //     printf("   distanceFromTop: %d , y: %d \n", distanceFromTop, y);
            //     printf("   TEXTURE_HEIGHT: %d , wallStripHeight: %d\n", TEXTURE_HEIGHT,wallStripHeight);
            //     printf("   ((float)TEXTURE_HEIGHT / wallStripHeight): %f  \n", ((float)TEXTURE_HEIGHT / wallStripHeight));
            //     printf("   distanceFromTop * ((float)TEXTURE_HEIGHT / wallStripHeight): %f \n", (distanceFromTop * ((float)TEXTURE_HEIGHT / wallStripHeight)));
            //     printf("textureOffsetY: %d \n",textureOffsetY);
                
            //     count++;
            // }
            // else
            // {
            //     getchar();
            //     count = 0;
            // }            

            
            // Set the color of the wall based on the color from the texture
            color_t* wallTextureBuffer = (color_t*) upng_get_buffer(textures[texNum]);
            color_t texelColor = wallTextureBuffer[(textureWidth * textureOffsetY) + textureOffsetX];

            // Make the pixel color darker if the ray hit was vertical
            if (rays[x].wasHitVertical) 
	    {
                changeColorIntensity(&texelColor, 0.7);
            }            
            
            
            
            drawPixel(x, y, texelColor);
        }

        // Draw the floor
        for (int y = wallBottomY; y < WINDOW_HEIGHT; y++) 
	{
            drawPixel(x, y, 0xFF888888);
        }
    }
}





