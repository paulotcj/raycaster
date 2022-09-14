#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT   );
    glutSwapBuffers();
}


void init()
{
    glClearColor(0.3,0.3,0.3,0);
    gluOrtho2D(0,1024,512,0);
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
    glutMainLoop();

}


//install glut: sudo apt-get install freeglut3-dev
//compile on linux
//use: gcc raycaster.c -lGL -lGLU -lglut -lm
