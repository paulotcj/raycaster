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
    int directionVectorMultiplier;
    float playerColor3f[3];
};

struct ControlsDetails{
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

struct WindowProperties window = { .height = 512, .width = 1024, .backgroundColor4f = {0.3,0.3,0.3,0} };
struct PlayerDetails playerDet = { .pointSize = 8, .x = 300, .y = 300, .deltaX = 5, /* cos(0)*5 */
                                   .deltaY = 0,                                  /* sin(0)*5 */
                                   .angle = 0,
                                   .deltaMultiplier = 5,
                                   .playerColor3f = {1.0f, 0.0f, 1.0f},
                                   .directionVectorMultiplier = 5};

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
                            .lineThickness = 1 };

//-----------------------------------------------





void drawPlayer()
{
    // player dot
    glColor3f( playerDet.playerColor3f[0], playerDet.playerColor3f[1], playerDet.playerColor3f[2]);
    glPointSize(playerDet.pointSize);
    glBegin(GL_POINTS);
    glVertex2i(playerDet.x, playerDet.y);
    glEnd();

    // direction
    int x1 = playerDet.x + playerDet.deltaX * playerDet.directionVectorMultiplier;
    int y1 = playerDet.y + playerDet.deltaY * playerDet.directionVectorMultiplier;
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
    int x, y, x0, y0;
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
            x0 = x * blockSize;
            y0 = y * blockSize;

            // mark the first tile
            if (x0 == 0 && y0 == 0)
            { glColor3f(mapDet.firstTileColor3f[0], mapDet.firstTileColor3f[1], mapDet.firstTileColor3f[2]); }

            // printf("x0: %d, y0: %d \n", x0, y0);
            glBegin(GL_QUADS);
            glVertex2i(x0,                                      y0                                     );
            glVertex2i(x0,                                      y0 + (blockSize - mapDet.lineThickness));
            glVertex2i(x0 + (blockSize - mapDet.lineThickness), y0 + (blockSize - mapDet.lineThickness));
            glVertex2i(x0 + (blockSize - mapDet.lineThickness), y0                                     );
            glEnd();

            // X      , Y
            // 0      , 0
            // 0      , 0 + 64
            // 0 + 64 , 0 + 64
            // 0 + 64 , 0

            //getchar();

        }
    }
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT   );
    drawMap2D();
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

    printf("pa: %f - px: %f - py: %f - pdx: %f - pdy: %f \n", playerDet.angle, playerDet.x, playerDet.y, playerDet.deltaX, playerDet.deltaY);

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
