#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") // Link sound library

// Global Variables
float carX = -300.0f;
float ballY = 0.0f;
bool ballUp = true;

// Utility: Draw filled circle
void drawCircle(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 100; ++i) {
        float angle = 2 * 3.1416f * i / 100;
        glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
    }
    glEnd();
}

// Cartoon Car
void drawRedCartoonCar(float x, float y) {
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 120, y); glVertex2f(x, y);
    glVertex2f(x, y + 20); glVertex2f(x + 20, y + 40);
    glVertex2f(x + 80, y + 40); glVertex2f(x + 100, y + 20);
    glVertex2f(x + 120, y + 20);
    glEnd();

    glColor3f(0.6f, 0.8f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 12, y + 22); glVertex2f(x + 25, y + 38);
    glVertex2f(x + 55, y + 38); glVertex2f(x + 55, y + 22);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(x + 60, y + 22); glVertex2f(x + 60, y + 38);
    glVertex2f(x + 82, y + 38); glVertex2f(x + 98, y + 22);
    glEnd();

    for (int i = 0; i < 2; i++) {
        float cx = x + 25 + i * 70;
        float cy = y - 2;
        drawCircle(cx, cy, 12, 0, 0, 0);
        drawCircle(cx, cy, 6, 1, 1, 1);
    }
}

// Small Tree
void drawSmallMountainTree(float x, float y) {
    glColor3f(0.4f, 0.2f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 1, y); glVertex2f(x + 1, y);
    glVertex2f(x + 1, y + 5); glVertex2f(x - 1, y + 5);
    glEnd();
    drawCircle(x, y + 8, 4, 0.0f, 0.4f, 0.0f);
}

// Fruit Tree
void drawFruitTree(float x, float y) {
    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 5, y); glVertex2f(x + 5, y);
    glVertex2f(x + 5, y + 50); glVertex2f(x - 5, y + 50);
    glEnd();

    float cx = x, cy = y + 60;
    drawCircle(cx, cy + 10, 20, 0.0f, 0.6f, 0.0f);
    drawCircle(cx - 15, cy, 18, 0.0f, 0.5f, 0.0f);
    drawCircle(cx + 15, cy, 18, 0.0f, 0.5f, 0.0f);

    drawCircle(cx - 5, cy + 5, 3, 1, 0, 0);
    drawCircle(cx + 8, cy + 12, 3, 1, 0, 0);
    drawCircle(cx - 12, cy - 4, 3, 1, 0, 0);
    drawCircle(cx + 15, cy - 2, 3, 1, 0, 0);
    drawCircle(cx + 2, cy - 8, 3, 1, 0, 0);
}

// Boat
void drawBoat(float x, float y) {
    glColor3f(0.4f, 0.2f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 20, y); glVertex2f(x + 20, y);
    glVertex2f(x + 15, y - 10); glVertex2f(x - 15, y - 10);
    glEnd();

    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex2f(x, y); glVertex2f(x, y + 20);
    glEnd();

    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 20); glVertex2f(x, y);
    glVertex2f(x + 15, y + 10);
    glEnd();
}

// Mountains
void drawDistantMountains() {
    glColor3f(0.7f, 0.9f, 0.7f);
    glBegin(GL_TRIANGLES);
    glVertex2f(50, 300); glVertex2f(200, 400); glVertex2f(350, 300);
    glVertex2f(300, 300); glVertex2f(450, 420); glVertex2f(600, 300);
    glVertex2f(550, 300); glVertex2f(620, 370); glVertex2f(700, 300);
    glEnd();

    float treeX[] = {120, 160, 200, 280, 320, 360, 400, 460, 520, 580, 600, 650};
    float treeY[] = {310, 315, 320, 310, 305, 300, 310, 305, 310, 300, 310, 305};
    for (int i = 0; i < 12; ++i)
        drawSmallMountainTree(treeX[i], treeY[i]);
}

// Ocean
void drawOcean() {
    glColor3f(0.2f, 0.6f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0, 150); glVertex2f(800, 150);
    glVertex2f(800, 300); glVertex2f(0, 300);
    glEnd();

    drawBoat(200, 250);
    drawBoat(700, 270);
    drawBoat(500, 210);
}

// Beach
void drawBeach() {
    glColor3f(0.96f, 0.76f, 0.26f);
    glBegin(GL_POLYGON);
    glVertex2f(0, 100); glVertex2f(800, 100);
    glVertex2f(800, 200); glVertex2f(0, 200);
    glEnd();
}

// Road
void drawRoad() {
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(0, 0); glVertex2f(800, 0);
    glVertex2f(800, 100); glVertex2f(0, 100);
    glEnd();

    glColor3f(1, 1, 1);
    for (float i = 0; i < 800; i += 40) {
        glBegin(GL_LINES);
        glVertex2f(i, 50); glVertex2f(i + 20, 50);
        glEnd();
    }
}

// Beach Ball
void drawBeachBallOnSand() {
    float cx = 650, cy = 130 + ballY, radius = 12;
    float colors[6][3] = {
        {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
        {0, 1, 1}, {0, 0, 1}, {1, 0, 1}
    };

    for (int i = 0; i < 100; ++i) {
        int colorIndex = (i * 6) / 100;
        glColor3f(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
        float a1 = i * 2 * 3.1416f / 100;
        float a2 = (i + 1) * 2 * 3.1416f / 100;
        glBegin(GL_TRIANGLES);
        glVertex2f(cx, cy);
        glVertex2f(cx + cos(a1) * radius, cy + sin(a1) * radius);
        glVertex2f(cx + cos(a2) * radius, cy + sin(a2) * radius);
        glEnd();
    }

    drawCircle(cx, cy, 3, 1, 1, 1);
}

// Sun
void drawSun() {
    drawCircle(100, 450, 30, 1, 0.85f, 0);
}

// Cloud
void drawCloud(float x, float y) {
    drawCircle(x, y, 15, 1, 1, 1);
    drawCircle(x + 20, y, 15, 1, 1, 1);
    drawCircle(x + 10, y + 10, 15, 1, 1, 1);
}

// Bird
void drawBird(float x, float y) {
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex2f(x - 5, y); glVertex2f(x, y + 5);
    glVertex2f(x, y + 5); glVertex2f(x + 5, y);
    glEnd();
}

// Roadside Trees
void drawRoadsideTrees() {
    float positions[10] = {50, 130, 210, 290, 370, 450, 530, 610, 690, 770};
    for (int i = 0; i < 10; i++) {
        drawFruitTree(positions[i], 100);
    }
}

// Draw All
void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawSun();
    drawDistantMountains();
    drawOcean();
    drawBeach();
    drawRoad();
    drawRoadsideTrees();
    drawBeachBallOnSand();
    drawRedCartoonCar(carX, 90);
    drawCloud(500, 430); drawCloud(600, 390); drawCloud(700, 410);
    drawBird(200, 420); drawBird(250, 410);
    glutSwapBuffers();
}

// Animation
void update(int value) {
    carX += 2.0f;
    if (carX > 800) carX = -150;

    ballY += ballUp ? 0.8f : -0.8f;
    if (ballY > 15) ballUp = false;
    if (ballY < 0) ballUp = true;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Init
void initGL() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 500);
}

// Main
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 500);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Scenic Journey – Ocean & Mountain Scene with Sound");
    glutDisplayFunc(drawScene);
    glutTimerFunc(0, update, 0);
    initGL();
    PlaySound(TEXT("sound.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    glutMainLoop();
    return 0;
}
