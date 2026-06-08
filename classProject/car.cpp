#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <cmath>

// =========================
// CAR
// =========================

GLdouble carX = 1000;
GLdouble carY = 1000;

GLdouble vx = 0;
GLdouble vy = 0;

GLdouble angle = 0;

GLdouble acceleration = 0.2;
GLdouble friction = 0.98;

// =========================
// CAMERA
// =========================

GLdouble cameraX = 0;
GLdouble cameraY = 0;

// =========================
// WORLD
// =========================

const int WORLD_WIDTH = 3000;
const int WORLD_HEIGHT = 3000;

// =========================
// AUDIO
// =========================

Mix_Chunk* hitSound = nullptr;

void playHit() {

    if(hitSound)
        Mix_PlayChannel(-1, hitSound, 0);
}

// =========================
// INIT
// =========================

void init() {

    glClearColor(0.3, 0.7, 0.3, 1);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluOrtho2D(0, 800, 600, 0);
}

// =========================
// INPUT
// =========================

void keyboard(unsigned char key, int x, int y) {

    // ACCELERATION

    if(key == 'w') {

        vx += cos(angle) * acceleration;

        vy += sin(angle) * acceleration;
    }

    // BRAKE

    if(key == 's') {

        vx -= cos(angle) * acceleration;

        vy -= sin(angle) * acceleration;
    }

    // LEFT

    if(key == 'a') {

        angle -= 0.08;
    }

    // RIGHT

    if(key == 'd') {

        angle += 0.08;
    }
}

// =========================
// UPDATE
// =========================

void update() {

    // MOVE CAR

    carX += vx;
    carY += vy;

    // FRICTION

    vx *= friction;
    vy *= friction;

    // WORLD BOUNDS

    if(carX < 0) {

        carX = 0;
        vx *= -0.5;

        playHit();
    }

    if(carX > WORLD_WIDTH) {

        carX = WORLD_WIDTH;
        vx *= -0.5;

        playHit();
    }

    if(carY < 0) {

        carY = 0;
        vy *= -0.5;

        playHit();
    }

    if(carY > WORLD_HEIGHT) {

        carY = WORLD_HEIGHT;
        vy *= -0.5;

        playHit();
    }

    // CAMERA FOLLOW

    cameraX = carX - 400;
    cameraY = carY - 300;

    glutPostRedisplay();
}

// =========================
// DRAW GRID WORLD
// =========================

void drawWorld() {

    // GRASS

    glColor3f(0.3, 0.7, 0.3);

    glBegin(GL_QUADS);

        glVertex2f(0,0);
        glVertex2f(WORLD_WIDTH,0);
        glVertex2f(WORLD_WIDTH,WORLD_HEIGHT);
        glVertex2f(0,WORLD_HEIGHT);

    glEnd();

    // GRID

    glColor3f(0.2,0.5,0.2);

    for(int x=0; x<WORLD_WIDTH; x+=100) {

        glBegin(GL_LINES);

            glVertex2f(x,0);
            glVertex2f(x,WORLD_HEIGHT);

        glEnd();
    }

    for(int y=0; y<WORLD_HEIGHT; y+=100) {

        glBegin(GL_LINES);

            glVertex2f(0,y);
            glVertex2f(WORLD_WIDTH,y);

        glEnd();
    }

    // ROAD

    glColor3f(0.2,0.2,0.2);

    glBegin(GL_QUADS);

        glVertex2f(200,1200);
        glVertex2f(2800,1200);
        glVertex2f(2800,1800);
        glVertex2f(200,1800);

    glEnd();
}

// =========================
// DRAW CAR
// =========================

void drawCar() {

    glPushMatrix();

    glTranslatef(carX, carY, 0);

    glRotatef(angle * 57.2958, 0, 0, 1);

    // BODY

    glColor3f(1,0,0);

    glBegin(GL_QUADS);

        glVertex2f(-20,-10);
        glVertex2f( 20,-10);
        glVertex2f( 20, 10);
        glVertex2f(-20, 10);

    glEnd();

    // FRONT

    glColor3f(1,1,0);

    glBegin(GL_QUADS);

        glVertex2f(10,-5);
        glVertex2f(20,-5);
        glVertex2f(20, 5);
        glVertex2f(10, 5);

    glEnd();

    glPopMatrix();
}

// =========================
// DISPLAY
// =========================

void display() {

    glClear(GL_COLOR_BUFFER_BIT);

    // CAMERA TRANSFORM

    glPushMatrix();

    glTranslatef(-cameraX, -cameraY, 0);

    drawWorld();

    drawCar();

    glPopMatrix();

    glutSwapBuffers();
}

// =========================
// MAIN
// =========================

int main(int argc, char** argv) {

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(800,600);

    glutCreateWindow("Professional Racing Engine");

    init();

    // AUDIO

    SDL_Init(SDL_INIT_AUDIO);

    Mix_OpenAudio(44100,
                  MIX_DEFAULT_FORMAT,
                  2,
                  2048);

    hitSound = Mix_LoadWAV("hit.wav");

    // GLUT

    glutDisplayFunc(display);

    glutIdleFunc(update);

    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;
}