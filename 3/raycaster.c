#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>


float px,py;

void drawPlayer()
{
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px,py);
    glEnd();
}


int mapX = 8, mapY = 8, mapS = 64;
int map[]=           //the map array. Edit to change level but keep the outer walls
{
 1,1,1,1,1,1,1,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,0,0,0,0,0,1,
 1,0,0,0,0,1,0,1,
 1,0,0,0,0,0,0,1,
 1,1,1,1,1,1,1,1,	
};

void drawMap2D() 
{
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
            if( map[idx] == 1 )
            { glColor3f(1,1,1); }
            else 
            { glColor3f(0,0,0); }

            //printf("x*mapS: %d   - y*mapS: %d\n",x*mapS,y*mapS);
            x0 = x*mapS;
            y0 = y*mapS;

            glBegin(GL_QUADS);
            glVertex2i(x0      +1 , y0         +1);
            glVertex2i(x0      +1 , y0 + mapS  -1);
            glVertex2i(x0+mapS -1 , y0 + mapS  -1);
            glVertex2i(x0+mapS -1 , y0         +1);
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
    if(key=='a'){px-=5;}
    if(key=='d'){px+=5;}
    if(key=='w'){py-=5;}
    if(key=='s'){py+=5;}
    glutPostRedisplay();
}


void init()
{
    glClearColor(0.3,0.3,0.3,0);
    gluOrtho2D(0,1024,512,0);
    px=300; py=300;
}

int main(int argc, char* argv[])
{
    printf("environment validation\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1024, 512);
    glutCreateWindow("test glut");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();

}


//install glut: sudo apt-get install freeglut3-dev
//compile on linux
//use: gcc raycaster.c -lGL -lGLU -lglut -lm
