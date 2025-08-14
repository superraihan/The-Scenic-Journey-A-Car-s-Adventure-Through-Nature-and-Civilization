#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include<mmsystem.h>

#define PI 3.14159265358979323846f
#define WIDTH 800
#define HEIGHT 600

float cloudPosition1 = 0.0f;
float cloudPosition2 = 300.0f;
float cloudPosition3 = 600.0f;
float birdX = 0.0f;
float birdY = 400.0f;
bool birdDirection = true;

float birdWingAngle = 0.0f;
bool birdWingUp = true;
float birdFlapSpeed = 0.2f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.3f)));

float sunX = WIDTH * 0.7f;
float sunY = -200.0f;
bool sunRising = true;
bool sunHasRisen = false;
const float peakSunY = HEIGHT * 0.7f;

float carX = -120.0f;
float carSpeed = 3.0f;
bool carMoving = false;

float riverWaveOffset = 0.0f;

const float FOG_R = 0.85f;
const float FOG_G = 0.9f;
const float FOG_B = 0.95f;

float dawn_horizon_R = 0.9f;
float dawn_horizon_G = 0.5f;
float dawn_horizon_B = 0.2f;

float dawn_top_R = 0.1f;
float dawn_top_G = 0.2f;
float dawn_top_B = 0.4f;

float day_horizon_R = 0.7f;
float day_horizon_G = 0.9f;
float day_horizon_B = 1.0f;

float day_top_R = 0.2f;
float day_top_G = 0.5f;
float day_top_B = 0.8f;


void init() {
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, WIDTH, 0, HEIGHT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    srand(time(NULL));
}

void drawFilledEllipse(float cx, float cy, float rx, float ry, float alpha, float r, float g, float b, int segments =36) {
    glColor4f(r, g, b, alpha);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = i * 2 * PI / segments;
        glVertex2f(cx + rx * cos(angle), cy + ry * sin(angle));
    }
    glEnd();
}

void drawGradientRectangle(float x1, float y1, float x2, float y2,
                           float r1, float g1, float b1,
                           float r2, float g2, float b2) {
    glBegin(GL_POLYGON);
        glColor3f(r2, g2, b2); glVertex2f(x1, y1);
        glColor3f(r2, g2, b2); glVertex2f(x2, y1);
        glColor3f(r1, g1, b1); glVertex2f(x2, y2);
        glColor3f(r1, g1, b1); glVertex2f(x1, y2);
    glEnd();
}

void drawSky() {
    float normalized_sun_y = (sunY - (-200.0f)) / (peakSunY - (-200.0f));
    if (normalized_sun_y < 0.0f) normalized_sun_y = 0.0f;
    if (normalized_sun_y > 1.0f) normalized_sun_y = 1.0f;

    float current_horizon_R = dawn_horizon_R + (day_horizon_R - dawn_horizon_R) * normalized_sun_y;
    float current_horizon_G = dawn_horizon_G + (day_horizon_G - dawn_horizon_G) * normalized_sun_y;
    float current_horizon_B = dawn_horizon_B + (day_horizon_B - dawn_horizon_B) * normalized_sun_y;

    float current_top_R = dawn_top_R + (day_top_R - dawn_top_R) * normalized_sun_y;
    float current_top_G = dawn_top_G + (day_top_G - dawn_top_G) * normalized_sun_y;
    float current_top_B = dawn_top_B + (day_top_B - dawn_top_B) * normalized_sun_y;

    glBegin(GL_QUAD_STRIP);
        glColor3f(current_top_R, current_top_G, current_top_B);
        glVertex2f(0, HEIGHT);
        glVertex2f(WIDTH, HEIGHT);
        glColor3f(current_top_R * 0.7f + current_horizon_R * 0.3f,
                  current_top_G * 0.7f + current_horizon_G * 0.3f,
                  current_top_B * 0.7f + current_horizon_B * 0.3f);
        glVertex2f(0, HEIGHT * 0.6f);
        glVertex2f(WIDTH, HEIGHT * 0.6f);
        glColor3f(current_horizon_R, current_horizon_G, current_horizon_B);
        glVertex2f(0, HEIGHT * 0.4f);
        glVertex2f(WIDTH, HEIGHT * 0.4f);
    glEnd();
}

void drawHorizonHaze() {
    float normalized_sun_y = (sunY - (-200.0f)) / (peakSunY - (-200.0f));
    if (normalized_sun_y < 0.0f) normalized_sun_y = 0.0f;
    if (normalized_sun_y > 1.0f) normalized_sun_y = 1.0f;

    float haze_alpha = 0.6f - (0.3f * normalized_sun_y);
    if (haze_alpha < 0.1f) haze_alpha = 0.1f;

    float current_fog_R = FOG_R + (0.95f - FOG_R) * normalized_sun_y;
    float current_fog_G = FOG_G + (0.98f - FOG_G) * normalized_sun_y;
    float current_fog_B = FOG_B + (1.0f - FOG_B) * normalized_sun_y;

    glBegin(GL_QUAD_STRIP);
        glColor4f(current_fog_R, current_fog_G, current_fog_B, 0.0f);
        glVertex2f(0, HEIGHT * 0.45f);
        glVertex2f(WIDTH, HEIGHT * 0.45f);
        glColor4f(current_fog_R, current_fog_G, current_fog_B, haze_alpha);
        glVertex2f(0, HEIGHT * 0.35f);
        glVertex2f(WIDTH, HEIGHT * 0.35f);
    glEnd();
}

void drawHills() {
    float hill_visibility_factor = (sunY - (-200.0f)) / (peakSunY - (-200.0f));
    if (hill_visibility_factor < 0.0f) hill_visibility_factor = 0.0f;
    if (hill_visibility_factor > 1.0f) hill_visibility_factor = 1.0f;

    float far_hill_r = 0.10f + (0.20f - 0.10f) * hill_visibility_factor;
    float far_hill_g = 0.30f + (0.50f - 0.30f) * hill_visibility_factor;
    float far_hill_b = 0.05f + (0.15f - 0.05f) * hill_visibility_factor;

    glBegin(GL_POLYGON);
        glColor3f(far_hill_r, far_hill_g, far_hill_b);
        glVertex2f(0, HEIGHT * 0.4f);
        glVertex2f(WIDTH * 0.15f, HEIGHT * 0.50f);
        glVertex2f(WIDTH * 0.35f, HEIGHT * 0.46f);
        glVertex2f(WIDTH * 0.55f, HEIGHT * 0.53f);
        glVertex2f(WIDTH * 0.75f, HEIGHT * 0.48f);
        glVertex2f(WIDTH, HEIGHT * 0.51f);
        glVertex2f(WIDTH, HEIGHT * 0.35f);
        glVertex2f(0, HEIGHT * 0.35f);
    glEnd();

    float mid_hill_r = 0.15f + (0.28f - 0.15f) * hill_visibility_factor;
    float mid_hill_g = 0.40f + (0.60f - 0.40f) * hill_visibility_factor;
    float mid_hill_b = 0.07f + (0.10f - 0.07f) * hill_visibility_factor;

    glBegin(GL_POLYGON);
        glColor3f(mid_hill_r, mid_hill_g, mid_hill_b);
        glVertex2f(0, HEIGHT * 0.3f);
        glVertex2f(WIDTH * 0.1f, HEIGHT * 0.36f);
        glVertex2f(WIDTH * 0.25f, HEIGHT * 0.33f);
        glVertex2f(WIDTH * 0.4f, HEIGHT * 0.41f);
        glVertex2f(WIDTH * 0.55f, HEIGHT * 0.39f);
        glVertex2f(WIDTH * 0.7f, HEIGHT * 0.35f);
        glVertex2f(WIDTH * 0.85f, HEIGHT * 0.30f);
        glVertex2f(WIDTH, HEIGHT * 0.32f);
        glVertex2f(WIDTH, HEIGHT * 0.25f);
        glVertex2f(0, HEIGHT * 0.25f);
    glEnd();
}

void drawGround() {
    glBegin(GL_POLYGON);
        glColor3f(0.15f, 0.45f, 0.08f);
        glVertex2f(0, 0);
        glVertex2f(WIDTH, 0);
        glColor3f(0.25f, 0.55f, 0.12f);
        glVertex2f(WIDTH, HEIGHT * 0.25f);
        glVertex2f(0, HEIGHT * 0.25f);
    glEnd();

    glColor4f(0.1f, 0.3f, 0.05f, 0.2f);
    glBegin(GL_POINTS);
    for(int i = 0; i < WIDTH; i += 5) {
        for(int j = 0; j < HEIGHT * 0.25f; j += 5) {
            if (rand() % 10 == 0)
                glVertex2f(i + (rand() % 5), j + (rand() % 5));
        }
    }
    glEnd();
}


void drawHouse(float x, float y, float scale, float sun_brightness_factor) {
    float base_width = 50.0f * scale;
    float base_height = 50.0f * scale;
    float roof_height = 30.0f * scale;
    float foundation_height = 5.0f * scale;
    float door_width = 15.0f * scale;
    float door_height = 35.0f * scale;
    float window_size = 12.0f * scale;

    float dawn_foundation_r = 0.35f, dawn_foundation_g = 0.25f, dawn_foundation_b = 0.15f;
    float day_foundation_r = 0.45f, day_foundation_g = 0.35f, day_foundation_b = 0.25f;

    float dawn_body_top_r = 0.65f, dawn_body_top_g = 0.55f, dawn_body_top_b = 0.4f;
    float day_body_top_r = 0.85f, day_body_top_g = 0.75f, day_body_top_b = 0.6f;

    float dawn_body_bottom_r = 0.55f, dawn_body_bottom_g = 0.45f, dawn_body_bottom_b = 0.3f;
    float day_body_bottom_r = 0.75f, day_body_bottom_g = 0.65f, day_body_bottom_b = 0.5f;

    float dawn_texture_r = 0.4f, dawn_texture_g = 0.3f, dawn_texture_b = 0.2f;
    float day_texture_r = 0.6f, day_texture_g = 0.5f, day_texture_b = 0.4f;

    float dawn_thatch_base_r = 0.5f, dawn_thatch_base_g = 0.35f, dawn_thatch_base_b = 0.15f;
    float day_thatch_base_r = 0.7f, day_thatch_base_g = 0.55f, day_thatch_base_b = 0.35f;

    float dawn_thatch_main_r = 0.6f, dawn_thatch_main_g = 0.45f, dawn_thatch_main_b = 0.25f;
    float day_thatch_main_r = 0.8f, day_thatch_main_g = 0.65f, day_thatch_main_b = 0.45f;

    float dawn_thatch_ridge_r = 0.4f, dawn_thatch_ridge_g = 0.25f, dawn_thatch_ridge_b = 0.05f;
    float day_thatch_ridge_r = 0.6f, day_thatch_ridge_g = 0.4f, day_thatch_ridge_b = 0.2f;


    float current_foundation_r = dawn_foundation_r + (day_foundation_r - dawn_foundation_r) * sun_brightness_factor;
    float current_foundation_g = dawn_foundation_g + (day_foundation_g - dawn_foundation_g) * sun_brightness_factor;
    float current_foundation_b = dawn_foundation_b + (day_foundation_b - dawn_foundation_b) * sun_brightness_factor;

    float current_body_top_r = dawn_body_top_r + (day_body_top_r - dawn_body_top_r) * sun_brightness_factor;
    float current_body_top_g = dawn_body_top_g + (day_body_top_g - dawn_body_top_g) * sun_brightness_factor;
    float current_body_top_b = dawn_body_top_b + (day_body_top_b - dawn_body_top_b) * sun_brightness_factor;

    float current_body_bottom_r = dawn_body_bottom_r + (day_body_bottom_r - dawn_body_bottom_r) * sun_brightness_factor;
    float current_body_bottom_g = dawn_body_bottom_g + (day_body_bottom_g - dawn_body_bottom_g) * sun_brightness_factor;
    float current_body_bottom_b = dawn_body_bottom_b + (day_body_bottom_b - dawn_body_bottom_b) * sun_brightness_factor;

    float current_texture_r = dawn_texture_r + (day_texture_r - dawn_texture_r) * sun_brightness_factor;
    float current_texture_g = dawn_texture_g + (day_texture_g - dawn_texture_g) * sun_brightness_factor;
    float current_texture_b = dawn_texture_b + (day_texture_b - dawn_texture_b) * sun_brightness_factor;

    float current_thatch_base_r = dawn_thatch_base_r + (day_thatch_base_r - dawn_thatch_base_r) * sun_brightness_factor;
    float current_thatch_base_g = dawn_thatch_base_g + (day_thatch_base_g - dawn_thatch_base_g) * sun_brightness_factor;
    float current_thatch_base_b = dawn_thatch_base_b + (day_thatch_base_b - dawn_thatch_base_b) * sun_brightness_factor;

    float current_thatch_main_r = dawn_thatch_main_r + (day_thatch_main_r - dawn_thatch_main_r) * sun_brightness_factor;
    float current_thatch_main_g = dawn_thatch_main_g + (day_thatch_main_g - dawn_thatch_main_g) * sun_brightness_factor;
    float current_thatch_main_b = dawn_thatch_main_b + (day_thatch_main_b - dawn_thatch_main_b) * sun_brightness_factor;

    float current_thatch_ridge_r = dawn_thatch_ridge_r + (day_thatch_ridge_r - dawn_thatch_ridge_r) * sun_brightness_factor;
    float current_thatch_ridge_g = dawn_thatch_ridge_g + (day_thatch_ridge_g - dawn_thatch_ridge_g) * sun_brightness_factor;
    float current_thatch_ridge_b = dawn_thatch_ridge_b + (day_thatch_ridge_b - dawn_thatch_ridge_b) * sun_brightness_factor;


    glColor3f(current_foundation_r, current_foundation_g, current_foundation_b);
    glBegin(GL_POLYGON);
        glVertex2f(x - 2 * scale, y - foundation_height);
        glVertex2f(x + base_width + 2 * scale, y - foundation_height);
        glVertex2f(x + base_width, y);
        glVertex2f(x, y);
    glEnd();

    glBegin(GL_POLYGON);
        glColor3f(current_body_top_r, current_body_top_g, current_body_top_b);
        glVertex2f(x, y + base_height);
        glVertex2f(x + base_width, y + base_height);
        glColor3f(current_body_bottom_r, current_body_bottom_g, current_body_bottom_b);
        glVertex2f(x + base_width, y);
        glVertex2f(x, y);
    glEnd();

    glColor4f(current_texture_r, current_texture_g, current_texture_b, 0.2f);
    glBegin(GL_LINES);
    for (int i = 0; i < base_width; i += 7 * scale) {
        glVertex2f(x + i, y);
        glVertex2f(x + i, y + base_height);
    }
    glEnd();


    glColor3f(current_thatch_base_r, current_thatch_base_g, current_thatch_base_b);
    glBegin(GL_POLYGON);
        glVertex2f(x - 5 * scale, y + base_height - 3 * scale);
        glVertex2f(x + base_width + 5 * scale, y + base_height - 3 * scale);
        glVertex2f(x + base_width + 2 * scale, y + base_height + roof_height * 0.2f);
        glVertex2f(x - 2 * scale, y + base_height + roof_height * 0.2f);
    glEnd();

    glColor3f(current_thatch_main_r, current_thatch_main_g, current_thatch_main_b);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 5 * scale, y + base_height);
        glVertex2f(x + base_width + 5 * scale, y + base_height);
        glVertex2f(x + base_width / 2, y + base_height + roof_height);
    glEnd();

    glColor3f(current_thatch_ridge_r, current_thatch_ridge_g, current_thatch_ridge_b);
    glLineWidth(2.0 * scale);
    glBegin(GL_LINES);
        glVertex2f(x + base_width / 2, y + base_height + roof_height);
        glVertex2f(x + base_width / 2, y + base_height + roof_height - 5 * scale);
    glEnd();
    glLineWidth(1.0);


    glColor3f(0.3f, 0.15f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2f(x + (base_width - door_width) / 2, y);
        glVertex2f(x + (base_width + door_width) / 2, y);
        glVertex2f(x + (base_width + door_width) / 2, y + door_height);
        glVertex2f(x + (base_width - door_width) / 2, y + door_height);
    glEnd();
    glColor3f(0.1f, 0.05f, 0.0f);
    drawFilledEllipse(x + (base_width + door_width) / 2 - 3 * scale, y + door_height / 2, 1.5 * scale, 1.5 * scale, 1.0f, 0.1f, 0.05f, 0.0f);


    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 10 * scale, y + base_height - 25 * scale);
        glVertex2f(x + 10 * scale + window_size, y + base_height - 25 * scale);
        glVertex2f(x + 10 * scale + window_size, y + base_height - 25 * scale + window_size);
        glVertex2f(x + 10 * scale, y + base_height - 25 * scale + window_size);
    glEnd();
    if (base_width > 40 * scale) {
        glBegin(GL_POLYGON);
            glVertex2f(x + base_width - 10 * scale - window_size, y + base_height - 25 * scale);
            glVertex2f(x + base_width - 10 * scale, y + base_height - 25 * scale);
            glVertex2f(x + base_width - 10 * scale, y + base_height - 25 * scale + window_size);
            glVertex2f(x + base_width - 10 * scale - window_size, y + base_height - 25 * scale + window_size);
        glEnd();
    }
}

void drawRealisticTree(float x, float y, float scale) {
    float trunk_base_w = 15 * scale;
    float trunk_top_w = 8 * scale;
    float trunk_h = 80 * scale;
    float canopy_r_max = 60 * scale;

    glColor3f(0.45f, 0.22f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2f(x - trunk_base_w / 2, y);
        glVertex2f(x + trunk_base_w / 2, y);
        glVertex2f(x + trunk_top_w / 2 + 2 * scale, y + trunk_h * 0.4f);
        glVertex2f(x - trunk_top_w / 2 - 2 * scale, y + trunk_h * 0.4f);
    glEnd();

    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_POLYGON);
        glVertex2f(x - trunk_top_w / 2 * 0.8f, y + trunk_h * 0.4f);
        glVertex2f(x + trunk_top_w / 2 * 0.8f, y + trunk_h * 0.4f);
        glVertex2f(x + trunk_top_w / 2 * 0.8f, y + trunk_h);
        glVertex2f(x - trunk_top_w / 2 * 0.8f, y + trunk_h);
    glEnd();

    glColor3f(0.4f, 0.2f, 0.05f);
    glLineWidth(2.0);
    glBegin(GL_POLYGON);
        glVertex2f(x + (rand() % 5), y + trunk_h + (rand() % 10));
        glVertex2f(x - trunk_top_w / 2 * 0.8f, y + trunk_h * 0.7f);
        glVertex2f(x - trunk_top_w / 1.5f, y + trunk_h * 0.8f);
        glVertex2f(x - trunk_top_w * 2.5f, y + trunk_h * 1.1f);
        glVertex2f(x - trunk_top_w * 2, y + trunk_h * 1.0f);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(x + trunk_top_w / 2 * 0.8f, y + trunk_h * 0.6f);
        glVertex2f(x + trunk_top_w * 1.2f, y + trunk_h * 0.7f);
        glVertex2f(x + trunk_top_w * 2.0f, y + trunk_h * 0.9f);
        glVertex2f(x + trunk_top_w * 1.5f, y + trunk_h * 0.8f);
    glEnd();
    glLineWidth(1.0);

    drawFilledEllipse(x + 5 * scale, y + trunk_h + 30 * scale, canopy_r_max * 0.8f, canopy_r_max * 0.7f, 0.8f, 0.05f, 0.45f, 0.05f, 24);
    drawFilledEllipse(x + 10 * scale, y + trunk_h + 45 * scale, canopy_r_max, canopy_r_max * 0.9f, 0.85f, 0.1f, 0.6f, 0.1f, 28);
    drawFilledEllipse(x - 15 * scale, y + trunk_h + 35 * scale, canopy_r_max * 0.7f, canopy_r_max * 0.8f, 0.88f, 0.15f, 0.65f, 0.15f, 26);
    drawFilledEllipse(x + 25 * scale, y + trunk_h + 30 * scale, canopy_r_max * 0.6f, canopy_r_max * 0.65f, 0.87f, 0.12f, 0.62f, 0.12f, 25);
    drawFilledEllipse(x + 20 * scale, y + trunk_h + 55 * scale, canopy_r_max * 0.7f, canopy_r_max * 0.65f, 0.9f, 0.2f, 0.7f, 0.2f, 24);
    drawFilledEllipse(x - 5 * scale, y + trunk_h + 60 * scale, canopy_r_max * 0.5f, canopy_r_max * 0.55f, 0.9f, 0.25f, 0.75f, 0.25f, 22);
    drawFilledEllipse(x + 30 * scale, y + trunk_h + 70 * scale, canopy_r_max * 0.3f, canopy_r_max * 0.28f, 0.95f, 0.4f, 0.85f, 0.4f, 18);

    drawFilledEllipse(x + 8 * scale, y + trunk_h + 20 * scale, canopy_r_max * 0.6f, canopy_r_max * 0.5f, 0.6f, 0.0f, 0.3f, 0.0f, 20);
    drawFilledEllipse(x - 10 * scale, y + trunk_h + 25 * scale, canopy_r_max * 0.5f, canopy_r_max * 0.45f, 0.65f, 0.02f, 0.35f, 0.02f, 20);
}


void drawRoad() {
    glBegin(GL_POLYGON);
        glColor3f(0.2f, 0.2f, 0.2f); glVertex2f(0, 0);
        glColor3f(0.2f, 0.2f, 0.2f); glVertex2f(WIDTH, 0);
        glColor3f(0.3f, 0.3f, 0.3f); glVertex2f(WIDTH, HEIGHT * 0.12f);
        glColor3f(0.3f, 0.3f, 0.3f); glVertex2f(0, HEIGHT * 0.12f);
    glEnd();

    glColor4f(0.1f, 0.1f, 0.1f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(WIDTH * 0.2f, HEIGHT * 0.05f);
        glVertex2f(WIDTH * 0.25f, HEIGHT * 0.06f);
        glVertex2f(WIDTH * 0.23f, HEIGHT * 0.09f);
        glVertex2f(WIDTH * 0.18f, HEIGHT * 0.08f);

        glVertex2f(WIDTH * 0.6f, HEIGHT * 0.02f);
        glVertex2f(WIDTH * 0.63f, HEIGHT * 0.03f);
        glVertex2f(WIDTH * 0.62f, HEIGHT * 0.06f);
        glVertex2f(WIDTH * 0.59f, HEIGHT * 0.05f);
    glEnd();
}

void drawDashedRoadLine() {
    glColor3f(0.9f, 0.9f, 0.0f);
    glLineWidth(4.0);

    float roadCenterY = (0 + HEIGHT * 0.12f) / 2.0f;
    float lineBottomY = roadCenterY - 3;
    float lineTopY = roadCenterY + 3;

    for (int i = 0; i < WIDTH; i += 80) {
        glBegin(GL_POLYGON);
            glVertex2f(i, lineBottomY);
            glVertex2f(i + 40, lineBottomY);
            glVertex2f(i + 40, lineTopY);
            glVertex2f(i, lineTopY);
        glEnd();
    }
    glLineWidth(1.0);
}


void drawCar(float x, float y) {
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);

        glVertex2f(x + 120, y);
        glVertex2f(x, y);
        glVertex2f(x, y + 20);
        glVertex2f(x + 20, y + 40);
        glVertex2f(x + 80, y + 40);
        glVertex2f(x + 100, y + 20);
        glVertex2f(x + 120, y + 20);
    glEnd();

    glColor3f(0.6f, 0.8f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 12, y + 22);
        glVertex2f(x + 25, y + 38);
        glVertex2f(x + 55, y + 38);
        glVertex2f(x + 55, y + 22);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(x + 60, y + 22);
        glVertex2f(x + 60, y + 38);
        glVertex2f(x + 82, y + 38);
        glVertex2f(x + 98, y + 22);
    glEnd();

    for (int i = 0; i < 2; i++) {
        float cx = x + 25 + i * 70;
        float cy = y - 2;
        float radiusOuter = 12;
        float radiusInner = 6;

        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int j = 0; j <= 100; ++j) {
                float angle = 2 * 3.1416f * j / 100;
                glVertex2f(cx + cos(angle) * radiusOuter, cy + sin(angle) * radiusOuter);
            }
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int j = 0; j <= 100; ++j) {
                float angle = 2 * 3.1416f * j / 100;
                glVertex2f(cx + cos(angle) * radiusInner, cy + sin(angle) * radiusInner);
            }

        glEnd();
    }
}

void drawSun(float x, float y, float baseRadius) {
    drawFilledEllipse(x, y, baseRadius, baseRadius, 1.0f, 1.0f, 1.0f, 0.0f, 60);

    drawFilledEllipse(x, y, baseRadius * 1.3, baseRadius * 1.3, 0.3f, 1.0f, 0.9f, 0.0f, 60);
    drawFilledEllipse(x, y, baseRadius * 1.6, baseRadius * 1.6, 0.15f, 1.0f, 0.85f, 0.0f, 60);
    drawFilledEllipse(x, y, baseRadius * 2.0, baseRadius * 2.0, 0.08f, 1.0f, 0.8f, 0.0f, 60);

    glColor4f(1.0f, 1.0f, 0.5f, 0.1f);
    for (int i = 0; i < 16; ++i) {
        float angle = i * (360.0f / 16) * PI / 180.0f;
        float start_x = x + baseRadius * cos(angle);
        float start_y = y + baseRadius * sin(angle);
        float end_x = x + baseRadius * 2.5 * cos(angle);
        float end_y = y + baseRadius * 2.5 * sin(angle);

        glBegin(GL_LINES);
            glVertex2f(start_x, start_y);
            glVertex2f(end_x, end_y);
        glEnd();
    }
}

void drawCloud(float x, float y) {
    const float CLOUD_R = 0.95f;
    const float CLOUD_G = 0.97f;
    const float CLOUD_B = 1.0f;

    drawFilledEllipse(x + 40, y + 12, 60, 25, 0.9f, CLOUD_R, CLOUD_G, CLOUD_B, 30);
    drawFilledEllipse(x + 15, y + 16, 45, 20, 0.92f, CLOUD_R * 1.02f, CLOUD_G * 1.02f, CLOUD_B * 1.02f, 30);
    drawFilledEllipse(x + 75, y + 16, 55, 22, 0.88f, CLOUD_R, CLOUD_G, CLOUD_B, 30);

    drawFilledEllipse(x + 30, y + 22, 40, 20, 0.95f, CLOUD_R * 1.05f, CLOUD_G * 1.05f, CLOUD_B * 1.05f, 30);
    drawFilledEllipse(x + 65, y + 25, 45, 22, 0.9f, CLOUD_R * 1.03f, CLOUD_G * 1.03f, CLOUD_B * 1.03f, 30);
    drawFilledEllipse(x + 50, y + 30, 35, 18, 0.98f, 1.0f, 1.0f, 1.0f, 30);

    drawFilledEllipse(x + 40, y + 2, 60, 20, 0.5f, CLOUD_R * 0.8f, CLOUD_G * 0.8f, CLOUD_B * 0.8f, 30);
    drawFilledEllipse(x + 25, y + 6, 30, 15, 0.45f, CLOUD_R * 0.75f, CLOUD_G * 0.75f, CLOUD_B * 0.75f, 30);

    drawFilledEllipse(x + 55, y + 33, 20, 12, 0.9f, 1.0f, 1.0f, 1.0f, 30);
    drawFilledEllipse(x + 20, y + 27, 18, 10, 0.85f, 1.0f, 1.0f, 1.0f, 30);
}

void drawRiver(float y_start, float y_end) {
    glColor3f(0.2f, 0.6f, 0.9f);

    glBegin(GL_POLYGON);
        glVertex2f(0, y_start);
        glVertex2f(WIDTH, y_start);
        glVertex2f(WIDTH, y_end);
        glVertex2f(0, y_end);
    glEnd();

    glBegin(GL_QUAD_STRIP);
        glColor3f(0.1f, 0.4f, 0.7f);
        glVertex2f(0, y_start);
        glVertex2f(WIDTH, y_start);
        glColor3f(0.2f, 0.6f, 0.9f);
        glVertex2f(0, y_end);
        glVertex2f(WIDTH, y_end);
    glEnd();

    glColor4f(0.95f, 0.98f, 1.0f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    for (int i = 0; i < WIDTH; i += 10) {
        float ripple_y_base = y_start + (y_end - y_start) * 0.85f;
        float ripple_amplitude = 8.0f;
        float ripple_frequency = 0.02f;

        glVertex2f(i, ripple_y_base + sin(i * ripple_frequency + riverWaveOffset * 1.5f) * ripple_amplitude);
        glVertex2f(i + 10, ripple_y_base + sin((i + 10) * ripple_frequency + riverWaveOffset * 1.5f) * ripple_amplitude);
    }
    for (int i = 0; i < WIDTH; i += 12) {
        float ripple_y_base = y_start + (y_end - y_start) * 0.65f;
        float ripple_amplitude = 10.0f;
        float ripple_frequency = 0.015f;

        glVertex2f(i, ripple_y_base + cos(i * ripple_frequency + riverWaveOffset * 1.2f) * ripple_amplitude);
        glVertex2f(i + 15, ripple_y_base + cos((i + 15) * ripple_frequency + riverWaveOffset * 1.2f) * ripple_amplitude);
    }
    for (int i = 0; i < WIDTH; i += 15) {
        float ripple_y_base = y_start + (y_end - y_start) * 0.45f;
        float ripple_amplitude = 12.0f;
        float ripple_frequency = 0.01f;

        glVertex2f(i, ripple_y_base + sin(i * ripple_frequency * 0.8f + riverWaveOffset * 0.7f) * ripple_amplitude);
        glVertex2f(i + 20, ripple_y_base + sin((i + 20) * ripple_frequency * 0.8f + riverWaveOffset * 0.7f) * ripple_amplitude);
    }
    for (int i = 0; i < WIDTH; i += 18) {
        float ripple_y_base = y_start + (y_end - y_start) * 0.25f;
        float ripple_amplitude = 7.0f;
        float ripple_frequency = 0.025f;

        glVertex2f(i, ripple_y_base + cos(i * ripple_frequency * 1.5f + riverWaveOffset * 2.0f) * ripple_amplitude);
        glVertex2f(i + 12, ripple_y_base + cos((i + 12) * ripple_frequency * 1.5f + riverWaveOffset * 2.0f) * ripple_amplitude);
    }
    glEnd();

    glLineWidth(1.0);
}

void drawBird(float x, float y, bool direction) {
    glColor3f(0.2f, 0.2f, 0.2f);

    glBegin(GL_TRIANGLES);
        glVertex2f(x + (direction ? 0 : 15), y);
        glVertex2f(x + (direction ? 15 : 0), y);
        glVertex2f(x + (direction ? 7.5 : 7.5), y + 7);
    glEnd();

    glPushMatrix();
    glTranslatef(x + (direction ? 7.5 : 7.5), y + 7, 0.0f);
    glRotatef(birdWingAngle * (direction ? 1 : -1), 0.0f, 0.0f, 1.0f);
    glTranslatef(-(x + (direction ? 7.5 : 7.5)), -(y + 7), 0.0f);

    glBegin(GL_TRIANGLES);
        glVertex2f(x + (direction ? 7.5 : 7.5), y + 7);
        glVertex2f(x + (direction ? 25 : -10), y + 18);
        glVertex2f(x + (direction ? 15 : 0), y + 7);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x + (direction ? 7.5 : 7.5), y + 7, 0.0f);
    glRotatef(-birdWingAngle * (direction ? 1 : -1), 0.0f, 0.0f, 1.0f);
    glTranslatef(-(x + (direction ? 7.5 : 7.5)), -(y + 7), 0.0f);

    glBegin(GL_TRIANGLES);
        glVertex2f(x + (direction ? 7.5 : 7.5), y + 7);
        glVertex2f(x + (direction ? -10 : 25), y + 18);
        glVertex2f(x + (direction ? 0 : 15), y + 7);
    glEnd();
    glPopMatrix();
}

void drawFence(float y_offset, float height, float spacing) {
    glColor3f(0.5f, 0.3f, 0.1f);
    float post_width = 5.0f;
    for (float x = 10.0f; x < WIDTH - 10.0f; x += spacing) {
        glBegin(GL_POLYGON);
            glVertex2f(x, y_offset);
            glVertex2f(x + post_width, y_offset);
            glVertex2f(x + post_width, y_offset + height);
            glVertex2f(x, y_offset + height);
        glEnd();
    }

    glColor3f(0.6f, 0.35f, 0.15f);
    float rail_height = 5.0f;
    glBegin(GL_POLYGON);
        glVertex2f(0, y_offset + height * 0.2f);
        glVertex2f(WIDTH, y_offset + height * 0.2f);
        glVertex2f(WIDTH, y_offset + height * 0.2f + rail_height);
        glVertex2f(0, y_offset + height * 0.2f + rail_height);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(0, y_offset + height * 0.8f);
        glVertex2f(WIDTH, y_offset + height * 0.8f);
        glVertex2f(WIDTH, y_offset + height * 0.8f + rail_height);
        glVertex2f(0, y_offset + height * 0.8f + rail_height);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    float normalized_sun_y = (sunY - (-200.0f)) / (peakSunY - (-200.0f));
    if (normalized_sun_y < 0.0f) normalized_sun_y = 0.0f;
    if (normalized_sun_y > 1.0f) normalized_sun_y = 1.0f;


    drawSky();

    drawSun(sunX, sunY, 50);

    drawHorizonHaze();
    drawHills();

    drawCloud(cloudPosition1, HEIGHT - 100);
    drawCloud(cloudPosition2, HEIGHT - 50);
    drawCloud(cloudPosition3, HEIGHT - 120);

    drawRiver(0, HEIGHT * 0.4f);
    drawGround();

    drawFence(HEIGHT * 0.24f, 30.0f, 40.0f);
    drawRealisticTree(WIDTH * 0.15f, HEIGHT * 0.18f, 0.8f);
    drawRealisticTree(WIDTH * 0.05f, HEIGHT * 0.22f, 0.7f);
    drawRealisticTree(WIDTH * 0.25f, HEIGHT * 0.20f, 0.9f);

    drawRealisticTree(WIDTH * 0.45f, HEIGHT * 0.25f, 0.95f);
    drawRealisticTree(WIDTH * 0.35f, HEIGHT * 0.28f, 0.85f);
    drawRealisticTree(WIDTH * 0.55f, HEIGHT * 0.23f, 1.0f);

    drawRealisticTree(WIDTH * 0.75f, HEIGHT * 0.19f, 0.8f);
    drawRealisticTree(WIDTH * 0.85f, HEIGHT * 0.24f, 0.75f);
    drawRealisticTree(WIDTH * 0.65f, HEIGHT * 0.21f, 0.9f);

    drawHouse(WIDTH * 0.08f, HEIGHT * 0.12f, 0.95f, normalized_sun_y);
    drawHouse(WIDTH * 0.18f, HEIGHT * 0.15f, 1.0f, normalized_sun_y);
    drawHouse(WIDTH * 0.02f, HEIGHT * 0.10f, 0.85f, normalized_sun_y);

    drawHouse(WIDTH * 0.30f, HEIGHT * 0.18f, 1.1f, normalized_sun_y);
    drawHouse(WIDTH * 0.45f, HEIGHT * 0.15f, 1.05f, normalized_sun_y);
    drawHouse(WIDTH * 0.38f, HEIGHT * 0.16f, 0.9f, normalized_sun_y);

    drawHouse(WIDTH * 0.60f, HEIGHT * 0.17f, 0.98f, normalized_sun_y);
    drawHouse(WIDTH * 0.75f, HEIGHT * 0.18f, 0.88f, normalized_sun_y);
    drawHouse(WIDTH * 0.88f, HEIGHT * 0.19f, 0.8f, normalized_sun_y);


    drawRoad();
    drawDashedRoadLine();

    drawCar(carX, HEIGHT * 0.08f);


    drawBird(birdX, birdY, birdDirection);
    drawBird(birdX + 150, birdY - 50, birdDirection);
    drawBird(birdX + 300, birdY + 30, birdDirection);
    drawBird(birdX - 200, birdY + 100, !birdDirection);
    drawBird(birdX + 500, birdY - 20, !birdDirection);


    glutSwapBuffers();
}

void update(int value) {
    cloudPosition1 += 0.5f;
    if (cloudPosition1 > WIDTH + 100) cloudPosition1 = -150;

    cloudPosition2 += 0.3f;
    if (cloudPosition2 > WIDTH + 100) cloudPosition2 = -150;

    cloudPosition3 += 0.4f;
    if (cloudPosition3 > WIDTH + 100) cloudPosition3 = -150;

    if (birdWingUp) {
        birdWingAngle += birdFlapSpeed;
        if (birdWingAngle > 30.0f) {
            birdWingUp = false;
        }
    } else {
        birdWingAngle -= birdFlapSpeed;
        if (birdWingAngle < -30.0f) {
            birdWingUp = true;
        }
    }

    if (birdDirection) {
        birdX += 1.5f;
        if (birdX > WIDTH + 100) {
            birdDirection = false;
            birdX = WIDTH + 50;
            birdY = (float)(rand() % (HEIGHT / 4)) + (HEIGHT * 0.6f);
            birdFlapSpeed = 0.2f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.3f)));
        }
    } else {
        birdX -= 1.5f;
        if (birdX < -150) {
            birdDirection = true;
            birdX = -100;
            birdY = (float)(rand() % (HEIGHT / 4)) + (HEIGHT * 0.6f);
            birdFlapSpeed = 0.2f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.3f)));
        }
    }

    if (sunRising && !sunHasRisen) {
        sunY += 1.984f;
        if (sunY >= peakSunY) {
            sunY = peakSunY;
            sunHasRisen = true;
        }
    }

    float sun_travel_range = peakSunY - (-200.0f);
    float sun_trigger_y = -200.0f + (sun_travel_range * 0.9f);

    if (!carMoving && sunY >= sun_trigger_y) {
        carMoving = true;
        sndPlaySound("sound", SND_ASYNC);

    }

    if (carMoving) {
        carX += carSpeed;
        if (carX > WIDTH + 150) {
            carMoving = false;
        }
    }

    riverWaveOffset += 0.05f;


    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':
            break;
        case 'r':
        case 'R':
            sunY = -200.0f;
            sunHasRisen = false;
            sunRising = true;
            carX = -120.0f;
            carMoving = false;
            birdX = 0.0f;
            birdY = 400.0f;
            birdDirection = true;
            birdWingAngle = 0.0f;
            birdWingUp = true;
            birdFlapSpeed = 0.2f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.3f)));
            cloudPosition1 = 0.0f;
            cloudPosition2 = 300.0f;
            cloudPosition3 = 600.0f;
            riverWaveOffset = 0.0f;
            break;
        case 27:
            exit(0);
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Rural Village Scene - Bangladesh");

    init();

    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
