#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// ======================================================
// WINDOW & ARCHITECTURE DIMENSIONS
// ======================================================
#define WIDTH 640
#define HEIGHT 480
#define UI_HEADER_HEIGHT 40  // Reserved space at top for UI metrics

// ======================================================
// SNAKE
// ======================================================
#define SIZE 10
#define MAX_SNAKE 500

int snakeX[MAX_SNAKE];
int snakeY[MAX_SNAKE];

int snakeLength = 5;

// ======================================================
// FOOD
// ======================================================
int foodX;
int foodY;

// ======================================================
// GAME VARIABLES
// ======================================================
int dir = 0; // 0 RIGHT, 1 LEFT, 2 UP, 3 DOWN

int score = 0;
int highScore = 0;

int level = 1;
int speed = 120;

int gameOver = 0;
int paused = 0;
int gameStarted = 0;

// ======================================================
// MOUSE ENGINE INPUTS
// ======================================================
int mouseX = 0;
int mouseY = 0;

// ======================================================
// SOUND
// ======================================================
Mix_Chunk* hitSound = NULL;
Mix_Chunk* eatSound = NULL;

// ======================================================
// TEXT BUFFER
// ======================================================
char buffer[100];

// ======================================================
// DRAW TEXT
// ======================================================
void drawString(void* font, const char* text, float x, float y)
{
    glRasterPos2f(x, y);
    for(size_t i = 0; i < strlen(text); i++)
    {
        glutBitmapCharacter(font, text[i]);
    }
}

// ======================================================
// PLAY SOUND
// ======================================================
void playSound(Mix_Chunk* sound)
{
    if(sound != NULL)
    {
        Mix_PlayChannel(-1, sound, 0);
    }
}

// ======================================================
// INITIALIZATION
// ======================================================
void myInit()
{
    glClearColor(0.03f, 0.05f, 0.08f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
}

// ======================================================
// HIGH SCORE SYSTEM
// ======================================================
void loadHighScore()
{
    FILE* file = fopen("highscore.txt", "r");
    if(file != NULL)
    {
        if (fscanf(file, "%d", &highScore) != 1) {
            highScore = 0;
        }
        fclose(file);
    }
}

void saveHighScore()
{
    FILE* file = fopen("highscore.txt", "w");
    if(file != NULL)
    {
        fprintf(file, "%d", highScore);
        fclose(file);
    }
}

// ======================================================
// BOUNDED FOOD GENERATION
// ======================================================
void generateFood()
{
    foodX = (rand() % ((WIDTH - (SIZE * 2)) / SIZE)) * SIZE + SIZE;
    foodY = (rand() % ((HEIGHT - UI_HEADER_HEIGHT - (SIZE * 2)) / SIZE)) * SIZE + UI_HEADER_HEIGHT + SIZE;
}

// ======================================================
// RESET GAME ENGINE STATE
// ======================================================
void resetGame()
{
    snakeLength = 5;
    for(int i = 0; i < snakeLength; i++)
    {
        snakeX[i] = 100 - (i * SIZE);
        snakeY[i] = 100 + UI_HEADER_HEIGHT;
    }

    dir = 0;
    score = 0;
    level = 1;
    speed = 120;
    gameOver = 0;
    paused = 0;

    generateFood();
}

// ======================================================
// ADVANCED TECHNICAL BACKGROUND RENDERING
// ======================================================
void drawSeniorBackground()
{
    glBegin(GL_LINES);
    // Vertical Grid Pass
    for (int x = 0; x <= WIDTH; x += SIZE) 
    {
        if (x % (SIZE * 5) == 0) glColor3f(0.08f, 0.18f, 0.30f);
        else                     glColor3f(0.04f, 0.08f, 0.15f);
        glVertex2i(x, UI_HEADER_HEIGHT);
        glVertex2i(x, HEIGHT);
    }
    // Horizontal Grid Pass
    for (int y = UI_HEADER_HEIGHT; y <= HEIGHT; y += SIZE) 
    {
        if ((y - UI_HEADER_HEIGHT) % (SIZE * 5) == 0) glColor3f(0.08f, 0.18f, 0.30f);
        else                                         glColor3f(0.04f, 0.08f, 0.15f);
        glVertex2i(0, y);
        glVertex2i(WIDTH, y);
    }
    glEnd();

    // Structural UI Panel Block
    glColor3f(0.05f, 0.09f, 0.14f);
    glBegin(GL_POLYGON);
        glVertex2i(0, 0);
        glVertex2i(WIDTH, 0);
        glVertex2i(WIDTH, UI_HEADER_HEIGHT);
        glVertex2i(0, UI_HEADER_HEIGHT);
    glEnd();

    // High-Tech Neon Framing
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.7f, 0.9f);
    glBegin(GL_LINE_LOOP);
        glVertex2i(2, UI_HEADER_HEIGHT);
        glVertex2i(WIDTH - 2, UI_HEADER_HEIGHT);
        glVertex2i(WIDTH - 2, HEIGHT - 2);
        glVertex2i(2, HEIGHT - 2);
    glEnd();
    glLineWidth(1.0f);
}

// ======================================================
// REFACTORED BUTTON ROUTINE (Includes interactive hover context)
// ======================================================
void drawButton(int x, int y, int w, int h, const char* text, float r, float g, float b)
{
    int isHovered = (mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h);
    
    if(isHovered) {
        glColor3f(r + 0.15f, g + 0.15f, b + 0.15f); // Brighten up on hover
    } else {
        glColor3f(r, g, b);
    }

    glBegin(GL_POLYGON);
        glVertex2i(x, y);
        glVertex2i(x + w, y);
        glVertex2i(x + w, y + h);
        glVertex2i(x, y + h);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2i(x, y);
        glVertex2i(x + w, y);
        glVertex2i(x + w, y + h);
        glVertex2i(x, y + h);
    glEnd();

    drawString(GLUT_BITMAP_HELVETICA_18, text, x + 25, y + 32);
}

// ======================================================
// DRAW SNAKE
// ======================================================
void drawSnake()
{
    for(int i = 0; i < snakeLength; i++)
    {
        if(i == 0) glColor3f(0.6f, 0.2f, 1.0f); // Neon Purple Head
        else       glColor3f(0.9f, 0.1f, 0.6f); // Vivid Magenta Segments

        glBegin(GL_POLYGON);
            glVertex2i(snakeX[i], snakeY[i]);
            glVertex2i(snakeX[i] + SIZE, snakeY[i]);
            glVertex2i(snakeX[i] + SIZE, snakeY[i] + SIZE);
            glVertex2i(snakeX[i], snakeY[i] + SIZE);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f); 
        glBegin(GL_LINE_LOOP);
            glVertex2i(snakeX[i], snakeY[i]);
            glVertex2i(snakeX[i] + SIZE, snakeY[i]);
            glVertex2i(snakeX[i] + SIZE, snakeY[i] + SIZE);
            glVertex2i(snakeX[i], snakeY[i] + SIZE);
        glEnd();
    }
}

// ======================================================
// DRAW FOOD
// ======================================================
void drawFood()
{
    glColor3f(1.0f, 0.2f, 0.2f);
    glBegin(GL_POLYGON);
        glVertex2i(foodX, foodY);
        glVertex2i(foodX + SIZE, foodY);
        glVertex2i(foodX + SIZE, foodY + SIZE);
        glVertex2i(foodX, foodY + SIZE);
    glEnd();
}

// ======================================================
// STATE CONTROLLERS
// ======================================================
void updateLevel()
{
    level = (score / 5) + 1;
    speed = 120 - ((level - 1) * 10);
    if(speed < 40) speed = 40;
}

void checkCollision()
{
    if(snakeX[0] < 0 || snakeX[0] >= WIDTH || snakeY[0] < UI_HEADER_HEIGHT || snakeY[0] >= HEIGHT)
    {
        playSound(hitSound);
        gameOver = 1;
    }

    for(int i = 1; i < snakeLength; i++)
    {
        if(snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i])
        {
            playSound(hitSound);
            gameOver = 1;
        }
    }

    if(score > highScore)
    {
        highScore = score;
        saveHighScore();
    }
}

void moveSnake()
{
    // Senior Fix: Mouse steering logic is completely removed here.
    // Movement is now controlled strictly via standard key inputs.

    for(int i = snakeLength - 1; i > 0; i--)
    {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    if(dir == 0) snakeX[0] += SIZE;
    if(dir == 1) snakeX[0] -= SIZE;
    if(dir == 2) snakeY[0] -= SIZE;
    if(dir == 3) snakeY[0] += SIZE;

    if(snakeX[0] == foodX && snakeY[0] == foodY)
    {
        playSound(eatSound);
        score++;
        snakeLength++;
        updateLevel();
        generateFood();
    }

    checkCollision();
}

// ======================================================
// MENUS & INTERFACES
// ======================================================
void drawStartScreen()
{
    drawSeniorBackground();
    glColor3f(0, 1, 1);
    drawString(GLUT_BITMAP_TIMES_ROMAN_24, "ADVANCED SNAKE GAME", 190, 120);

    drawButton(220, 190, 200, 50, "START GAME", 0.1f, 0.5f, 0.1f);
    drawButton(220, 270, 200, 50, "EXIT", 0.7f, 0.1f, 0.1f);

    glColor3f(1,1,1);
    // Fixed font constant bug to prevent compilation failures
    drawString(GLUT_BITMAP_HELVETICA_12, "Use Keyboard Arrow Keys to Navigate the Arena", 175, 370);
    drawString(GLUT_BITMAP_HELVETICA_12, "Press 'P' to Pause / Mouse Support for all Menus", 170, 400);
}

void drawPauseScreen()
{
    glColor3f(1,1,0);
    drawString(GLUT_BITMAP_TIMES_ROMAN_24, "GAME PAUSED", 220, 150);

    drawButton(220, 220, 200, 50, "RESUME", 0.1f, 0.4f, 0.8f);
    drawButton(220, 300, 200, 50, "EXIT", 0.8f, 0.1f, 0.1f);
}

void drawGameOver()
{
    glColor3f(1,0,0);
    drawString(GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER", 240, 120);

    sprintf(buffer, "Final Score : %d", score);
    drawString(GLUT_BITMAP_HELVETICA_18, buffer, 240, 170);

    sprintf(buffer, "High Score : %d", highScore);
    drawString(GLUT_BITMAP_HELVETICA_18, buffer, 240, 200);

    drawButton(220, 260, 200, 50, "RESTART", 0.1f, 0.5f, 0.1f);
    drawButton(220, 340, 200, 50, "EXIT", 0.7f, 0.1f, 0.1f);
}

// ======================================================
// DISPLAY MANAGER
// ======================================================
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if(!gameStarted)
    {
        drawStartScreen();
        glutSwapBuffers();
        return;
    }

    drawSeniorBackground();

    if(!gameOver)
    {
        drawSnake();
        drawFood();

        glColor3f(0.0f, 0.9f, 1.0f);
        sprintf(buffer, "SCORE: %03d", score);
        drawString(GLUT_BITMAP_HELVETICA_18, buffer, 30, 25);

        sprintf(buffer, "LEVEL: %02d", level);
        drawString(GLUT_BITMAP_HELVETICA_18, buffer, 260, 25);

        sprintf(buffer, "HIGH SCORE: %03d", highScore);
        drawString(GLUT_BITMAP_HELVETICA_18, buffer, 460, 25);

        if(paused) drawPauseScreen();
    }
    else
    {
        drawGameOver();
    }

    glutSwapBuffers();
}

// ======================================================
// TIMING SUBSYSTEM
// ======================================================
void timer(int value)
{
    if(gameStarted && !paused && !gameOver)
    {
        moveSnake();
    }
    glutPostRedisplay();
    glutTimerFunc(speed, timer, 0);
}

// ======================================================
// KEYBOARD HANDLER (Arrow keys handle all direction assignments)
// ======================================================
void specialKeys(int key, int x, int y)
{
    if(paused || gameOver || !gameStarted)
        return;

    switch(key)
    {
        case GLUT_KEY_RIGHT: if(dir != 1) dir = 0; break;
        case GLUT_KEY_LEFT:  if(dir != 0) dir = 1; break;
        case GLUT_KEY_UP:    if(dir != 3) dir = 2; break;
        case GLUT_KEY_DOWN:  if(dir != 2) dir = 3; break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 13:  gameStarted = 1; break;
        case 'p':
        case 'P': if(gameStarted && !gameOver) paused = !paused; break;
        case 'r':
        case 'R': if(gameOver) resetGame(); break;
        case 27:  exit(0); break;
    }
}

// ======================================================
// PASSIVE MOTION VECTOR TRACKING
// ======================================================
void passiveMouseMotion(int x, int y)
{
    mouseX = x;
    mouseY = y;
}

// ======================================================
// INTERACTION CLICK BOUNDS MANAGER
// ======================================================
void mouseClicks(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouseX = x;
        mouseY = y;

        if(!gameStarted)
        {
            if(mouseX >= 220 && mouseX <= 420 && mouseY >= 190 && mouseY <= 240) gameStarted = 1;
            if(mouseX >= 220 && mouseX <= 420 && mouseY >= 270 && mouseY <= 320) exit(0);
        }
        else if(paused)
        {
            if(mouseX >= 220 && mouseX <= 420 && mouseY >= 220 && mouseY <= 270) paused = 0;
            if(mouseX >= 220 && mouseX <= 420 && mouseY >= 300 && mouseY <= 350) exit(0);
        }
        else if(gameOver)
        {
            if(mouseX >= 220 && mouseX <= 420 && mouseY >= 260 && mouseY <= 310) resetGame();
            if(mouseX >= 220 && mouseX <= 420 && mouseY >= 340 && mouseY <= 390) exit(0);
        }
    }
}

// ======================================================
// CLEANUP
// ======================================================
void cleanUp()
{
    if(hitSound) Mix_FreeChunk(hitSound);
    if(eatSound) Mix_FreeChunk(eatSound);
    Mix_CloseAudio();
    SDL_Quit();
}

// ======================================================
// MAIN ENTRY POINT
// ======================================================
int main(int argc, char** argv)
{
    srand((unsigned int)time(0));
    loadHighScore();
    resetGame();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Snake Game");

    if(SDL_Init(SDL_INIT_AUDIO) < 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("Audio Initialization Failed!\n");
    }
    else
    {
        hitSound = Mix_LoadWAV("hit.wav");
        eatSound = Mix_LoadWAV("eat.wav");
    }

    myInit();

    // Setup GLUT Listeners and Engine Hooks
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys); // Keyboard Arrow Keys Listener Re-engaged
    glutKeyboardFunc(keyboard);
    
    // Mouse Callbacks kept purely for Buttons and Hover effects
    glutMouseFunc(mouseClicks);
    glutPassiveMotionFunc(passiveMouseMotion);

    glutTimerFunc(speed, timer, 0);
    atexit(cleanUp);

    glutMainLoop();
    return 0;
}