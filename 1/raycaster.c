#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

struct WindowProperties { 
    int height;
    int width;
    float backgroundColor[]; //red, green, blue, alpha
};
struct WindowProperties window = { height : 512, width : 1024, backgroundColor : {0.3,0.3,0.3,0} };
//-----------------------------------------------


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT   );
    glutSwapBuffers();
}


void init()
{
    glClearColor(window.backgroundColor[0],window.backgroundColor[1],window.backgroundColor[2],window.backgroundColor[3]); //red, green, blue, alpha
    gluOrtho2D(0,window.width,window.height,0);
}

int main(int argc, char* argv[])
{
    printf("environment validation\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("test glut");
    init();
    glutDisplayFunc(display);
    glutMainLoop();

}


//install glut: sudo apt-get install freeglut3-dev
//compile on linux
//use: gcc raycaster.c -lGL -lGLU -lglut -lm
