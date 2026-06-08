#include <GL/glut.h>
#include <iostream>

GLint px=100;
GLint py=100;
GLint fx=320;
GLint fy=240;

void myInit(){
    glClearColor(1,1,1,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,640,480,0);
}

void mykeyboard (unsigned char key ,int x ,int y){

   if((px+5 >= fx-5) && (px-5 <= fx+5) && (py+5 >= fy-5) && (py-5 <= fy+5)){
       exit(0);
   }

   switch(key){
       case 'A': case 'a':
           if(px-5>0)
               px-=5;
           break;

       case 'D': case 'd':
           if(px+5<640)
               px+=5;
           break;

       case 'w': case 'W':
           if(py-5>0)
               py-=5;
           break;

       case 'x': case 'X':
           if(py+5<480)
               py+=5;
           break;
   }

   glutPostRedisplay();
}

void mySpecial(int key ,int x,int y){
    if((px+5 >= fx-5) && (px-5 <= fx+5) && (py+5 >= fy-5) && (py-5 <= fy+5)){
       exit(0);
   }
    switch(key){

    case GLUT_KEY_UP:
        if(py!=30)
            py-=5;
        break;

    case GLUT_KEY_RIGHT:
        px+=5;
        break;

    case GLUT_KEY_LEFT:
        px-=5;
        break;

    case GLUT_KEY_DOWN:
        py+=5;
        break;

    case GLUT_KEY_END:
        exit(0);
        break;
    }

    glutPostRedisplay();
}

void myMouse (int button , int state,int x,int y){
    if(button==GLUT_LEFT_BUTTON) && (state==GLUT_DOWN){
        exit(0);
    }

    glutPostRedisplay();
}

void myDisplay(){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1,0,0);
    glPointSize(10);

    glBegin(GL_POINTS);
        glVertex2i(px,py);
        glColor3f(0,1,0);
        glVertex2i(fx,fy);
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