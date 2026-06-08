#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <glut.h>
#else
#include <GL/glut.h>
#endif

// ======================================================
// STATES
// ======================================================

enum State
{
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_HELP,
    STATE_RULES,
    STATE_ABOUT
};

State gameState = STATE_MENU;

// ======================================================
// WINDOW
// ======================================================

const int BASE_WIDTH = 1280;
const int BASE_HEIGHT = 720;

// ======================================================
// PLAYER
// ======================================================

float px = 640;
float py = 300;

float previousPy = 300;

float ballRadius = 22;

float xVelocity = 0;
float yVelocity = 0;

float gravity = 0.45f;
float jumpForce = -11.5f;

float ballRotation = 0;

bool grounded = false;

// ======================================================
// SCORE
// ======================================================

int currentScore = 0;
int currentLevel = 1;

int highScore = 0;
int highestLevel = 1;

// ======================================================
// MENU
// ======================================================

int selectedMenuItem = 0;

const char *menuItems[] =
    {
        "CONTINUE GAME",
        "NEW GAME",
        "HELP",
        "CONTROLS",
        "ABOUT",
        "EXIT"};

// ======================================================
// UI BUTTON
// ======================================================

struct UIButton
{
    float x;
    float y;
    float width;
    float height;
};

UIButton menuButtons[6];
UIButton backButton;

// ======================================================
// PLATFORM
// ======================================================

struct Platform
{
    float x;
    float y;
    float width;
    float height;
};

#define MAX_PLATFORMS 8

Platform platforms[MAX_PLATFORMS];

float platformSpeed = 2.0f;

// ======================================================
// COINS
// ======================================================

struct Coin
{
    float x;
    float y;
    bool active;
    float pulse;
};

#define MAX_COINS 6

Coin coins[MAX_COINS];

// ======================================================
// PARTICLES
// ======================================================

struct Particle
{
    float x;
    float y;

    float vx;
    float vy;

    float alpha;
    float size;

    bool active;
};

#define MAX_PARTICLES 120

Particle particles[MAX_PARTICLES];

// ======================================================
// RANDOM
// ======================================================

float randomFloat(float min, float max)
{
    return min +
           ((float)rand() / RAND_MAX) *
               (max - min);
}

// ======================================================
// SAVE DATA
// ======================================================

void saveHighData()
{
    FILE *file = fopen("highscore.txt", "w");

    if (file)
    {
        fprintf(file,
                "%d %d",
                highScore,
                highestLevel);

        fclose(file);
    }
}

void loadHighData()
{
    FILE *file = fopen("highscore.txt", "r");

    if (file)
    {
        fscanf(file,
               "%d %d",
               &highScore,
               &highestLevel);

        fclose(file);
    }
}

// ======================================================
// TEXT
// ======================================================

void drawText(
    void *font,
    const char *text,
    float x,
    float y,
    float r,
    float g,
    float b)
{
    glColor3f(0, 0, 0);

    glRasterPos2f(x + 2, y + 2);

    for (unsigned int i = 0; i < strlen(text); i++)
        glutBitmapCharacter(font, text[i]);

    glColor3f(r, g, b);

    glRasterPos2f(x, y);

    for (unsigned int i = 0; i < strlen(text); i++)
        glutBitmapCharacter(font, text[i]);
}

// ======================================================
// SHAPES
// ======================================================

void drawRect(
    float x,
    float y,
    float width,
    float height,
    float r,
    float g,
    float b)
{
    glColor3f(r, g, b);

    glBegin(GL_QUADS);

    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);

    glEnd();
}

void drawCircle(
    float cx,
    float cy,
    float r,
    int segments)
{
    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(cx, cy);

    for (int i = 0; i <= segments; i++)
    {
        float angle =
            2.0f * 3.1415926f *
            i / segments;

        float x = r * cosf(angle);
        float y = r * sinf(angle);

        glVertex2f(cx + x, cy + y);
    }

    glEnd();
}

// ======================================================
// BACKGROUND
// ======================================================

void drawBackground()
{
    glBegin(GL_QUADS);

    glColor3f(0.05f, 0.05f, 0.15f);
    glVertex2f(0, 0);

    glColor3f(0.08f, 0.08f, 0.20f);
    glVertex2f(BASE_WIDTH, 0);

    glColor3f(0.18f, 0.10f, 0.35f);
    glVertex2f(BASE_WIDTH, BASE_HEIGHT);

    glColor3f(0.12f, 0.08f, 0.25f);
    glVertex2f(0, BASE_HEIGHT);

    glEnd();
}

// ======================================================
// PARTICLES
// ======================================================

void spawnParticles(float x, float y)
{
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < MAX_PARTICLES; j++)
        {
            if (!particles[j].active)
            {
                particles[j].active = true;

                particles[j].x = x;
                particles[j].y = y;

                particles[j].vx =
                    randomFloat(-3, 3);

                particles[j].vy =
                    randomFloat(-3, 3);

                particles[j].alpha = 1.0f;

                particles[j].size =
                    randomFloat(2, 5);

                break;
            }
        }
    }
}

// ======================================================
// RESET GAME
// ======================================================

void resetGame()
{
    px = 640;
    py = 300;

    xVelocity = 0;
    yVelocity = 0;

    currentScore = 0;
    currentLevel = 1;

    platformSpeed = 2.0f;

    for (int i = 0; i < MAX_PLATFORMS; i++)
    {
        platforms[i].width = 180;
        platforms[i].height = 20;

        platforms[i].x =
            randomFloat(220, 900);

        platforms[i].y =
            140 + i * 80;
    }

    for (int i = 0; i < MAX_COINS; i++)
    {
        coins[i].active = false;
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].active = false;
    }
}

// ======================================================
// INIT
// ======================================================

void init()
{
    srand(time(NULL));

    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0, 0, 0, 1);

    loadHighData();

    resetGame();
}

// ======================================================
// RESHAPE
// ======================================================

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluOrtho2D(
        0,
        BASE_WIDTH,
        BASE_HEIGHT,
        0);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
}

// ======================================================
// BALL
// ======================================================

void drawBall()
{
    glPushMatrix();

    glTranslatef(px, py, 0);

    ballRotation += xVelocity * 0.7f;

    glRotatef(ballRotation, 0, 0, 1);

    glColor3f(1.0f, 0.45f, 0.0f);

    drawCircle(0, 0, ballRadius, 40);

    glColor3f(1, 0.8f, 0.2f);

    glLineWidth(3);

    glBegin(GL_LINES);

    glVertex2f(-ballRadius, 0);
    glVertex2f(ballRadius, 0);

    glVertex2f(0, -ballRadius);
    glVertex2f(0, ballRadius);

    glEnd();

    glColor3f(1, 1, 1);

    drawCircle(-6, -6, 4, 20);

    glPopMatrix();
}

// ======================================================
// HUD
// ======================================================

void drawHUD()
{
    char buffer[128];

    drawRect(
        0,
        0,
        180,
        BASE_HEIGHT,
        0.08f,
        0.08f,
        0.15f);

    drawRect(
        1100,
        0,
        180,
        BASE_HEIGHT,
        0.08f,
        0.08f,
        0.15f);

    drawText(
        GLUT_BITMAP_TIMES_ROMAN_24,
        "PLAYER HUD",
        1120,
        60,
        1,
        1,
        1);

    sprintf(buffer,
            "SCORE : %d",
            currentScore);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        buffer,
        1120,
        140,
        1,
        1,
        0);

    sprintf(buffer,
            "LEVEL : %d",
            currentLevel);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        buffer,
        1120,
        200,
        0.2f,
        1,
        1);

    sprintf(buffer,
            "HIGH SCORE : %d",
            highScore);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        buffer,
        1120,
        260,
        1,
        0.5f,
        0.3f);

    sprintf(buffer,
            "BEST LEVEL : %d",
            highestLevel);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        buffer,
        1120,
        320,
        1,
        0.7f,
        1);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        "P = PAUSE",
        30,
        650,
        1,
        1,
        1);
}

// ======================================================
// MENU
// ======================================================

void renderMenu()
{
    drawBackground();

    float buttonWidth = 420;
    float buttonHeight = 55;

    float startX =
        (BASE_WIDTH - buttonWidth) * 0.5f;

    float startY = 240;

    drawText(
        GLUT_BITMAP_TIMES_ROMAN_24,
        "ARCADE ROLLING BALL",
        430,
        120,
        1,
        1,
        1);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        "FINAL PROFESSIONAL EDITION",
        460,
        170,
        0.2f,
        1,
        1);

    for (int i = 0; i < 6; i++)
    {
        float y = startY + i * 70;

        menuButtons[i].x = startX;
        menuButtons[i].y = y;
        menuButtons[i].width = buttonWidth;
        menuButtons[i].height = buttonHeight;

        if (selectedMenuItem == i)
        {
            drawRect(
                startX - 4,
                y - 4,
                buttonWidth + 8,
                buttonHeight + 8,
                0,
                0.9f,
                1);

            drawRect(
                startX,
                y,
                buttonWidth,
                buttonHeight,
                0.1f,
                0.8f,
                1);

            drawText(
                GLUT_BITMAP_HELVETICA_18,
                menuItems[i],
                startX + 95,
                y + 35,
                0,
                0,
                0);
        }
        else
        {
            drawRect(
                startX,
                y,
                buttonWidth,
                buttonHeight,
                0.15f,
                0.15f,
                0.25f);

            drawText(
                GLUT_BITMAP_HELVETICA_18,
                menuItems[i],
                startX + 95,
                y + 35,
                1,
                1,
                1);
        }
    }
}

// ======================================================
// BACK BUTTON
// ======================================================

void renderBackButton()
{
    backButton.x = 40;
    backButton.y = 40;
    backButton.width = 140;
    backButton.height = 45;

    drawRect(
        backButton.x,
        backButton.y,
        backButton.width,
        backButton.height,
        0.2f,
        0.7f,
        1.0f);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        "BACK",
        backButton.x + 40,
        backButton.y + 28,
        0,
        0,
        0);
}

// ======================================================
// TEXT SCREEN
// ======================================================

void renderTextScreen(
    const char *title,
    const char *lines[],
    int count)
{
    drawBackground();

    drawText(
        GLUT_BITMAP_TIMES_ROMAN_24,
        title,
        520,
        120,
        1,
        1,
        1);

    for (int i = 0; i < count; i++)
    {
        drawText(
            GLUT_BITMAP_HELVETICA_18,
            lines[i],
            240,
            240 + i * 70,
            1,
            1,
            1);
    }

    renderBackButton();
}

// ======================================================
// GAME OVER
// ======================================================

void renderGameOver()
{
    drawBackground();

    drawText(
        GLUT_BITMAP_TIMES_ROMAN_24,
        "GAME OVER",
        520,
        220,
        1,
        0.2f,
        0.2f);

    char buffer[128];

    sprintf(buffer,
            "FINAL SCORE : %d",
            currentScore);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        buffer,
        500,
        320,
        1,
        1,
        1);

    sprintf(buffer,
            "FINAL LEVEL : %d",
            currentLevel);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        buffer,
        500,
        370,
        1,
        1,
        1);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        "PRESS N FOR NEW GAME",
        470,
        470,
        0.2f,
        1,
        0.7f);

    drawText(
        GLUT_BITMAP_HELVETICA_18,
        "PRESS M FOR MENU",
        500,
        520,
        1,
        1,
        0);
}

// ======================================================
// GAME
// ======================================================

void renderGame()
{
    drawBackground();

    drawHUD();

    drawRect(
        180,
        0,
        8,
        BASE_HEIGHT,
        1,
        0.2f,
        0.3f);

    drawRect(
        1092,
        0,
        8,
        BASE_HEIGHT,
        1,
        0.2f,
        0.3f);

    drawRect(
        180,
        0,
        920,
        8,
        1,
        0,
        0);

    for (int i = 0; i < MAX_PLATFORMS; i++)
    {
        drawRect(
            platforms[i].x,
            platforms[i].y,
            platforms[i].width,
            platforms[i].height,
            0.1f,
            1,
            0.4f);
    }

    for (int i = 0; i < MAX_COINS; i++)
    {
        if (coins[i].active)
        {
            coins[i].pulse += 0.1f;

            float pulse =
                12 + sinf(coins[i].pulse) * 2;

            glColor3f(1, 0.85f, 0);

            drawCircle(
                coins[i].x,
                coins[i].y,
                pulse,
                30);
        }
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
        {
            glColor4f(
                1,
                0.5f,
                0,
                particles[i].alpha);

            drawCircle(
                particles[i].x,
                particles[i].y,
                particles[i].size,
                12);
        }
    }

    drawBall();
}

// ======================================================
// GAME OVER LOGIC
// ======================================================

void triggerGameOver()
{
    if (currentScore > highScore)
        highScore = currentScore;

    if (currentLevel > highestLevel)
        highestLevel = currentLevel;

    saveHighData();

    gameState = STATE_GAMEOVER;
}

// ======================================================
// PHYSICS
// ======================================================

void updatePhysics(int value)
{
    if (gameState == STATE_PLAYING)
    {
        previousPy = py;

        grounded = false;

        yVelocity += gravity;

        px += xVelocity;
        py += yVelocity;

        xVelocity *= 0.94f;

        for (int i = 0; i < MAX_PLATFORMS; i++)
        {
            platforms[i].y -= platformSpeed;

            if (platforms[i].y < -20)
            {
                platforms[i].y =
                    BASE_HEIGHT + 40;

                platforms[i].x =
                    randomFloat(220, 900);

                currentScore += 10;

                if (currentScore % 100 == 0)
                {
                    currentLevel++;

                    platformSpeed += 0.2f;
                }
            }

            bool falling =
                yVelocity > 0;

            bool wasAbove =
                previousPy + ballRadius <= platforms[i].y;

            bool nowTouching =
                py + ballRadius >= platforms[i].y;

            bool insideX =
                px + ballRadius >
                    platforms[i].x &&

                px - ballRadius <
                    platforms[i].x +
                        platforms[i].width;

            if (falling &&
                wasAbove &&
                nowTouching &&
                insideX)
            {
                py =
                    platforms[i].y -
                    ballRadius;

                yVelocity = jumpForce;

                grounded = true;

                spawnParticles(px, py);
            }
        }

        if (py - ballRadius <= 8)
        {
            triggerGameOver();
        }

        if (py + ballRadius >= BASE_HEIGHT)
        {
            triggerGameOver();
        }

        if (px < 210)
        {
            px = 210;
            xVelocity = 0;
        }

        if (px > 1070)
        {
            px = 1070;
            xVelocity = 0;
        }

        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            if (particles[i].active)
            {
                particles[i].x += particles[i].vx;
                particles[i].y += particles[i].vy;

                particles[i].alpha -= 0.03f;

                if (particles[i].alpha <= 0)
                    particles[i].active = false;
            }
        }
    }

    glutPostRedisplay();

    glutTimerFunc(16, updatePhysics, 0);
}

// ======================================================
// DISPLAY
// ======================================================

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    switch (gameState)
    {
    case STATE_MENU:
        renderMenu();
        break;

    case STATE_PLAYING:
        renderGame();
        break;

    case STATE_GAMEOVER:
        renderGameOver();
        break;

    case STATE_HELP:
    {
        const char *help[] =
            {
                "WELCOME TO ROLLING BALL",
                "SURVIVE BY LANDING ON MOVING PLATFORMS",
                "COLLECT GOLD COINS TO INCREASE YOUR SCORE",
                "THE GAME SPEED INCREASES AS YOUR LEVEL RISES",
                "AVOID HITTING THE TOP OR BOTTOM BOUNDARIES",
                "USE QUICK MOVEMENT AND GOOD TIMING TO SURVIVE",
                "TRY TO BEAT YOUR HIGHEST SCORE"};

        renderTextScreen(
            "GAME HELP",
            help,
            7);

        break;
    }

    case STATE_RULES:
    {
        const char *rules[] =
            {
                "A KEY OR LEFT ARROW  : MOVE BALL LEFT",
                "D KEY OR RIGHT ARROW : MOVE BALL RIGHT",
                "MOUSE                 : MENU NAVIGATION",
                "LEFT MOUSE CLICK      : SELECT MENU OPTION",
                "P KEY                 : PAUSE / RESUME GAME",
                "M KEY                 : RETURN TO MAIN MENU",
                "N KEY                 : START NEW GAME",
                "ESC KEY               : EXIT GAME"};
        renderTextScreen(
            "CONTROLS",
            rules,
            8);

        break;
    }

    case STATE_ABOUT:
    {
        const char *about[] =
            {
                "ROLLING BALL",
                "PROFESSIONAL 2D OPENGL ARCADE GAME",
                "BUILT USING OPENGL AND GLUT IN C++",
                "FEATURES REALISTIC PHYSICS, PARTICLES",
                "RESPONSIVE USER INTERFACE AND MOUSE SUPPORT",
                "FOR COMPUTER GRAPHICS OPENGL FINAL PROJECT",
                "DEVELOPED BY YOHANNES GAROMSA",
            };

        renderTextScreen(
            "ABOUT",
            about,
            7);

        break;
    }

    default:
        renderMenu();
        break;
    }

    glutSwapBuffers();
}

// ======================================================
// MOUSE HOVER
// ======================================================

void mouseMotion(int x, int y)
{
    if (gameState == STATE_MENU)
    {
        for (int i = 0; i < 6; i++)
        {
            UIButton b = menuButtons[i];

            if (x >= b.x &&
                x <= b.x + b.width &&
                y >= b.y &&
                y <= b.y + b.height)
            {
                selectedMenuItem = i;
            }
        }
    }

    glutPostRedisplay();
}

// ======================================================
// MOUSE CLICK
// ======================================================

void mouse(
    int button,
    int state,
    int x,
    int y)
{
    if (button != GLUT_LEFT_BUTTON ||
        state != GLUT_DOWN)
        return;

    if (gameState == STATE_MENU)
    {
        for (int i = 0; i < 6; i++)
        {
            UIButton b = menuButtons[i];

            if (x >= b.x &&
                x <= b.x + b.width &&
                y >= b.y &&
                y <= b.y + b.height)
            {
                switch (i)
                {
                case 0:
                    gameState = STATE_PLAYING;
                    break;

                case 1:
                    resetGame();
                    gameState = STATE_PLAYING;
                    break;

                case 2:
                    gameState = STATE_HELP;
                    break;

                case 3:
                    gameState = STATE_RULES;
                    break;

                case 4:
                    gameState = STATE_ABOUT;
                    break;

                case 5:
                    exit(0);
                    break;
                }
            }
        }
    }

    if (gameState == STATE_HELP ||
        gameState == STATE_RULES ||
        gameState == STATE_ABOUT)
    {
        if (x >= backButton.x &&
            x <= backButton.x + backButton.width &&
            y >= backButton.y &&
            y <= backButton.y + backButton.height)
        {
            gameState = STATE_MENU;
        }
    }

    glutPostRedisplay();
}

// ======================================================
// KEYBOARD
// ======================================================

void keyboard(
    unsigned char key,
    int x,
    int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;

    case 'm':
    case 'M':
        gameState = STATE_MENU;
        break;

    case 'n':
    case 'N':
        resetGame();
        gameState = STATE_PLAYING;
        break;

    case 'a':
    case 'A':
        if (gameState == STATE_PLAYING)
            xVelocity = -7;
        break;

    case 'd':
    case 'D':
        if (gameState == STATE_PLAYING)
            xVelocity = 7;
        break;
    }

    glutPostRedisplay();
}

// ======================================================
// SPECIAL KEYS
// ======================================================

void special(
    int key,
    int x,
    int y)
{
    if (gameState == STATE_MENU)
    {
        if (key == GLUT_KEY_UP)
        {
            selectedMenuItem--;

            if (selectedMenuItem < 0)
                selectedMenuItem = 5;
        }

        if (key == GLUT_KEY_DOWN)
        {
            selectedMenuItem++;

            if (selectedMenuItem > 5)
                selectedMenuItem = 0;
        }
    }

    if (gameState == STATE_PLAYING)
    {
        if (key == GLUT_KEY_LEFT)
            xVelocity = -7;

        if (key == GLUT_KEY_RIGHT)
            xVelocity = 7;
    }

    glutPostRedisplay();
}

// ======================================================
// MAIN
// ======================================================

int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(
        GLUT_DOUBLE |
        GLUT_RGBA);

    glutInitWindowSize(
        BASE_WIDTH,
        BASE_HEIGHT);

    glutCreateWindow(
        "JO Rolling Ball");

    init();

    glutDisplayFunc(display);

    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyboard);

    glutSpecialFunc(special);

    glutPassiveMotionFunc(mouseMotion);

    glutMouseFunc(mouse);

    glutTimerFunc(16, updatePhysics, 0);

    glutMainLoop();

    return 0;
}