#include<GL/glut.h>

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 640, 480, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(100, 150);
        glVertex2f(150, 250);
        glVertex2f(200, 250);
        glVertex2f(250, 150);
        glVertex2f(200, 50);
        glVertex2f(150, 50);
    glEnd();

        
   
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("OpenGL Polygon");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}