#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>  // Added for strlen
#include <stdio.h>   // Added for sprintf
#include <stdlib.h>  // Added for exit

GLdouble px = 100;
GLdouble py = 50;
GLdouble fx = 320;
GLdouble fy = 240;

GLint ps = 10;
GLint d = 0;
int hit = 0;
char buffer[10];


Mix_Chunk* hitSound = NULL;

#define TEXT(x) x
#define SND_ASYNC 1

void myInit() {
    glClearColor(1, 1, 1, 0); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 0 is Top, 480 is Bottom
    gluOrtho2D(0, 640, 480, 0);
}


void playSound(const char* soundName, int dummy1, int dummy2) {
    if (hitSound != NULL) {
        Mix_PlayChannel(-1, hitSound, 0); 
    }
}

void drawString(void *font, const char *s, float x, float y){
    unsigned int i;
    glPushAttrib(GL_CURRENT_BIT);
    glRasterPos2f(x, y);
    glColor3f(1, 0, 1);
    for (i = 0; i < strlen(s); i++){
        glutBitmapCharacter(font, s[i]);
    }
    glPopAttrib();
}

void mySpecial(int key, int x, int y){
    switch(key){
        case GLUT_KEY_UP:    d = 2; break;
        case GLUT_KEY_DOWN:  d = 3; break;
        case GLUT_KEY_LEFT:  d = 1; break;
        case GLUT_KEY_RIGHT: d = 0; break;
    }
    glutPostRedisplay();
}

void myIdeal(){
    if(d == 0)      { px += 3; } 
    else if(d == 1) { px -= 3; } 
    else if(d == 2) { py -= 3; } 
    else if(d == 3) { py += 3; } 
    if((px + ps/2) >= 640){
        playSound(TEXT("hit"), NULL, SND_ASYNC);
        hit = hit + 1;
        d = 1;
    }
    else if((px - ps/2) <= 0){
        playSound(TEXT("hit"), NULL, SND_ASYNC);
        hit = hit + 1;
        d = 0;
    }
    else if((py + ps/2) >= 480){
        playSound(TEXT("hit"), NULL, SND_ASYNC);
        hit = hit + 1;
        d = 2; 
    }
    else if((py - ps/2) <= 0){
        playSound(TEXT("hit"), NULL, SND_ASYNC);
        hit = hit + 1;
        d = 3; 
    }
    if((px + ps/2 >= fx - 5) && (px - ps/2 <= fx + 5) && (py + ps/2 >= fy - 5) && (py - ps/2 <= fy + 5)){
        exit(0);
    }
    glutPostRedisplay();
}
void mydisplay() {
    glClear(GL_COLOR_BUFFER_BIT); 

    glColor3f(1, 0, 0); 
    glPointSize(ps);
    glBegin(GL_POINTS);
    glVertex2d(px, py);
    glEnd();

    glColor3f(1, 1, 0); 
    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex2d(fx, fy);
    glEnd();
    drawString(GLUT_BITMAP_HELVETICA_12, "COL:", 540, 30);
    sprintf(buffer, "%d", hit); 
    drawString(GLUT_BITMAP_HELVETICA_12, buffer, 590, 30);
    glutSwapBuffers();
}
void cleanUp() {
    if (hitSound) Mix_FreeChunk(hitSound);
    Mix_CloseAudio();
    SDL_Quit();
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 480); 
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Animation");

    if (SDL_Init(SDL_INIT_AUDIO) < 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Audio system initialization failed!\n");
    } else {
        hitSound = Mix_LoadWAV("hit.wav");
        if (!hitSound) {
            printf("Warning: hit.wav not found in execution directory.\n");
        }
    }

    myInit();
    glutDisplayFunc(mydisplay);
    glutIdleFunc(myIdeal);
    glutSpecialFunc(mySpecial);
    
    atexit(cleanUp); 

    glutMainLoop();
    return 0;
}