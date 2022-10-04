#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

struct WindowProperties { 
    int height;
    int width;
    float backgroundColor4f[]; //red, green, blue, alpha
};
struct PlayerDetails {
    int pointSize;
    int px;
    int py;
    float playerColor3f[];
};

struct MapDetails {
    int width;
    int height;
    int tileSizePx;
    int map[];
};


struct WindowProperties window = { height : 512, width : 1024, backgroundColor4f : {0.3f,0.3f,0.3f,0} };
struct PlayerDetails playerDet = {pointSize : 8, px: 300, py: 300, playerColor3f : {1.0f,1.0f,0.0f}};
struct MapDetails mapDet = {map: {
                                    1,1,1,1,1,1,1,1, //the map array. Edit to change level but keep the outer walls
                                    1,0,1,0,0,0,0,1,
                                    1,0,1,0,0,0,0,1,
                                    1,0,1,0,0,0,0,1,
                                    1,0,0,0,0,0,0,1,
                                    1,0,0,0,0,1,0,1,
                                    1,0,0,0,0,0,0,1,
                                    1,1,1,1,1,1,1,1,	
                                    } , width : 8, height: 8, tileSizePx : 64};


//-----------------------------------------------



void drawPlayer()
{
    glColor3f(1,1,0);
    glPointSize(playerDet.pointSize);
    glBegin(GL_POINTS);
    glVertex2i(playerDet.px,playerDet.py);
    glEnd();
}


void drawMap2D() 
{
    int mapX = mapDet.width, mapY = mapDet.height, blockSize = mapDet.tileSizePx;
    const int lineThickness = 1;
    //the loop below executes mapY * mapX -> 8 * 8 = 64;
    int x,y,x0,y0;
    for(y = 0 ; y < mapY ; y++) //we are looping through a 1D array
    {
        for(x = 0 ; x < mapX ; x++)
        {
            //We are using a 1D array and making it behave as it was a 2D array of 8x8 (64 items) this could be: 
            // idx: 0    - y*mapX: 0 ,     x: 0 
            // idx: 1    - y*mapX: 0 ,     x: 1 
            // ...
            // idx: 7    - y*mapX: 0 ,     x: 7 
            // idx: 8    - y*mapX: 8 ,     x: 0 
            // idx: 9    - y*mapX: 8 ,     x: 1 
            // idx: 10   - y*mapX: 8 ,     x: 2 
            // ...
            // idx: 62   - y*mapX: 56 ,    x: 6 
            // idx: 63   - y*mapX: 56 ,    x: 7            

            //  e.g.: 3*8    + 2  -> 24+2 -> 26
            int idx = y*mapX + x;
            //printf("idx: %d   - y*mapX: %d ,    x: %d \n",idx, y*mapX, x);
            if( mapDet.map[idx] == 1 )
            { glColor3f(1,1,1); }
            else 
            { glColor3f(0,0,0); }

            //printf("x*blockSize: %d   - y*blockSize: %d\n",x*blockSize,y*blockSize);
            x0 = x*blockSize;
            y0 = y*blockSize;

            if(x0 == 0 && y0 == 0){glColor3f(0.8,0.8,0.8);} //mark the first 'tile'

            

            //printf("x0: %d, y0: %d \n", x0, y0);
            glBegin(GL_QUADS);
            glVertex2i(x0                             , y0                              );
            glVertex2i(x0                             , y0 + (blockSize -lineThickness) );
            glVertex2i(x0+ (blockSize -lineThickness) , y0 + (blockSize -lineThickness) );
            glVertex2i(x0+ (blockSize -lineThickness) , y0                              );
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
    if(key=='a'){playerDet.px-=5;}
    if(key=='d'){playerDet.px+=5;}
    if(key=='w'){playerDet.py-=5;}
    if(key=='s'){playerDet.py+=5;}
    glutPostRedisplay();
}


void init()
{
    glClearColor(window.backgroundColor4f[0],window.backgroundColor4f[1],window.backgroundColor4f[2],window.backgroundColor4f[3]); //red, green, blue, alpha
    gluOrtho2D(0,window.width,window.height,0);
    //px=300; py=300;
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
//use: gcc raycaster.c -lGL -lGLU -lglut -lm
//     ./a.out
