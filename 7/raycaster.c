#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#define PI 3.141592653589793238
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //one degree in radians
#define max_num(x,y) (((x) >= (y)) ? (x) : (y))

struct WindowProperties
{
    const int height;
    const int width;
    const float backgroundColor4f[]; // red, green, blue, alpha
};
struct PlayerDetails
{
    int pointSize;
    float x;
    float y;
    float angle;
    float deltaX; // player deltas - this is the intensity to which we are moving on the X/Y axis
    float deltaY;
    int deltaMultiplier;
    //int directionVectorMultiplier;
    //float playerColor3f[3];
};

struct PlayerImplementationDetails
{
    int directionVectorMultiplier;
    int beamThicknessHorizontal;
    int beamThicknessVertical;
    float beamColorHorizontal3f[3];
    float beamColorVertical3f[3];
    float playerColor3f[3];
};

struct ControlsDetails
{
    float angleStep;
};

struct MapDetails
{
    const int width;
    const int height;
    int tileSizePx;
    int lineThickness;
    const int mapCount;
    const int wall;
    float tileFloorColor3f[3];
    float tileWallColor3f[3];
    float firstTileColor3f[3];
    const int map[];
};

struct RayDetails
{
    int RayPositionRound;
};

//-----------------------------------------------
struct WindowProperties window = { .height = 512, .width = 1024, .backgroundColor4f = {0.3f, 0.3f, 0.3f, 0}};
struct PlayerDetails playerDet = { .pointSize = 8, .x = 300, .y = 300, .deltaX = 5, /* cos(0)*5 */
                                   .deltaY = 0,                                  /* sin(0)*5 */
                                   .angle = 0,
                                   .deltaMultiplier = 5
};
struct PlayerImplementationDetails playerImpDet = 
{
    .directionVectorMultiplier = 5, .beamThicknessHorizontal = 6, .beamThicknessVertical = 3,
    .beamColorHorizontal3f = {0.0f,1.0f,0.0f}, .beamColorVertical3f = {1.0f,0.0f,0.0f}, 
    .playerColor3f = {1.0f, 0.0f, 1.0f}
};

struct ControlsDetails controlDet = { .angleStep = 0.1f};

struct MapDetails mapDet = { .map = {
                                    1,1,1,1,1,1,1,1, //the map array. Edit to change level but keep the outer walls
                                    1,0,1,0,0,0,0,1,
                                    1,0,1,0,0,0,0,1,
                                    1,0,1,0,0,0,0,1,
                                    1,0,0,0,0,0,0,1,
                                    1,0,0,0,0,1,0,1,
                                    1,0,0,0,0,0,0,1,
                                    1,1,1,1,1,1,1,1,	
                            },
                            .width = 8,
                            .height = 8,
                            .mapCount = 8*8, //width * height
                            .tileSizePx = 64,
                            .wall = 1,
                            .tileFloorColor3f = {0.0f, 0.0f, 0.0f},
                            .tileWallColor3f = {1.0f, 1.0f, 1.0f},
                            .firstTileColor3f = {0.8f, 0.8f, 0.8f},
                            .lineThickness =1 };

struct RayDetails rayDetails = { .RayPositionRound = 6};
//-----------------------------------------------





void drawPlayer()
{
    // player dot
    glColor3f( playerImpDet.playerColor3f[0], playerImpDet.playerColor3f[1], playerImpDet.playerColor3f[2]);
    glPointSize(playerDet.pointSize);
    glBegin(GL_POINTS);
    glVertex2i(playerDet.x, playerDet.y);
    glEnd();

    // direction
    int x1 = playerDet.x + playerDet.deltaX * playerImpDet.directionVectorMultiplier;
    int y1 = playerDet.y + playerDet.deltaY * playerImpDet.directionVectorMultiplier;
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(playerDet.x, playerDet.y);
    glVertex2i(x1, y1);
    glEnd();

}

void drawMap2D()
{
    int mapX = mapDet.width, mapY = mapDet.height, blockSize = mapDet.tileSizePx;
    //const int lineThickness = 1;
    // the loop below executes mapY * mapX -> 8 * 8 = 64;
    int x, y, xOffset, yOffset;
    for (y = 0; y < mapY; y++) // we are looping through a 1D array
    {
        for (x = 0; x < mapX; x++)
        {
            // We are using a 1D array and making it behave as it was a 2D array of 8x8 (64 items) this could be:
            //  idx: 0    - y*mapX: 0 ,     x: 0
            //  idx: 1    - y*mapX: 0 ,     x: 1
            //  ...
            //  idx: 7    - y*mapX: 0 ,     x: 7
            //  idx: 8    - y*mapX: 8 ,     x: 0
            //  idx: 9    - y*mapX: 8 ,     x: 1
            //  idx: 10   - y*mapX: 8 ,     x: 2
            //  ...
            //  idx: 62   - y*mapX: 56 ,    x: 6
            //  idx: 63   - y*mapX: 56 ,    x: 7

            //  e.g.: 3*8    + 2  -> 24+2 -> 26
            int idx = y * mapX + x;

            if (mapDet.map[idx] == 1) // wall
            { glColor3f(mapDet.tileWallColor3f[0], mapDet.tileWallColor3f[1], mapDet.tileWallColor3f[2]); }
            else // floor
            { glColor3f(mapDet.tileFloorColor3f[0], mapDet.tileFloorColor3f[1], mapDet.tileFloorColor3f[2]); }


            xOffset = x * blockSize;
            yOffset = y * blockSize;

            // mark the first tile
            if (xOffset == 0 && yOffset == 0)
            { glColor3f(mapDet.firstTileColor3f[0], mapDet.firstTileColor3f[1], mapDet.firstTileColor3f[2]); }

            glBegin(GL_QUADS);
            glVertex2i(xOffset,                                      yOffset                                     );
            glVertex2i(xOffset,                                      yOffset + (blockSize - mapDet.lineThickness));
            glVertex2i(xOffset + (blockSize - mapDet.lineThickness), yOffset + (blockSize - mapDet.lineThickness));
            glVertex2i(xOffset + (blockSize - mapDet.lineThickness), yOffset                                     );
            glEnd();

            // X      , Y
            // 0      , 0
            // 0      , 0 + 64
            // 0 + 64 , 0 + 64
            // 0 + 64 , 0

            // getchar();
        }
    }
}

float dist(float pointA_X, float pointA_Y, float pointB_X, float pointB_Y, float ang)
{
    return ( sqrt( (pointB_X-pointA_X) * (pointB_X-pointA_X) + (pointB_Y-pointA_Y) * (pointB_Y-pointA_Y) )  );
}

void drawRays2D()
{   

    int ray;
    float rayXEndPos, rayYEndPos, raysAngle, xOffset, yOffset; //rayXPos and rayYPos indicates where the rays hit the closest horizontal line
    //raysAngle=playerDet.angle;

    const float accuracyConst = 0.0001;
    float roundUp = 0;



    int mapX, mapY, tileInspecting = 0 , depthOfField = 0;
    int mapMaxDepth = max_num(mapDet.width, mapDet.height);   

    raysAngle = playerDet.angle - DR * 30;
    if( raysAngle < 0){ raysAngle += 2*PI; }
    if( raysAngle > 2*PI){ raysAngle -= 2*PI; } 

    if(raysAngle == 0 || raysAngle == PI) //looking directly left or right
    { raysAngle += accuracyConst; }  

    for(ray = 0 ; ray < 1 ; ray++ )
    {
        depthOfField = 0;
        float distHorizontal = 1000000, horX = playerDet.x , horY = playerDet.y;
        //check horizontal lines
        float aTan_rayAngle = -1 / tan(raysAngle); //finding angle???
        //printf("raysAngle: %f , tan(raysAngle): %f, -1 / tan(raysAngle): %f\n", raysAngle, tan(raysAngle), (-1 / tan(raysAngle)));


        //-----------
        //are we looking up or down?
        // looking up is between 0 deg and 180 deg, which in radians, which in this application is between +6.28 and +3.14
        // anything else is looking down


        //we also need to round the ray position to the nearest 64 value (blocksize)
        // it's done by bit shifting right by 6, which is (num >> 6) and then bit shifting left by 6 (num << 6)
        // the 6 in this case is equivalent dividing by 64
        // e.g.:     69 >> 6 = 1   ---> 1000101 >> 6 = 1 (we just lost all the less significant digits)
        // and then: 1 << 6 =  1000000 ---> 64

        if( raysAngle > PI ) //looking up
        { 
            roundUp = ( (int)playerDet.y >> rayDetails.RayPositionRound) << rayDetails.RayPositionRound; //round to the next tile multiplier position

            rayYEndPos = roundUp - accuracyConst; 
            rayXEndPos = (playerDet.y - rayYEndPos) * aTan_rayAngle + playerDet.x; // (dist) * -1/tan + x
            //the movement is focused on Y axis, so it always receive 'full values' while X gets proportionally.
            yOffset = -mapDet.tileSizePx; //looking up, therefore we reduce Y, since the map has origin (0,0)
            xOffset = -yOffset * aTan_rayAngle;  
        } 
        else if( raysAngle < PI )//looking down
        { 
            roundUp = ( (int)playerDet.y >> rayDetails.RayPositionRound) << rayDetails.RayPositionRound;
            
            rayYEndPos = roundUp + mapDet.tileSizePx; //trying to be sure we are projecting at least 1 tile
            rayXEndPos = (playerDet.y - rayYEndPos) * aTan_rayAngle + playerDet.x; 
            yOffset = mapDet.tileSizePx; 
            xOffset = -yOffset * aTan_rayAngle;  
        } 
        if( raysAngle == 0 || raysAngle == PI ) //looking directly left or right
        { 
            rayXEndPos = playerDet.x; 
            rayYEndPos = playerDet.y; 
            depthOfField = mapMaxDepth; 
        } 
        //printf("p.y: %f, rayYEndPos: %f, (p.y - rayYEndPos):, %f , p.x: %f \n", playerDet.y, rayYEndPos, (playerDet.y - rayYEndPos), playerDet.x);
        //printf("  rayXEndPos: %f  - (playerDet.y - rayYEndPos) * aTan_rayAngle: %f\n", rayXEndPos, ((playerDet.y - rayYEndPos) * aTan_rayAngle));

        //printf("-- p.x: %f, p.y: %f, rayXEndPos: %f, rayYEndPos: %f\n", playerDet.x, playerDet.y, rayXEndPos, rayYEndPos);
        

        //the rayYEndPos and rayXEndPos where given a somewhat arbitrary lengthy value, we are just trying to point it in the same direction as the player
        // the goal now is to use this way and its direction, figure out, if we will hit a wall.


    
        //printf("--------------------------------------\n");
        while(depthOfField < mapMaxDepth) //this is max depth of the map - we do not need to check further if that's the case
        {
            

            //printf("rayXEndPos: %d\n", rayXEndPos);
            mapX = (int)(rayXEndPos) >> rayDetails.RayPositionRound; 
            mapY = (int)(rayYEndPos) >> rayDetails.RayPositionRound; 
            tileInspecting = (mapY * mapDet.width) + mapX;  

            if(tileInspecting > mapDet.mapCount) { tileInspecting = 0; } //bug fix

            //printf("my: %d , mx: %d , tileIns: %d, yOffset: %f , xOffset: %f\n", mapY, mapX, tileInspecting , yOffset, xOffset);
            // printf("my: %d , mx: %d , mapDet.width: %d\n", my, mx, mapDet.width);
            // printf("my: %d , mx: %d , mapDet.width: %d\n", my, mx, mapDet.width);
            // printf("mapDet.width * mapDet.height: %f\n", mapDet.width * mapDet.height);
            // printf("mp: %d\n", mp);
            // printf("mapDet.map[mp]: %d\n", mapDet.map[mp]);

            //trying to find vertical hits
            if(tileInspecting > 0 && tileInspecting < mapDet.mapCount && mapDet.map[tileInspecting] == mapDet.wall) //hit wall
            { 
                horX = rayXEndPos; 
                horY = rayYEndPos; 
                distHorizontal = dist(playerDet.x, playerDet.y, horX, horY, raysAngle);
                depthOfField = mapMaxDepth;
            } 
            else
            {   //next line
                rayXEndPos += xOffset; 
                rayYEndPos += yOffset; 
                depthOfField += 1; 
            } 

        }
        //--------
        // glColor3f(playerImpDet.beamColorHorizontal3f[0],playerImpDet.beamColorHorizontal3f[1],
        //     playerImpDet.beamColorHorizontal3f[2]);
        // glLineWidth(playerImpDet.beamThicknessHorizontal);
        // glBegin(GL_LINES);
        // glVertex2i(playerDet.x, playerDet.y);
        // glVertex2i(rayXEndPos, rayYEndPos);
        // glEnd();


        //-----------------------------
        //check vertical lines
        depthOfField = 0;
        float distVertical = 1000000, vertX = playerDet.x , vertY = playerDet.y;
        float nTan_rayAngle = -tan(raysAngle); //negative tangent
        // num >> 6 = divide by 64    and   num << 6 = multiply by 64
        if( raysAngle > P2 && raysAngle <  P3 )
        { 
            rayXEndPos = ( ((int)playerDet.x >> rayDetails.RayPositionRound) << rayDetails.RayPositionRound ) - accuracyConst; 
            rayYEndPos = (playerDet.x - rayXEndPos) * nTan_rayAngle + playerDet.y; 
            xOffset = -mapDet.tileSizePx; 
            yOffset = -xOffset*nTan_rayAngle;  
        } //looking left
        if( raysAngle < P2 || raysAngle >  P3 )
        { 
            rayXEndPos = ( ((int)playerDet.x >> rayDetails.RayPositionRound) << rayDetails.RayPositionRound ) + mapDet.tileSizePx;     
            rayYEndPos = (playerDet.x - rayXEndPos) * nTan_rayAngle + playerDet.y; 
            xOffset =  mapDet.tileSizePx; 
            yOffset = -xOffset*nTan_rayAngle;  
        } //looking right
        if( raysAngle == 0 || raysAngle == PI )
        { 
            rayXEndPos = playerDet.x; 
            rayYEndPos = playerDet.y; 
            depthOfField = mapMaxDepth; 
        } //looking straight up or down
        
        while(depthOfField < mapMaxDepth) //this is max depth of the map - we do not need to check further if that's the case
        {
            mapX = (int)(rayXEndPos) >> rayDetails.RayPositionRound; 
            mapY = (int)(rayYEndPos) >> rayDetails.RayPositionRound; 
            tileInspecting = mapY*mapX + mapX;  

            if(tileInspecting > 0 && tileInspecting < mapX * mapY && mapDet.map[tileInspecting] == mapDet.wall)
            { 
                vertX = rayXEndPos; 
                vertY = rayYEndPos; 
                distVertical = dist( playerDet.x, playerDet.y,vertX,vertY, raysAngle); 
                depthOfField = mapMaxDepth;
            } //hit wall
            else
            { 
                rayXEndPos += xOffset; 
                rayYEndPos += yOffset; 
                depthOfField += 1; 
            } //next line
        }


        if(distVertical < distHorizontal)
        { 
            rayXEndPos = vertX; 
            rayYEndPos = vertY; 
        }
        if(distHorizontal < distVertical)
        { 
            rayXEndPos = horX; 
            rayYEndPos = horY;
        }

        glColor3f(playerImpDet.beamColorVertical3f[0],playerImpDet.beamColorVertical3f[1],
            playerImpDet.beamColorVertical3f[2]);
        glLineWidth(playerImpDet.beamThicknessVertical);
        glBegin(GL_LINES);
        glVertex2i(playerDet.x, playerDet.y);
        glVertex2i(rayXEndPos, rayYEndPos);
        glEnd();
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT   );
    drawMap2D();
    drawRays2D();
    drawPlayer();
    glutSwapBuffers();
}

void buttons(unsigned char key, int x ,int y)
{
    //note: the circle is 2 * PI, so every time the angle is less than zero or above 2*PI, we have to 'reset' it
    //note2: the Deltas have to be multiplied by a number or else we get something like: playerDet.x = playerDet.x + 0.21
    //    when we probably want somethine like playerDet.x = playerDet.x + (0.21 * 5) -> playerDet.x = playerDet.x + 1.05
    if (key == 'a')
    { 
        playerDet.angle -= controlDet.angleStep;  
        if(playerDet.angle < 0 ) { playerDet.angle += 2 * PI; } //circle = 2*PI*Rad
        
        playerDet.deltaX = cos(playerDet.angle) * playerDet.deltaMultiplier; 
        playerDet.deltaY = sin(playerDet.angle) * playerDet.deltaMultiplier; 
    }
    if (key == 'd')
    { 
        playerDet.angle += controlDet.angleStep; 
        if(playerDet.angle > 2 * PI){ playerDet.angle -= 2 * PI; } 
        
        playerDet.deltaX = cos(playerDet.angle) * playerDet.deltaMultiplier; 
        playerDet.deltaY = sin(playerDet.angle) * playerDet.deltaMultiplier; 
    }
    if (key == 'w') //simple
    { 
        playerDet.x += playerDet.deltaX; 
        playerDet.y += playerDet.deltaY; 
    }
    if (key == 's') //simple
    { 
        playerDet.x -= playerDet.deltaX; 
        playerDet.y -= playerDet.deltaY; 
    }

    glutPostRedisplay();
}


void init()
{
    glClearColor(window.backgroundColor4f[0],window.backgroundColor4f[1],window.backgroundColor4f[2],window.backgroundColor4f[3]); //red, green, blue, alpha
    gluOrtho2D(0,window.width,window.height,0);
    //px=300; py=300;
    // pdx = cos(pa)*5; pdy = sin(pa)*5;
    rayDetails.RayPositionRound = (int)(ceil(log(mapDet.tileSizePx) / log(2))); //log2(num) = x -> this will be used for bit shifting

}

int main(int argc, char* argv[])
{
    printf("environment and application: OK\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("Raycaster");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();

}


//install glut: sudo apt-get install freeglut3-dev
//compile on linux
// use: gcc raycaster.c -lGL -lGLU -lglut -lm
//      ./a.out
//compile on linux

// use: gcc raycaster.c -framework OpenGL -framework GLUT
//      ./a.out
