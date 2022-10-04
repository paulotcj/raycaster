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
    int beamThickness;
    float beamColor3f[3];
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
    float tileFloorColor3f[3];
    float tileWallColor3f[3];
    float firstTileColor3f[3];
    const int map[];
};

struct WindowProperties window = { .height = 512, .width = 1024, .backgroundColor4f = {0.3f, 0.3f, 0.3f, 0}};
struct PlayerDetails playerDet = { .pointSize = 8, .x = 300, .y = 300, .deltaX = 5, /* cos(0)*5 */
                                   .deltaY = 0,                                  /* sin(0)*5 */
                                   .angle = 0,
                                   .deltaMultiplier = 5
};
struct PlayerImplementationDetails playerImpDet = 
{
    .directionVectorMultiplier = 5, .beamThickness = 1, .beamColor3f = {0.0f,1.0f,0.0f}, 
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
                            .tileSizePx = 64,
                            .tileFloorColor3f = {0.0f, 0.0f, 0.0f},
                            .tileWallColor3f = {1.0f, 1.0f, 1.0f},
                            .firstTileColor3f = {0.8f, 0.8f, 0.8f},
                            .lineThickness =1 };

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
            // printf("idx: %d   - y*mapX: %d ,    x: %d \n",idx, y*mapX, x);
            if (mapDet.map[idx] == 1) // wall
            { glColor3f(mapDet.tileWallColor3f[0], mapDet.tileWallColor3f[1], mapDet.tileWallColor3f[2]); }
            else // floor
            { glColor3f(mapDet.tileFloorColor3f[0], mapDet.tileFloorColor3f[1], mapDet.tileFloorColor3f[2]); }

            // printf("x*blockSize: %d   - y*blockSize: %d\n",x*blockSize,y*blockSize);
            xOffset = x * blockSize;
            yOffset = y * blockSize;

            // mark the first tile
            if (xOffset == 0 && yOffset == 0)
            { glColor3f(mapDet.firstTileColor3f[0], mapDet.firstTileColor3f[1], mapDet.firstTileColor3f[2]); }

            // printf("x0: %d, y0: %d \n", x0, y0);
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
void drawRays2D()
{
    //int r;
    int mx, my, mp, depthOfField;
    float rayXPos, rayYPos, raysAngle, xOffset, yOffset; //rayXPos and rayYPos indicates where the rays hit the closest horizontal line
    raysAngle=playerDet.angle;

    int mapMaxDepth = max_num(mapDet.width, mapDet.height);
    const float accuracyConst = 0.0001;

    //check horizontal lines
    depthOfField = 0;
    float aTan = -1 / tan(raysAngle); //finding angle???

    //-----------
    //are we looking up or down?
    // looking up is between 0 deg and 180 deg, which in radians, which in this application is between +6.28 and +3.14
    // anything else is looking down

    //we also need to round the ray position to the nearest 64 value (blocksize)
    // it's done by bit shiftting right by 6, which is (num >> 6) and then bit shiftting left by 6 (num << 6)
    // the 6 in this case is equivalent dividing by 64
    // e.g.:     69 >> 6 = 1   ---> 1000101 >> 6 = 1 (we just lost all the less significant digits)
    // and then: 1 << 6 =  1000000 ---> 64

    printf("raysAngle: %f - PI: %f\n",raysAngle, PI);
    if(      raysAngle > PI ){ rayYPos = ( ((int)playerDet.y >> 6) << 6 ) - accuracyConst; rayXPos = (playerDet.y - rayYPos) * aTan + playerDet.x; yOffset = -64; xOffset = -yOffset*aTan;  } //looking up
    else if( raysAngle < PI ){ rayYPos = ( ((int)playerDet.y >> 6) << 6 ) + 64;            rayXPos = (playerDet.y - rayYPos) * aTan + playerDet.x; yOffset =  64; xOffset = -yOffset*aTan;  } //looking down
    if(raysAngle == 0 || raysAngle == PI){ rayXPos = playerDet.x; rayYPos = playerDet.y; depthOfField = 8; } //looking straight left or right
    
    //-----------
    while(depthOfField < mapMaxDepth) //this is max depth of the map - we do not need to check further if that's the case
    {
        mx = (int)(rayXPos) >> 6; my = (int)(rayYPos) >> 6; mp = my*mapDet.width + mx;  

        if(mp < mapDet.width * mapDet.height && mapDet.map[mp] == 1)
        { depthOfField = 8;} //hit wall
        else
        { rayXPos += xOffset; rayYPos += yOffset; depthOfField += 1; } //next line
    }
    //-----------
    
    glColor3f(playerImpDet.beamColor3f[0],playerImpDet.beamColor3f[1],playerImpDet.beamColor3f[2]);
    glLineWidth(playerImpDet.beamThickness);
    glBegin(GL_LINES);
    glVertex2i(playerDet.x, playerDet.y);
    glVertex2i(rayXPos, rayYPos);
    glEnd();
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
    if (key == 'a'){ playerDet.angle -= controlDet.angleStep; if(playerDet.angle < 0     ){ playerDet.angle += 2 * PI; } playerDet.deltaX = cos(playerDet.angle) * playerDet.deltaMultiplier; playerDet.deltaY = sin(playerDet.angle) * playerDet.deltaMultiplier; }
    if (key == 'd'){ playerDet.angle += controlDet.angleStep; if(playerDet.angle > 2 * PI){ playerDet.angle -= 2 * PI; } playerDet.deltaX = cos(playerDet.angle) * playerDet.deltaMultiplier; playerDet.deltaY = sin(playerDet.angle) * playerDet.deltaMultiplier; }
    if (key == 'w'){ playerDet.x += playerDet.deltaX; playerDet.y += playerDet.deltaY; }
    if (key == 's'){ playerDet.x -= playerDet.deltaX; playerDet.y -= playerDet.deltaY; }

    //printf("pa: %f - px: %f - py: %f - pdx: %f - pdy: %f \n", playerDet.angle, playerDet.x, playerDet.y, playerDet.deltaX, playerDet.deltaY);

    glutPostRedisplay();
}


void init()
{
    glClearColor(window.backgroundColor4f[0],window.backgroundColor4f[1],window.backgroundColor4f[2],window.backgroundColor4f[3]); //red, green, blue, alpha
    gluOrtho2D(0,window.width,window.height,0);
    //px=300; py=300;
    // pdx = cos(pa)*5; pdy = sin(pa)*5;
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
