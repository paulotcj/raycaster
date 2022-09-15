#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533

float px,py;//player pos x , pos y
float pdx, pdy; //player deltas
float pa; //player angle

void drawPlayer()
{
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px,py);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px          , py         );
    glVertex2i(px + pdx*5  , py + pdy*5 );
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


float dist(float ax, float ay, float bx, float by, float ang)
{
    return ( sqrt( (bx-ax) * (bx-ax) + (by-ay) * (by-ay) )  );
}




void drawRays2D()
{
    int r, mx, my, mp, dof;
    float rx, ry, ra, x0, y0, disT;
    ra=pa-DR*30;
    if( ra < 0    ){ ra+= 2*PI; }
    if( ra > 2*PI ){ ra-= 2*PI; }

    for(r = 0 ; r < 60 ; r++ )
    {

        //check horizontal lines
        dof = 0;
        float disH = 1000000, hx = px , hy = py;
        float aTan = -1 / tan(ra);
        // num >> 6 = divide by 64    and   num << 6 = multiply by 64
        if(ra > PI){ ry = ( ((int)py >> 6) << 6 ) - 0.0001; rx = (py - ry) * aTan + px; y0 = -64; x0 = -y0*aTan;  } //looking up
        if(ra < PI){ ry = ( ((int)py >> 6) << 6 ) + 64;     rx = (py - ry) * aTan + px; y0 =  64; x0 = -y0*aTan;  } //looking down
        if(ra == 0 || ra == PI){ rx = px; ry = py; dof = 8; } //looking straight left or right
        
        while(dof < 8) //this is max depth of the map - we do not need to check further if that's the case
        {
            mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my*mapX + mx;  
            if(mp > 0 && mp < mapX * mapY && map[mp] == 1){ hx = rx; hy = ry; disH = dist(px,py,hx,hy,ra); dof = 8;} //hit wall
            else{ rx += x0; ry += y0; dof += 1; } //next line
        }

        // glColor3f(0,1,0);
        // glLineWidth(6);
        // glBegin(GL_LINES);
        // glVertex2i(px, py);
        // glVertex2i(rx, ry);
        // glEnd();


        //check vertical lines
        dof = 0;
        float disV = 1000000, vx = px , vy = py;
        float nTan = -tan(ra); //negative tangent
        // num >> 6 = divide by 64    and   num << 6 = multiply by 64
        if( ra > P2 && ra <  P3 ){ rx = ( ((int)px >> 6) << 6 ) - 0.0001; ry = (px - rx) * nTan + py; x0 = -64; y0 = -x0*nTan;  } //looking left
        if( ra < P2 || ra >  P3 ){ rx = ( ((int)px >> 6) << 6 ) + 64;     ry = (px - rx) * nTan + py; x0 =  64; y0 = -x0*nTan;  } //looking right
        if( ra == 0 || ra == PI ){ rx = px; ry = py; dof = 8; } //looking straight up or down
        
        while(dof < 8) //this is max depth of the map - we do not need to check further if that's the case
        {
            mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my*mapX + mx;  
            if(mp > 0 && mp < mapX * mapY && map[mp] == 1){ vx = rx; vy = ry; disV = dist(px,py,vx,vy,ra); dof = 8;} //hit wall
            else{ rx += x0; ry += y0; dof += 1; } //next line
        }

        if(disV < disH){ rx = vx; ry = vy; disT = disV; glColor3f(0.9,0,0);} //vertical wall hit
        if(disH < disV){ rx = hx; ry = hy; disT = disH; glColor3f(0.7,0,0);} //horizontal wall hit


        glLineWidth(3);
        glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(rx, ry);
        glEnd();

        //Draw 3D - start
        float ca = pa-ra;               //fix fisheye - start
        if( ca < 0    ){ ca+= 2*PI; }
        if( ca > 2*PI ){ ca-= 2*PI; }
        disT = disT * cos(ca);          //fix fisheye - end

        float lineH = (mapS * 320 )/disT;
        if(lineH > 320){ lineH = 320;} //line height
        float lineO = 160 - lineH/2;   //line offset
        glLineWidth(8);
        glBegin(GL_LINES);
        glVertex2i(r*8 + 530, lineO         );
        glVertex2i(r*8 + 530, lineO + lineH );
        glEnd();

        //Draw 3D - end

        ra+=DR;
        if( ra < 0    ){ ra+= 2*PI; }
        if( ra > 2*PI ){ ra-= 2*PI; }
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
    //pa => player angle

    
    if(key=='a'){ pa -= 0.1; if( pa < 0    ){ pa += 2*PI; }  pdx = cos(pa)*5; pdy = sin(pa)*5; }
    if(key=='d'){ pa += 0.1; if( pa > 2*PI ){ pa -= 2*PI; }  pdx = cos(pa)*5; pdy = sin(pa)*5; }
    if(key=='w'){ px += pdx; py += pdy; }
    if(key=='s'){ px -= pdx; py -= pdy; }

    glutPostRedisplay();
}


void init()
{
    glClearColor(0.3,0.3,0.3,0);
    gluOrtho2D(0,1024,512,0);
    px=300; py=300;
    pdx = cos(pa)*5; pdy = sin(pa)*5;
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
