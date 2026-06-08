#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 640
#define HEIGHT 480

#define MAX_SNAKE 100
#define SNAKE_SIZE 10
int snakeX[MAX_SNAKE];
int snakeY[MAX_SNAKE];

int snakeLength = 5;

// 0 = RIGHT
// 1 = LEFT
// 2 = UP
// 3 = DOWN

int d = 0;
int foodX = 300;
int foodY = 200;
int score = 0;

int gameOver = 0;

int paused = 0;

char buffer[100];

Mix_Chunk* eatSound = NULL;
void myInit()
{
    glClearColor(0.02f, 0.02f, 0.05f, 1);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluOrtho2D(0, WIDTH, HEIGHT, 0);
}

void drawString(
    void *font,
    const char *text,
    float x,
    float y)
{
    glRasterPos2f(x, y);

    for(unsigned int i = 0; i < strlen(text); i++)
    {
        glutBitmapCharacter(font, text[i]);
    }
}
void playSound()
{
    if(eatSound != NULL)
    {
        Mix_PlayChannel(-1, eatSound, 0);
    }
}
void randomFood()
{
    foodX =
        (rand() % (WIDTH / SNAKE_SIZE))
        * SNAKE_SIZE;

    foodY =
        (rand() % (HEIGHT / SNAKE_SIZE))
        * SNAKE_SIZE;
}
void resetGame()
{
    snakeLength = 5;

    score = 0;

    gameOver = 0;

    paused = 0;

    d = 0;

    for(int i = 0; i < snakeLength; i++)
    {
        snakeX[i] =
            100 - (i * SNAKE_SIZE);

        snakeY[i] = 100;
    }

    randomFood();
}
void mySpecial(int key, int x, int y)
{
    if(gameOver || paused)
        return;

    switch(key)
    {
        case GLUT_KEY_RIGHT:
            if(d != 1)
                d = 0;
            break;
        case GLUT_KEY_LEFT:

            if(d != 0)
                d = 1;
            break;
        case GLUT_KEY_UP:
            if(d != 3)
                d = 2;
            break;
        case GLUT_KEY_DOWN:
            if(d != 2)
                d = 3;
            break;
    }
} 

void myKeyboard(
    unsigned char key,
    int x,
    int y)
{
    switch(key)
    {
        case 'p':
        case 'P':

            if(!gameOver)
            {
                paused = !paused;
            }

            break;
        case 'r':
        case 'R':
            resetGame();
            break;
        case 27:
            exit(0);
            break;
    }

    glutPostRedisplay();
}
void update()
{
    // ==========================================
    // PAUSED
    // ==========================================

    if(paused)
    {
        glutPostRedisplay();
        return;
    }

    if(gameOver)
    {
        glutPostRedisplay();
        return;
    }

    // ==========================================
    // MOVE BODY
    // ==========================================

    for(int i = snakeLength - 1; i > 0; i--)
    {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // ==========================================
    // MOVE HEAD
    // ==========================================

    if(d == 0)
        snakeX[0] += SNAKE_SIZE;

    else if(d == 1)
        snakeX[0] -= SNAKE_SIZE;

    else if(d == 2)
        snakeY[0] -= SNAKE_SIZE;

    else if(d == 3)
        snakeY[0] += SNAKE_SIZE;

    // ==========================================
    // WALL COLLISION
    // ==========================================

    if(snakeX[0] < 0 ||
       snakeX[0] >= WIDTH ||
       snakeY[0] < 0 ||
       snakeY[0] >= HEIGHT)
    {
        gameOver = 1;
    }

    // ==========================================
    // SELF COLLISION
    // ==========================================

    for(int i = 1; i < snakeLength; i++)
    {
        if(snakeX[0] == snakeX[i] &&
           snakeY[0] == snakeY[i])
        {
            gameOver = 1;
        }
    }
    if(snakeX[0] == foodX &&
       snakeY[0] == foodY)
    {
        playSound();

        score += 10;

        if(snakeLength < MAX_SNAKE)
            snakeLength++;

        randomFood();
    }

    glutPostRedisplay();
}
void timer(int value)
{
    update();

    glutTimerFunc(100, timer, 0);
}

void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // ==================================================
    // BACKGROUND GRID
    // ==================================================

    glColor3f(0.08f, 0.08f, 0.12f);

    glBegin(GL_LINES);

    for(int i = 0; i < WIDTH; i += 20)
    {
        glVertex2f(i, 0);
        glVertex2f(i, HEIGHT);
    }

    for(int i = 0; i < HEIGHT; i += 20)
    {
        glVertex2f(0, i);
        glVertex2f(WIDTH, i);
    }

    glEnd();
    glColor3f(1, 0.2f, 0.2f);

    glBegin(GL_QUADS);

        glVertex2f(foodX, foodY);
        glVertex2f(foodX + SNAKE_SIZE, foodY);
        glVertex2f(foodX + SNAKE_SIZE, foodY + SNAKE_SIZE);
        glVertex2f(foodX, foodY + SNAKE_SIZE);

    glEnd();

    for(int i = 0; i < snakeLength; i++)
    {
        // HEAD
        if(i == 0)
        {
            glColor3f(0.0f, 0.5f, 1.0f);
        }
        else
        {
            // BODY
            if(i % 2 == 0)
                glColor3f(0.0f, 0.9f, 1.0f);
            else
                glColor3f(0.0f, 0.7f, 0.9f);
        }

        glBegin(GL_QUADS);

            glVertex2f(snakeX[i], snakeY[i]);
            glVertex2f(snakeX[i] + SNAKE_SIZE, snakeY[i]);
            glVertex2f(snakeX[i] + SNAKE_SIZE, snakeY[i] + SNAKE_SIZE);
            glVertex2f(snakeX[i], snakeY[i] + SNAKE_SIZE);

        glEnd();

        // BORDER
        glColor3f(0, 0, 0);

        glLineWidth(1.5f);

        glBegin(GL_LINE_LOOP);

            glVertex2f(snakeX[i], snakeY[i]);
            glVertex2f(snakeX[i] + SNAKE_SIZE, snakeY[i]);
            glVertex2f(snakeX[i] + SNAKE_SIZE, snakeY[i] + SNAKE_SIZE);
            glVertex2f(snakeX[i], snakeY[i] + SNAKE_SIZE);

        glEnd();
    }
    glColor3f(1, 1, 1);

    sprintf(buffer, "Score : %d", score);

    drawString(
        GLUT_BITMAP_HELVETICA_18,
        buffer,
        10,
        20);
    glColor3f(0.8f, 0.8f, 0.8f);

    drawString(
        GLUT_BITMAP_HELVETICA_12,
        "Arrow Keys = Move | P = Pause | R = Restart | ESC = Exit",
        80,
        HEIGHT - 10);
    if(paused)
    {
        glColor3f(1, 1, 0);

        drawString(
            GLUT_BITMAP_TIMES_ROMAN_24,
            "GAME PAUSED",
            WIDTH / 2 - 80,
            HEIGHT / 2);

        glColor3f(1, 1, 1);

        drawString(
            GLUT_BITMAP_HELVETICA_18,
            "Press P to Resume",
            WIDTH / 2 - 70,
            HEIGHT / 2 + 40);
    }
    if(gameOver)
    {
        glColor3f(1, 0, 0);

        drawString(
            GLUT_BITMAP_TIMES_ROMAN_24,
            "GAME OVER",
            WIDTH / 2 - 70,
            HEIGHT / 2);

        glColor3f(1, 1, 1);

        drawString(
            GLUT_BITMAP_HELVETICA_18,
            "Press R to Restart",
            WIDTH / 2 - 70,
            HEIGHT / 2 + 40);
    }

    glutSwapBuffers();
}
void cleanUp()
{
    if(eatSound)
        Mix_FreeChunk(eatSound);

    Mix_CloseAudio();

    SDL_Quit();
}
int main(int argc, char** argv)
{
    srand(time(0));

    resetGame();

    // ==================================================
    // GLUT
    // ==================================================

    glutInit(&argc, argv);

    glutInitDisplayMode(
        GLUT_DOUBLE |
        GLUT_RGB);

    glutInitWindowSize(
        WIDTH,
        HEIGHT);

    glutInitWindowPosition(
        100,
        100);

    glutCreateWindow(
        "Snake Game");

    // ==================================================
    // SDL AUDIO
    // ==================================================

    if(SDL_Init(SDL_INIT_AUDIO) < 0 ||
       Mix_OpenAudio(
           44100,
           MIX_DEFAULT_FORMAT,
           2,
           2048) < 0)
    {
        printf("Audio initialization failed!\n");
    }
    else
    {
        eatSound = Mix_LoadWAV("hit.wav");

        if(!eatSound)
        {
            printf("hit.wav not found!\n");
        }
    }

  
    myInit();
    glutDisplayFunc(myDisplay);
    glutSpecialFunc(mySpecial);
    glutKeyboardFunc(myKeyboard);
    glutTimerFunc(100, timer, 0);
    atexit(cleanUp);
    glutMainLoop();

    return 0;
}