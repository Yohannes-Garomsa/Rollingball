#include <GL/glut.h>
#include <iostream>

GLint px = 100;
GLint py = 100;
GLint fx = 320;
GLint fy = 240;

// Step 1: Create a unified check function so ALL inputs trigger collisions
void checkCollision() {
    // Player size is 10 (from glPointSize(10)), so radius from center is 5
    if ((px + 5 >= fx - 5) && (px - 5 <= fx + 5) && 
        (py + 5 >= fy - 5) && (py - 5 <= fy + 5)) {
        std::cout << "Collision Detected! Exiting game...\n";
        exit(0);
    }
}

void myInit(){
    glClearColor(1, 1, 1, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Inverted coordinate plane: (0,0) is TOP-LEFT, (640,480) is BOTTOM-RIGHT
    gluOrtho2D(0, 640, 480, 0); 
}

void mykeyboard (unsigned char key, int x, int y){
   switch(key){
       case 'A': case 'a':
           if(px - 5 > 0) px -= 5;
           break;

       case 'D': case 'd':
           if(px + 5 < 640) px += 5;
           break;

       // CORRECTED FOR INVERTED Y-AXIS:
       case 'w': case 'W':
           if(py - 5 > 0) py -= 5; // Moves visually UP toward 0
           break;

       case 'x': case 'X':
           if(py + 5 < 480) py += 5; // Moves visually DOWN toward 480
           break;
   }

   checkCollision(); // Check right after normal keys move
   glutPostRedisplay();
}

void mySpecial(int key, int x, int y){
    switch(key){
    case GLUT_KEY_UP:
        if(py - 5 > 0) py -= 5; // Fixed boundary limit to match inverted plane
        break;

    case GLUT_KEY_DOWN:
        if(py + 5 < 480) py += 5;
        break;

    case GLUT_KEY_RIGHT:
        if(px + 5 < 640) px += 5;
        break;

    case GLUT_KEY_LEFT:
        if(px - 5 > 0) px -= 5;
        break;

    case GLUT_KEY_END:
        exit(0);
        break;
    }

    checkCollision(); // Check right after arrow keys move
    glutPostRedisplay();
}

void myMouse (int button, int state, int x, int y){
    glutPostRedisplay();
}

void myDisplay(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPointSize(10);
    glBegin(GL_POINTS);
        glColor3f(1, 0, 0); // Red Player
        glVertex2i(px, py);
        
        glColor3f(0, 1, 0); // Green Food
        glVertex2i(fx, fy);
    glEnd();

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowPosition(10, 10);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutCreateWindow("Keyboard Listener");

    myInit();

    glutKeyboardFunc(mykeyboard);
    glutSpecialFunc(mySpecial);
    glutMouseFunc(myMouse);
    glutDisplayFunc(myDisplay);

    glutMainLoop();
    return 0;
}