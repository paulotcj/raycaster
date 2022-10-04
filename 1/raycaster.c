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


struct WindowProperties window = { .height = 512, .width = 1024, .backgroundColor4f = {0.3,0.3,0.3,0} };
//-----------------------------------------------








void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT   );
    glutSwapBuffers();
}



void init()
{
    glClearColor(window.backgroundColor4f[0],window.backgroundColor4f[1],window.backgroundColor4f[2],window.backgroundColor4f[3]); //red, green, blue, alpha
    gluOrtho2D(0,window.width,window.height,0);
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
    glutMainLoop();

}


//install glut: sudo apt-get install freeglut3-dev
//compile on linux
// use: gcc raycaster.c -lGL -lGLU -lglut -lm
//      ./a.out
//compile on linux

// use: gcc raycaster.c -framework OpenGL -framework GLUT
//      ./a.out