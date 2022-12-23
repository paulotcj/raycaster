#include "ray.h"

ray_t rays[NUM_RAYS];

void normalizeAngle(float* angle)
{
    *angle = remainder(*angle, TWO_PI);
    if( *angle < 0 )
    {
        //2*pi would reset the angle to the start (or end, depending of your preference) position
        // but that might translate into a hiccup
        // by doing (2*pi) + angle  the transition is seamless
        *angle = TWO_PI + *angle;
    }


}

float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
//---------
bool isRayFacingDown(float angle) 
{
    return angle > 0 && angle < PI; 
}

bool isRayFacingUp(float angle) 
{
    return !isRayFacingDown(angle);
}

bool isRayFacingRight(float angle) 
{
    return angle < 0.5 * PI || angle > 1.5 * PI;
}

bool isRayFacingLeft(float angle) 
{
    return !isRayFacingRight(angle);
}

//---------

void castRay(float rayAngle, int stripId)
{
	normalizeAngle(&rayAngle);
    //angles: for the purpose of this application the angle 0 will be 2 * PI
    // or 6.283185307179586
    // So UP is everything between: 2*PI or zero, and PI (6.28 or 0, and 3.14), 
    //and by exclusion, down when the flag UP is not set
    //
    // Right: Right is when we have angles between 0deg and 90deg, and, between 270 and 360
    // to put differently, between 0 and 0.5*PI or between 1.5*PI and 2*PI

    float xintercept, yintercept;
    float xstep, ystep;

    ///////////////////////////////////////////
    // HORIZONTAL RAY-GRID INTERSECTION CODE
    ///////////////////////////////////////////    
    bool foundHorzWallHit = false;
    float horzWallHitX = 0;
    float horzWallHitY = 0;
    int horzWallContent = 0;

    //------------------------
    // INTERCEPT SECTION - START
    //
    // Find the y-coordinate of the closest horizontal grid intersection
    //  take the player coordinates and divide it by the tile size and round down the division
    //  to find which tile (row) is directly above the player. Keep in mind the tile (rows) start
    //  from top to bottom, so it's 32,64,96,...
    // Now, in case the player is facing down, the nearest ray intersection is, the same calculation
    //  as above, but we add ONE TILE    
    yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
    yintercept += isRayFacingDown(rayAngle) ? TILE_SIZE : 0;

    // Find the x-coordinate of the closest horizontal grid intersection
    //  note: we have the yintercept, which in a triangle this would be the hypotenuse
    //   and we are trying to find the adjacent side, and we have the angle
    //  So the formula: tan(x) = opp / ajd -> adj = opp / tan(x)    
    xintercept = player.x + ( yintercept - player.y ) / tan(rayAngle);
    
    // INTERCEPT SECTION - END
    //------------------------
    
    //------------------------
    // STEP SECTION - START
    // Calculate the increment xstep and ystep
    //  note: when scanning horizontally, the Y step is always 1 tile size
    ystep = TILE_SIZE;
    ystep *= isRayFacingUp(rayAngle) ? -1 : 1;

    //note: when scanning horizontally the X step is always proportional
    xstep = TILE_SIZE / tan(rayAngle);
    // if the ray is supposed to face left but the xstep is positive, then multiply by -1, so the orientation is correct
    // if the ray is supposed to face right but the xstep is negative, then multiply by -1, so the orientation is correct
    xstep *= (isRayFacingLeft(rayAngle) && xstep > 0) ? -1 : 1;
    xstep *= (isRayFacingRight(rayAngle) && xstep < 0) ? -1 : 1;
    // STEP SECTION - START
    //------------------------
    

    float nextHorzTouchX = xintercept;
    float nextHorzTouchY = yintercept;

    // Increment xstep and ystep until we find a wall
    while ( isInsideMap(nextHorzTouchX, nextHorzTouchY) ) 
    {
        float xToCheck = nextHorzTouchX;
        float yToCheck = nextHorzTouchY + (isRayFacingUp(rayAngle) ? -1 : 0); // if ray is facing up, force one pixel up so we are inside a grid cell

        if( mapHasWallAt( xToCheck, yToCheck ) )
        {
            // found a wall hit
            horzWallHitX = nextHorzTouchX;
            horzWallHitY = nextHorzTouchY;
            horzWallContent = getMapAt((int)floor(yToCheck / TILE_SIZE), (int)floor(xToCheck / TILE_SIZE));
            foundHorzWallHit = true;
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
    bool foundVertWallHit = false;
    float vertWallHitX = 0;
    float vertWallHitY = 0;
    int vertWallContent = 0;

    //------------------------
    // INTERCEPT SECTION - START
    //
    // Find the x-coordinate of the closest vertical grid intersection
    xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
    xintercept += isRayFacingRight(rayAngle) ? TILE_SIZE : 0;

    // Find the y-coordinate of the closest vertical grid intersection
    yintercept = player.y + (xintercept - player.x) * tan(rayAngle);
    // INTERCEPT SECTION - START
    //------------------------
    
    //------------------------
    // STEP SECTION - START
    // Calculate the increment xstep and ystep
    xstep = TILE_SIZE;
    xstep *= isRayFacingLeft(rayAngle) ? -1 : 1;

    ystep = TILE_SIZE * tan(rayAngle);
    ystep *= (isRayFacingUp(rayAngle) && ystep > 0) ? -1 : 1;
    ystep *= (isRayFacingDown(rayAngle) && ystep < 0) ? -1 : 1;
    // STEP SECTION - END
    //------------------------
    

    float nextVertTouchX = xintercept;
    float nextVertTouchY = yintercept;

    // Increment xstep and ystep until we find a wall
    while ( isInsideMap(nextVertTouchX, nextVertTouchY) ) 
    {
        float xToCheck = nextVertTouchX + (isRayFacingLeft(rayAngle) ? -1 : 0); // if ray is facing left, force one pixel left so we are inside a grid cell
        float yToCheck = nextVertTouchY;
        
        if (mapHasWallAt(xToCheck, yToCheck)) 
        {
            // found a wall hit
            vertWallHitX = nextVertTouchX;
            vertWallHitY = nextVertTouchY;
            vertWallContent = getMapAt((int)floor(yToCheck / TILE_SIZE), (int)floor(xToCheck / TILE_SIZE));
            foundVertWallHit = true;
            break;
        } 
        else 
        {
            nextVertTouchX += xstep;
            nextVertTouchY += ystep;
        }
    }   

    //-------------------------------

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
        rays[stripId].wasHitVertical = true;
        rays[stripId].rayAngle = rayAngle;
    } 
    else 
    {
        rays[stripId].distance = horzHitDistance;
        rays[stripId].wallHitX = horzWallHitX;
        rays[stripId].wallHitY = horzWallHitY;
        rays[stripId].wallHitContent = horzWallContent;
        rays[stripId].wasHitVertical = false;
        rays[stripId].rayAngle = rayAngle;

    }
    

}



void castAllRays(void)
{
    // start first ray subtracting half of the FOV
    //  note: we are tying to find the angle of the first ray, so if the player has a field of view of 60deg
    //   we divide it by 2, so it means it's 30deg to the left to where the player is looking plus 30deg
    //   to the right. In this case we start scanning from the left to right, so we subtract the angle
    //   so imagine the player is looking at 45deg, we then subtract 30 from FOV/2 which indicates the
    //   first ray is cast at 15deg
    //float rayAngle = player.rotationAngle - (FOV_ANGLE / 2);

    // loop all columns casting the rays
    for(int col = 0 ; col < NUM_RAYS ; col++)
    {
        //castRay(rayAngle, stripID);
        //rayAngle += FOV_ANGLE / NUM_RAYS;

        //If we assume a constant angle between arrays we will end up with a distorted view. In order to
        // render it straight we need to assume that the space between rays hitting the walls is equal
        // so each ray angle is different and we need to find this angle.
        // Note: arc tan = atan -> finds the angle based on triangle's sides
        //  tan(x) = Opp/Adj    atan(Opp/Adj) = x
        // in our example: atan(pixel columns / distance proj plane) = angle
        // with NUM_RAYS = 1280; NUM_RAYS/2 = 640; the expression: col - NUM_RAYS/2 ranges from -640 to +640
        // and DIST_PROJ_PLANE = 1108.512518
        // Therefore, the result of the expression: atan((col - NUM_RAYS/2) / DIST_PROJ_PLANE) yields different
        //  increments of angles per ray. For instance some diffs: 0.000814 , 0.000821 , 0.000885 , 0.000687
        //  The actual angles could be in the range: -0.5235987751467682 to 0.5235987751467682
        //  

        //printf("NUM_RAYS: %d  ,  NUM_RAYS/2: %d  ,  col: %d \n",NUM_RAYS, (NUM_RAYS/2), col);
        //printf("atan((col - NUM_RAYS/2) / DIST_PROJ_PLANE): %f\n", atan((col - NUM_RAYS/2) / DIST_PROJ_PLANE));
        float rayAngle = player.rotationAngle + atan((col - NUM_RAYS/2) / DIST_PROJ_PLANE);
        castRay(rayAngle, col);
    }
}


void renderRays(void) 
{
    //for (int i = 0; i < NUM_RAYS; i += 50) 
    for (int i = 0; i < NUM_RAYS; i += 30 ) 
    {
        drawLine(
            player.x * MINIMAP_SCALE_FACTOR,
            player.y * MINIMAP_SCALE_FACTOR,
            rays[i].wallHitX * MINIMAP_SCALE_FACTOR,
            rays[i].wallHitY * MINIMAP_SCALE_FACTOR,
            0xFF0000FF
        );
    }
}
