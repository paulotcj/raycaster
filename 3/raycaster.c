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
    int x,y,x0,y0;
    for(y = 0 ; y < mapY ; y++) //we are looping through a 1D array
    {
        for(x = 0 ; x < mapX ; x++)
        {
            //We are using a 1D array and making it behave as it was a 2D array of 8x8 (64 items)
            //this could be: 0*8 + 1  = 0  + 1 = 1
            //               2*8 + 5  = 16 + 5 = 21
            if( map[y*mapX + x] == 1 )
            { glColor3f(1,1,1); }
            else 
            { glColor3f(0,0,0); }

            x0 = x*mapS;
            y0 = y*mapS;

            glBegin(GL_QUADS);
            glVertex2i(x0      , y0        );
            glVertex2i(x0      , y0 + mapS );
            glVertex2i(x0+mapS , y0 + mapS );
            glVertex2i(x0+mapS , y0        );
            glEnd();

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
