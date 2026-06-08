#include <GL/glut.h>
#include <cstdlib>   
#include <cstdio>    

GLdouble px = 100, py = 50;
GLint ps = 10;
GLint d = 0;
GLdouble fx=320, fy=240;


int hit = 0;
char buffer[10];

void myInit() {
    glClearColor(1, 1, 1, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0, 640, 480, 0);
}

void playHitSound() {
    system("aplay hit.wav > /dev/null 2>&1 &");
}

void myspecial(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:    d = 3; break;
        case GLUT_KEY_DOWN:  d = 2; break;
        case GLUT_KEY_LEFT:  d = 1; break;
        case GLUT_KEY_RIGHT: d = 0; break;
    }
}

void randomizeFood() {
    fx = rand() % 620 ; 
    fy = rand() % 460 ; 
}

void MyIdle() {

    

    if (d == 0) {
        px += 0.1;
        if (px > 640) {
           
            
                  
            d = 1;
        }
    }
    else if (d == 1) {
        px -= 0.91;
        if (px < 0) {
           
            d = 0;
        }
    }
    else if (d == 2) {
        py += 0.91;
        if (py > 480) {
           
            d = 3;
        }
    }
    else if (d == 3) {
        py -= 0.91;
        if (py < 0) {
            
            d = 2;
        }
    }

    if((px+ps/2>=fx-5 && px-ps/2<=fx+5 && py+ps/2>=fy-5 && py-ps/2<=fy+5)) {
        hit+=5;
        playHitSound();
        randomizeFood();
        ps += 2; 
        px += 1;
        py += 1;
    }

    glutPostRedisplay();
}


void drawString(void *font, const char *str, int x, int y) {
    glRasterPos2i(x, y);

    for (int i = 0; str[i] != '\0'; i++) {
        glutBitmapCharacter(font, str[i]);
    }
}

void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1, 0, 0);

    glPointSize(ps);
    glBegin(GL_POINTS);
        glVertex2i(px, py);
    glEnd();

   
    glColor3f(1, 0, 0);
    drawString(GLUT_BITMAP_HELVETICA_12, "Col", 580, 20);

    sprintf(buffer, "%d", hit);
     glColor3f(0, 0, 0);
    drawString(GLUT_BITMAP_HELVETICA_18, buffer, 610, 20);
      glPointSize(10);
    glBegin(GL_POINTS);
  
        glColor3f(0, 1, 0);
        glVertex2i(fx, fy);
    glEnd();

    glutSwapBuffers();
}

int main(int argc, char *argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(640, 480);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("animation");

    myInit();

    glutDisplayFunc(myDisplay);
    glutIdleFunc(MyIdle);
    glutSpecialFunc(myspecial);

    glutMainLoop();
    return 0;
}