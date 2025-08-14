#define _USE_MATH_DEFINES // For M_PI
#include <windows.h>
#include <mmsystem.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#ifndef M_PI
    #define M_PI 3.14159265358979323846264338327950288
#endif


const GLfloat PI_CONSTANT = static_cast<GLfloat>(M_PI);


const int WINDOW_WIDTH = 793;
const int WINDOW_HEIGHT = 556;

// Layer Y-coordinates
const GLfloat WATER_BOTTOM_Y = 0.0f;
const GLfloat WATER_TOP_Y = 130.0f;
const GLfloat BRIDGE_BOTTOM_Y = WATER_TOP_Y;
const GLfloat BRIDGE_TOP_Y = BRIDGE_BOTTOM_Y + 80.0f;

// Barrier properties
const GLfloat BARRIER_COLOR_R = 228.0f / 255.0f;
const GLfloat BARRIER_COLOR_G = 177.0f / 255.0f;
const GLfloat BARRIER_COLOR_B = 93.0f / 255.0f;
const GLfloat BARRIER_HEIGHT = 15.0f;
const GLfloat BARRIER_LINE_THICKNESS = 2.0f;
const GLfloat BARRIER_VERTICAL_LINE_WIDTH = 2.0f;
const GLfloat BARRIER_VERTICAL_LINE_SPACING = 15.0f;

const GLfloat ROAD_BOTTOM_Y = BRIDGE_TOP_Y;
const GLfloat ROAD_TOP_Y = ROAD_BOTTOM_Y + 50.0f;
const GLfloat SKY_BOTTOM_Y = ROAD_TOP_Y;
const GLfloat SKY_TOP_Y = (GLfloat)WINDOW_HEIGHT;

// Arch properties
const int NUM_ARCHES = 4;
const GLfloat ARCH_RADIUS = 55.0f;
const int ARCH_SEGMENTS = 50;
const GLfloat ARCH_START_OFFSET = 80.0f;
const GLfloat ARCH_DISTANCE = 200.0f;

// Road stripe properties
const GLfloat ROAD_STRIPE_LEN = 30.0f;
const GLfloat ROAD_STRIPE_GAP = 20.0f;
const GLfloat ROAD_STRIPE_WIDTH = 2.0f;

// Water stripe properties
const GLfloat WATER_STRIPE_MIN_LEN = 20.0f;
const GLfloat WATER_STRIPE_MAX_LEN = 60.0f;
const GLfloat WATER_STRIPE_MIN_GAP = 10.0f;
const GLfloat WATER_STRIPE_MAX_GAP = 50.0f;
const GLfloat WATER_STRIPE_WIDTH = 3.0f;
const GLfloat WATER_STRIPE_VERT_SPACING = 15.0f;
const GLfloat WATER_STRIPE_RANDOM_Y_MAX = 2.0f;
const GLfloat WATER_STRIPE_DENSITY_FACTOR = 1.5f;
GLfloat waterStripeAnimationX = 0.0f;
const GLfloat WATER_STRIPE_FLOW_SPEED = 0.2f;

// Foreground ground properties
const GLfloat FG_GROUND_PEAK_HEIGHT = 50.0f;
const GLfloat GROUND_SINK_OFFSET = 2.0f;

// Tree/Bush properties
const GLfloat TREE_TRUNK_W = 10.0f;
const GLfloat TREE_TRUNK_H = 60.0f;
const GLfloat TREE_LEAVES_R = 30.0f;
const int TREE_LEAVES_SEG = 20;
const GLfloat TREE1_X = WINDOW_WIDTH * 0.55f;
const GLfloat TREE2_X = WINDOW_WIDTH * 0.62f;
const GLfloat TREE3_X = WINDOW_WIDTH * 0.70f;
const GLfloat TREE4_X = WINDOW_WIDTH * 0.78f;
const GLfloat TREE5_X = WINDOW_WIDTH * 0.86f;
const GLfloat TREE6_X = WINDOW_WIDTH * 0.94f;
const GLfloat BUSH1_X = WINDOW_WIDTH * 0.58f;
const GLfloat BUSH1_W = 120.0f;
const GLfloat BUSH1_H = 50.0f;
const GLfloat BUSH2_X = WINDOW_WIDTH * 0.75f;
const GLfloat BUSH2_W = 100.0f;
const GLfloat BUSH2_H = 40.0f;
const GLfloat BUSH_COLOR_R = 5.0f / 255.0f;
const GLfloat BUSH_COLOR_G = 46.0f / 255.0f;
const GLfloat BUSH_COLOR_B = 32.0f / 255.0f;
float leafOffset = 0.0f;

// Bridge stripe colors and structure
const GLfloat BRIDGE_STRIPE_COLOR1_R = 165.0f / 255.0f;
const GLfloat BRIDGE_STRIPE_COLOR1_G = 117.0f / 255.0f;
const GLfloat BRIDGE_STRIPE_COLOR1_B = 43.0f / 255.0f;
const GLfloat BRIDGE_STRIPE_COLOR2_R = 215.0f / 255.0f;
const GLfloat BRIDGE_STRIPE_COLOR2_G = 158.0f / 255.0f;
const GLfloat BRIDGE_STRIPE_COLOR2_B = 54.0f / 255.0f;
struct BridgeStripe {
    GLfloat x, y, length, width;
    GLfloat color_r, color_g, color_b;
};
const int NUM_STATIC_BRIDGE_STRIPES = 5;
BridgeStripe staticBridgeStripes[NUM_STATIC_BRIDGE_STRIPES];

// Car dimensions (original values for scaling reference)
const GLfloat ORIGINAL_CAR_BODY_WIDTH = 120.0f;
const GLfloat ORIGINAL_CAR_BODY_HEIGHT = 40.0f; // Used for scaling factor calculation
const GLfloat ORIGINAL_CAR_WHEEL_OUTER_RADIUS = 12.0f;


const GLfloat CAR_SCALE_FACTOR = (ROAD_TOP_Y - ROAD_BOTTOM_Y) / 3.0f / ORIGINAL_CAR_BODY_HEIGHT;


const GLfloat SCALED_CAR_BODY_WIDTH = ORIGINAL_CAR_BODY_WIDTH * CAR_SCALE_FACTOR;

const int MAX_CARS = 20;
struct Car {
    GLfloat x, y, speed, wheelAngle;
    GLfloat carColorR, carColorG, carColorB;
    int direction; // 1 for L-R, -1 for R-L
};
Car cars[MAX_CARS];

const GLfloat MIN_CAR_SPEED = 0.8f;
const GLfloat MAX_CAR_SPEED = 3.0f;
const GLfloat WHEEL_ROTATION_FACTOR = 0.5f;
const int SPEED_CHANGE_INTERVAL = 300;
int timerTickCount = 0;


const GLfloat HEADLIGHT_ORIGINAL_OFFSET_X = 115.0f;
const GLfloat HEADLIGHT_ORIGINAL_OFFSET_Y = 15.0f;
const GLfloat HEADLIGHT_ORIGINAL_WIDTH = 8.0f;
const GLfloat HEADLIGHT_ORIGINAL_HEIGHT = 5.0f;
const GLfloat HEADLIGHT_COLOR_R = 1.0f;
const GLfloat HEADLIGHT_COLOR_G = 1.0f;
const GLfloat HEADLIGHT_COLOR_B = 0.8f;


const GLfloat SHADOW_ORIGINAL_OFFSET_X = 10.0f;
const GLfloat SHADOW_ORIGINAL_OFFSET_Y = -15.0f;
const GLfloat SHADOW_ORIGINAL_WIDTH_FACTOR = 0.9f; // Factor of ORIGINAL_CAR_BODY_WIDTH
const GLfloat SHADOW_ORIGINAL_HEIGHT = 5.0f;
const GLfloat SHADOW_COLOR_A = 0.4f;

// Cityscape colors
const GLfloat CITYSCAPE_COLOR1_R = 85.0f / 255.0f, CITYSCAPE_COLOR1_G = 131.0f / 255.0f, CITYSCAPE_COLOR1_B = 129.0f / 255.0f;
const GLfloat CITYSCAPE_COLOR2_R = 141.0f / 255.0f, CITYSCAPE_COLOR2_G = 171.0f / 255.0f, CITYSCAPE_COLOR2_B = 168.0f / 255.0f;
const GLfloat CITYSCAPE_COLOR3_R = 177.0f / 255.0f, CITYSCAPE_COLOR3_G = 196.0f / 255.0f, CITYSCAPE_COLOR3_B = 196.0f / 255.0f;
const GLfloat GILDED_COLOR_1_R = 155.0f / 255.0f, GILDED_COLOR_1_G = 115.0f / 255.0f, GILDED_COLOR_1_B = 56.0f / 255.0f;
const GLfloat GILDED_COLOR_2_R = 203.0f / 255.0f, GILDED_COLOR_2_G = 171.0f / 255.0f, GILDED_COLOR_2_B = 120.0f / 255.0f;
const GLfloat GILDED_COLOR_3_R = 243.0f / 255.0f, GILDED_COLOR_3_G = 226.0f / 255.0f, GILDED_COLOR_3_B = 206.0f / 255.0f;
const GLfloat VICTORIAN_COLOR_1_R = 199.0f / 255.0f, VICTORIAN_COLOR_1_G = 170.0f / 255.0f, VICTORIAN_COLOR_1_B = 140.0f / 255.0f;
const GLfloat VICTORIAN_COLOR_2_R = 181.0f / 255.0f, VICTORIAN_COLOR_2_G = 139.0f / 255.0f, VICTORIAN_COLOR_2_B = 78.0f / 255.0f;
const GLfloat VICTORIAN_COLOR_3_R = 72.0f / 255.0f, VICTORIAN_COLOR_3_G = 48.0f / 255.0f, VICTORIAN_COLOR_3_B = 24.0f / 255.0f;
const GLfloat VICTORIAN_COLOR_4_R = 147.0f / 255.0f, VICTORIAN_COLOR_4_G = 116.0f / 255.0f, VICTORIAN_COLOR_4_B = 91.0f / 255.0f;
const GLfloat VICTORIAN_COLOR_5_R = 136.0f / 255.0f, VICTORIAN_COLOR_5_G = 107.0f / 255.0f, VICTORIAN_COLOR_5_B = 91.0f / 255.0f;

// Building types
const int BUILDING_TYPE_RECT = 0;
const int BUILDING_TYPE_SETBACK = 1;
const int BUILDING_TYPE_POINTED = 2;
const int BUILDING_TYPE_GILDED = 3;
const int BUILDING_TYPE_VICTORIAN = 4;
struct Building {
    GLfloat x, y, width, height;
    GLfloat colorR, colorG, colorB;
    int type;
};
const int NUM_BUILDINGS = 40;
Building buildings[NUM_BUILDINGS];

// Cloud properties
const int MAX_CLOUD_PACKS = 7;
const int MAX_CLOUDS_PER_PACK_MAX = 4;
const GLfloat CLOUD_PACK_SPEED_MIN = 0.3f;
const GLfloat CLOUD_PACK_SPEED_MAX = 1.0f;
struct Cloud {
    GLfloat x_offset, y_offset, size;
    GLfloat r, g, b, a;
};
struct CloudPack {
    GLfloat base_x, base_y, speed;
    int numCloudsInPack;
    Cloud individualClouds[MAX_CLOUDS_PER_PACK_MAX];
};
CloudPack cloudPacks[MAX_CLOUD_PACKS];


const int MAX_BIRD_GROUPS = 5;
const int MAX_BIRDS_PER_GROUP = 10;
const GLfloat BIRD_GROUP_SPEED_MIN = 1.5f;
const GLfloat BIRD_GROUP_SPEED_MAX = 4.0f;
const GLfloat BIRD_SIZE = 5.0f;
const GLfloat WING_FLAP_SPEED = 0.1f;
struct Bird {
    GLfloat x_offset, y_offset, wing_angle;
};
struct BirdGroup {
    GLfloat base_x, base_y, speed;
    int direction, numBirdsInGroup;
    Bird individualBirds[MAX_BIRDS_PER_GROUP];
};
BirdGroup birdGroups[MAX_BIRD_GROUPS];


const GLfloat SUNSET_HORIZON_R = 255.0f / 255.0f, SUNSET_HORIZON_G = 100.0f / 255.0f, SUNSET_HORIZON_B = 0.0f / 255.0f;
const GLfloat SUNSET_TOP_R = 150.0f / 255.0f, SUNSET_TOP_G = 200.0f / 255.0f, SUNSET_TOP_B = 255.0f / 255.0f;
const GLfloat CLOUD_COLOR_BASE_R = 0.95f, CLOUD_COLOR_BASE_G = 0.95f, CLOUD_COLOR_BASE_B = 0.95f;
const GLfloat CLOUD_COLOR_SHADE_R = 0.8f, CLOUD_COLOR_SHADE_G = 0.8f, CLOUD_COLOR_SHADE_B = 0.8f;
const GLfloat SUN_RADIUS = 40.0f;
const GLfloat SUN_COLOR_R = 255.0f / 255.0f, SUN_COLOR_G = 100.0f / 255.0f, SUN_COLOR_B = 0.0f / 255.0f;


void drawFilledArch(GLfloat centerX, GLfloat bottomY, GLfloat radius, int segments);
void drawCloud(GLfloat x, GLfloat y, GLfloat size, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void drawSun(GLfloat centerX, GLfloat centerY, GLfloat radius, int segments);
void drawBird(GLfloat x, GLfloat y, GLfloat wing_angle, int direction);
GLfloat getGroundYAtX(GLfloat x_pos);


GLfloat randomFloat(GLfloat min, GLfloat max) {
    return min + (static_cast<GLfloat>(rand()) / RAND_MAX) * (max - min);
}


void _drawCarShadowGeometry() {
    glColor4f(0.0f, 0.0f, 0.0f, SHADOW_COLOR_A); // Translucent black
    glBegin(GL_QUADS);
        glVertex2f(SHADOW_ORIGINAL_OFFSET_X * CAR_SCALE_FACTOR, SHADOW_ORIGINAL_OFFSET_Y * CAR_SCALE_FACTOR);
        glVertex2f((SHADOW_ORIGINAL_OFFSET_X + ORIGINAL_CAR_BODY_WIDTH * SHADOW_ORIGINAL_WIDTH_FACTOR) * CAR_SCALE_FACTOR, SHADOW_ORIGINAL_OFFSET_Y * CAR_SCALE_FACTOR);
        glVertex2f((SHADOW_ORIGINAL_OFFSET_X + ORIGINAL_CAR_BODY_WIDTH * SHADOW_ORIGINAL_WIDTH_FACTOR) * CAR_SCALE_FACTOR, (SHADOW_ORIGINAL_OFFSET_Y + SHADOW_ORIGINAL_HEIGHT) * CAR_SCALE_FACTOR);
        glVertex2f(SHADOW_ORIGINAL_OFFSET_X * CAR_SCALE_FACTOR, (SHADOW_ORIGINAL_OFFSET_Y + SHADOW_ORIGINAL_HEIGHT) * CAR_SCALE_FACTOR);
    glEnd();
}


void _drawCarHeadlightGeometry() {
    glColor3f(HEADLIGHT_COLOR_R, HEADLIGHT_COLOR_G, HEADLIGHT_COLOR_B);
    glBegin(GL_QUADS);
        glVertex2f(HEADLIGHT_ORIGINAL_OFFSET_X * CAR_SCALE_FACTOR, HEADLIGHT_ORIGINAL_OFFSET_Y * CAR_SCALE_FACTOR);
        glVertex2f((HEADLIGHT_ORIGINAL_OFFSET_X + HEADLIGHT_ORIGINAL_WIDTH) * CAR_SCALE_FACTOR, HEADLIGHT_ORIGINAL_OFFSET_Y * CAR_SCALE_FACTOR);
        glVertex2f((HEADLIGHT_ORIGINAL_OFFSET_X + HEADLIGHT_ORIGINAL_WIDTH) * CAR_SCALE_FACTOR, (HEADLIGHT_ORIGINAL_OFFSET_Y + HEADLIGHT_ORIGINAL_HEIGHT) * CAR_SCALE_FACTOR);
        glVertex2f(HEADLIGHT_ORIGINAL_OFFSET_X * CAR_SCALE_FACTOR, (HEADLIGHT_ORIGINAL_OFFSET_Y + HEADLIGHT_ORIGINAL_HEIGHT) * CAR_SCALE_FACTOR);
    glEnd();
}


void _drawCarBodyAndWheels(float wheelAngle, GLfloat r, GLfloat g, GLfloat b) {
    // Car body
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
        glVertex2f(ORIGINAL_CAR_BODY_WIDTH * CAR_SCALE_FACTOR, 0);
        glVertex2f(0, 0);
        glVertex2f(0, 20 * CAR_SCALE_FACTOR);
        glVertex2f(20 * CAR_SCALE_FACTOR, 40 * CAR_SCALE_FACTOR);
        glVertex2f(80 * CAR_SCALE_FACTOR, 40 * CAR_SCALE_FACTOR);
        glVertex2f(100 * CAR_SCALE_FACTOR, 20 * CAR_SCALE_FACTOR);
        glVertex2f(ORIGINAL_CAR_BODY_WIDTH * CAR_SCALE_FACTOR, 20 * CAR_SCALE_FACTOR);
    glEnd();

    // Windows (light blue)
    glColor3f(0.6f, 0.8f, 1.0f);
    glBegin(GL_POLYGON); // Front Window
        glVertex2f(12 * CAR_SCALE_FACTOR, 22 * CAR_SCALE_FACTOR);
        glVertex2f(25 * CAR_SCALE_FACTOR, 38 * CAR_SCALE_FACTOR);
        glVertex2f(55 * CAR_SCALE_FACTOR, 38 * CAR_SCALE_FACTOR);
        glVertex2f(55 * CAR_SCALE_FACTOR, 22 * CAR_SCALE_FACTOR);
    glEnd();
    glBegin(GL_POLYGON); // Rear Window
        glVertex2f(60 * CAR_SCALE_FACTOR, 22 * CAR_SCALE_FACTOR);
        glVertex2f(60 * CAR_SCALE_FACTOR, 38 * CAR_SCALE_FACTOR);
        glVertex2f(82 * CAR_SCALE_FACTOR, 38 * CAR_SCALE_FACTOR);
        glVertex2f(98 * CAR_SCALE_FACTOR, 22 * CAR_SCALE_FACTOR);
    glEnd();

    // Wheels
    for (int i = 0; i < 2; i++) {

        float cx_original = (25.0f + i * 70.0f);
        float cy_original = -2.0f;

        // Scaled positions
        float cx_scaled = cx_original * CAR_SCALE_FACTOR;
        float cy_scaled = cy_original * CAR_SCALE_FACTOR;

        float radiusOuter_scaled = ORIGINAL_CAR_WHEEL_OUTER_RADIUS * CAR_SCALE_FACTOR;
        float radiusInner_scaled = 6.0f * CAR_SCALE_FACTOR;

        glPushMatrix();

        glTranslatef(cx_scaled, cy_scaled, 0.0f);
        glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);


        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0.0f, 0.0f);
            for (int j = 0; j <= 100; ++j) {
                float angle = 2.0f * PI_CONSTANT * j / 100.0f;
                glVertex2f(cos(angle) * radiusOuter_scaled, sin(angle) * radiusOuter_scaled);
            }
        glEnd();


        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0.0f, 0.0f);
            for (int j = 0; j <= 100; ++j) {
                float angle = 2.0f * PI_CONSTANT * j / 100.0f;
                glVertex2f(cos(angle) * radiusInner_scaled, sin(angle) * radiusInner_scaled);
            }
        glEnd();
        glPopMatrix();
    }
}


void drawAnimatedCar(float x, float y, float wheelAngle, GLfloat r, GLfloat g, GLfloat b, int direction) {
    // Draw Shadow Part
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    if (direction == -1) {
        glScalef(-1.0f, 1.0f, 1.0f);

        glTranslatef(-SCALED_CAR_BODY_WIDTH, 0.0f, 0.0f);
    }
    _drawCarShadowGeometry();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    if (direction == -1) {
        glScalef(-1.0f, 1.0f, 1.0f);
        glTranslatef(-SCALED_CAR_BODY_WIDTH, 0.0f, 0.0f);
    }
    _drawCarBodyAndWheels(wheelAngle, r, g, b);
    _drawCarHeadlightGeometry();
    glPopMatrix();
}


// --- Other Drawing Functions (Unchanged except for PI_CONSTANT usage) ---

void drawFilledArch(GLfloat centerX, GLfloat bottomY, GLfloat radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centerX, bottomY);
        for (int i = 0; i <= segments; ++i) {
            float angle = PI_CONSTANT * i / segments; // Use PI_CONSTANT
            glVertex2f(centerX + radius * cos(angle), bottomY + radius * sin(angle));
        }
    glEnd();
}

void drawBuilding(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat r, GLfloat g, GLfloat b, int type) {
    glColor3f(r, g, b);
    if (type == BUILDING_TYPE_RECT) {
        glBegin(GL_QUADS);
            glVertex2f(x, y); glVertex2f(x + width, y);
            glVertex2f(x + width, y + height); glVertex2f(x, y + height);
        glEnd();
    } else if (type == BUILDING_TYPE_SETBACK) {
        GLfloat base_h = height * 0.7f, top_w = width * 0.7f, top_h = height * 0.3f, top_x_off = (width - top_w) / 2.0f;
        glBegin(GL_QUADS); // Base
            glVertex2f(x, y); glVertex2f(x + width, y);
            glVertex2f(x + width, y + base_h); glVertex2f(x, y + base_h);
        glEnd();
        glBegin(GL_QUADS); // Top
            glVertex2f(x + top_x_off, y + base_h); glVertex2f(x + top_x_off + top_w, y + base_h);
            glVertex2f(x + top_x_off + top_w, y + base_h + top_h); glVertex2f(x + top_x_off, y + base_h + top_h);
        glEnd();
    } else if (type == BUILDING_TYPE_POINTED) {
        GLfloat base_h = height * 0.85f, peak_h = height * 0.15f;
        glBegin(GL_QUADS); // Base
            glVertex2f(x, y); glVertex2f(x + width, y);
            glVertex2f(x + width, y + base_h); glVertex2f(x, y + base_h);
        glEnd();
        glBegin(GL_TRIANGLES); // Peak
            glVertex2f(x, y + base_h); glVertex2f(x + width, y + base_h);
            glVertex2f(x + width / 2.0f, y + base_h + peak_h);
        glEnd();
    } else if (type == BUILDING_TYPE_GILDED) {
        glBegin(GL_QUADS); // Main Body
            glVertex2f(x, y); glVertex2f(x + width, y);
            glVertex2f(x + width, y + height); glVertex2f(x, y + height);
        glEnd();
        GLfloat cornice_h = height * 0.08f, cornice_inset = width * 0.05f;
        glColor3f(r * 0.9f, g * 0.9f, b * 0.9f); // Cornice
        glBegin(GL_QUADS);
            glVertex2f(x - cornice_inset, y + height - cornice_h); glVertex2f(x + width + cornice_inset, y + height - cornice_h);
            glVertex2f(x + width + cornice_inset, y + height); glVertex2f(x - cornice_inset, y + height);
        glEnd();
        GLfloat dome_r = width * 0.2f, dome_cx = x + width / 2.0f, dome_cy = y + height;
        glColor3f(r * 0.7f, g * 0.7f, b * 0.7f); // Dome
        drawFilledArch(dome_cx, dome_cy, dome_r, 20);
        glBegin(GL_QUADS); // Dome Base
            glVertex2f(dome_cx - dome_r, dome_cy); glVertex2f(dome_cx + dome_r, dome_cy);
            glVertex2f(dome_cx + dome_r, dome_cy + dome_r * 0.2f); glVertex2f(dome_cx - dome_r, dome_cy + dome_r * 0.2f);
        glEnd();
    } else if (type == BUILDING_TYPE_VICTORIAN) {
        glBegin(GL_QUADS); // Main body
            glVertex2f(x, y); glVertex2f(x + width, y);
            glVertex2f(x + width, y + height * 0.7f); glVertex2f(x, y + height * 0.7f);
        glEnd();
        glColor3f(r * 0.8f, g * 0.8f, b * 0.8f); // Roof
        glBegin(GL_TRIANGLES);
            glVertex2f(x, y + height * 0.7f); glVertex2f(x + width, y + height * 0.7f);
            glVertex2f(x + width / 2.0f, y + height);
        glEnd();
        GLfloat turret_w = width * 0.25f, turret_h = height * 0.3f, turret_x = x + width * 0.7f, turret_y = y + height * 0.5f;
        glColor3f(r * 0.95f, g * 0.95f, b * 0.95f); // Turret
        glBegin(GL_QUADS);
            glVertex2f(turret_x, turret_y); glVertex2f(turret_x + turret_w, turret_y);
            glVertex2f(turret_x + turret_w, turret_y + turret_h); glVertex2f(turret_x, turret_y + turret_h);
        glEnd();
        glColor3f(r * 0.7f, g * 0.7f, b * 0.7f); // Turret Roof
        glBegin(GL_TRIANGLES);
            glVertex2f(turret_x, turret_y + turret_h); glVertex2f(turret_x + turret_w, turret_y + turret_h);
            glVertex2f(turret_x + turret_w / 2.0f, turret_y + turret_h + height * 0.1f);
        glEnd();
        GLfloat bay_w = width * 0.3f, bay_h = height * 0.2f, bay_x = x + width * 0.1f, bay_y = y + height * 0.1f;
        glColor3f(r * 0.9f, g * 0.9f, b * 0.9f); // Bay Window
        glBegin(GL_QUADS);
            glVertex2f(bay_x, bay_y); glVertex2f(bay_x + bay_w, bay_y);
            glVertex2f(bay_x + bay_w, bay_y + bay_h); glVertex2f(bay_x, bay_y + bay_h);
        glEnd();
    }

    // Windows
    glColor3f(r * 0.8f, g * 0.8f, b * 0.8f);
    GLfloat win_w = width * 0.1f, win_h = height * 0.07f, h_gap = width * 0.05f, v_gap = height * 0.05f;
    GLfloat win_top_bound = y + height;
    if (type == BUILDING_TYPE_SETBACK) win_top_bound = y + height * 0.7f;
    if (type == BUILDING_TYPE_POINTED) win_top_bound = y + height * 0.85f;
    if (type == BUILDING_TYPE_GILDED) win_top_bound = y + height - (height * 0.1f);
    if (type == BUILDING_TYPE_VICTORIAN) win_top_bound = y + height * 0.7f - v_gap;

    for (GLfloat cy = y + v_gap; cy < win_top_bound - win_h; cy += (win_h + v_gap)) {
        for (GLfloat cx = x + h_gap; cx < x + width - win_w; cx += (win_w + h_gap)) {
            if (type == BUILDING_TYPE_GILDED && (cy + win_h > y + height - (height * 0.1f))) continue;
            if (type == BUILDING_TYPE_VICTORIAN && (cy + win_h > y + height * 0.7f)) continue;
            glBegin(GL_QUADS);
                glVertex2f(cx, cy); glVertex2f(cx + win_w, cy);
                glVertex2f(cx + win_w, cy + win_h); glVertex2f(cx, cy + win_h);
            glEnd();
        }
    }
}

void drawTree(GLfloat x, GLfloat y, bool isYellow) {
    glColor3f(139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f); // Trunk
    glBegin(GL_QUADS);
        glVertex2f(x - TREE_TRUNK_W / 2.0f, y); glVertex2f(x + TREE_TRUNK_W / 2.0f, y);
        glVertex2f(x + TREE_TRUNK_W / 2.0f, y + TREE_TRUNK_H); glVertex2f(x - TREE_TRUNK_W / 2.0f, y + TREE_TRUNK_H);
    glEnd();

    if (isYellow) glColor3f(255.0f / 255.0f, 165.0f / 255.0f, 0.0f / 255.0f);
    else glColor3f(210.0f / 255.0f, 105.0f / 255.0f, 30.0f / 255.0f);

    GLfloat base_leaves_y = y + TREE_TRUNK_H;

    auto drawLeafCircle = [&](GLfloat cx_offset, GLfloat cy_offset, GLfloat radius_scale) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x + leafOffset + cx_offset, base_leaves_y + cy_offset);
        for (int i = 0; i <= TREE_LEAVES_SEG; ++i) {
            float angle = 2.0f * PI_CONSTANT * i / TREE_LEAVES_SEG; // Use PI_CONSTANT
            glVertex2f(x + leafOffset + cx_offset + TREE_LEAVES_R * radius_scale * cos(angle),
                       base_leaves_y + cy_offset + TREE_LEAVES_R * radius_scale * sin(angle));
        }
        glEnd();
    };
    drawLeafCircle(0.0f, TREE_LEAVES_R * 0.5f, 1.0f);
    drawLeafCircle(TREE_LEAVES_R * 0.3f, TREE_LEAVES_R * 0.8f, 0.8f);
    drawLeafCircle(-TREE_LEAVES_R * 0.3f, TREE_LEAVES_R * 0.8f, 0.8f);
    drawLeafCircle(0.0f, TREE_LEAVES_R * 1.5f, 0.6f);
}

void drawBush(GLfloat x, GLfloat y, GLfloat width, GLfloat height) {
    glColor3f(BUSH_COLOR_R, BUSH_COLOR_G, BUSH_COLOR_B);
    glBegin(GL_POLYGON);
        glVertex2f(x, y); glVertex2f(x + width, y);
        int segments = 20;
        for (int i = segments; i >= 0; --i) {
            float t = (float)i / segments;
            float current_x = x + width * t;
            float curve_y_offset = height * (1.0f - (t - 0.5f) * (t - 0.5f) * 4.0f);
            glVertex2f(current_x, y + curve_y_offset);
        }
    glEnd();
}

void drawForegroundGround() {
    glColor4f(50.0f / 255.0f, 100.0f / 255.0f, 50.0f / 255.0f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, WATER_BOTTOM_Y); glVertex2f((GLfloat)WINDOW_WIDTH, WATER_BOTTOM_Y);
        int segments = 100;
        for (int i = segments; i >= 0; --i) {
            float t = (float)i / segments;
            float x_pos = (GLfloat)WINDOW_WIDTH * t;
            float y_offset = FG_GROUND_PEAK_HEIGHT * (0.5f + 0.5f * sin(t * PI_CONSTANT * 3.0f));
            glVertex2f(x_pos, WATER_BOTTOM_Y + y_offset);
        }
    glEnd();
}

GLfloat getGroundYAtX(GLfloat x_pos) {
    GLfloat normalized_t = x_pos / (GLfloat)WINDOW_WIDTH;
    if (normalized_t < 0.0f) normalized_t = 0.0f;
    if (normalized_t > 1.0f) normalized_t = 1.0f;
    float y_offset = FG_GROUND_PEAK_HEIGHT * (0.5f + 0.5f * sin(normalized_t * PI_CONSTANT * 3.0f));
    return WATER_BOTTOM_Y + y_offset;
}

void drawCloud(GLfloat x, GLfloat y, GLfloat size, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    glColor4f(r, g, b, a);
    glPushMatrix();
    glTranslatef(x, y, 0);
    drawFilledArch(0, 0, 12 * size, 20);
    drawFilledArch(15 * size, 0, 17 * size, 20);
    drawFilledArch(7 * size, 10 * size, 15 * size, 20);
    glPopMatrix();
}

void drawSun(GLfloat centerX, GLfloat centerY, GLfloat radius, int segments) {
    glColor3f(SUN_COLOR_R, SUN_COLOR_G, SUN_COLOR_B);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centerX, centerY);
        for (int i = 0; i <= segments; ++i) {
            float angle = 2 * PI_CONSTANT * i / segments;
            glVertex2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
        }
    glEnd();
}

void drawBird(GLfloat x, GLfloat y, GLfloat wing_angle, int direction) {
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    if (direction == -1) glScalef(-1.0f, 1.0f, 1.0f);
    glRotatef(wing_angle, 0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2f(-BIRD_SIZE, 0.0f); glVertex2f(0.0f, BIRD_SIZE);
        glVertex2f(0.0f, BIRD_SIZE); glVertex2f(BIRD_SIZE, 0.0f);
    glEnd();
    glPopMatrix();
}

// OpenGL initialization
void initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize static bridge stripes
    for (int i = 0; i < NUM_STATIC_BRIDGE_STRIPES; ++i) {
        staticBridgeStripes[i].y = randomFloat(BRIDGE_BOTTOM_Y + 5.0f, BRIDGE_TOP_Y - 5.0f);
        staticBridgeStripes[i].length = randomFloat(30.0f, 100.0f);
        staticBridgeStripes[i].x = randomFloat(0.0f, (GLfloat)WINDOW_WIDTH - staticBridgeStripes[i].length);
        if (i % 3 == 0) staticBridgeStripes[i].width = 5.0f;
        else if (i % 3 == 1) staticBridgeStripes[i].width = 10.0f;
        else staticBridgeStripes[i].width = 15.0f;
        if (i % 2 == 0) {
            staticBridgeStripes[i].color_r = BRIDGE_STRIPE_COLOR1_R; staticBridgeStripes[i].color_g = BRIDGE_STRIPE_COLOR1_G; staticBridgeStripes[i].color_b = BRIDGE_STRIPE_COLOR1_B;
        } else {
            staticBridgeStripes[i].color_r = BRIDGE_STRIPE_COLOR2_R; staticBridgeStripes[i].color_g = BRIDGE_STRIPE_COLOR2_G; staticBridgeStripes[i].color_b = BRIDGE_STRIPE_COLOR2_B;
        }
    }

    // Initialize cars
    GLfloat road_half_height = (ROAD_TOP_Y - ROAD_BOTTOM_Y) / 2.0f;
    GLfloat lower_lane_y_center = ROAD_BOTTOM_Y + road_half_height * 0.5f;
    GLfloat upper_lane_y_center = ROAD_BOTTOM_Y + road_half_height * 1.5f;
    // Car's y is its bottom edge, so adjust from lane center by half of its scaled height.
    // ORIGINAL_CAR_BODY_HEIGHT * CAR_SCALE_FACTOR is the scaled height.
    GLfloat scaled_car_actual_height = ORIGINAL_CAR_BODY_HEIGHT * CAR_SCALE_FACTOR;


    for (int i = 0; i < MAX_CARS; ++i) {
        cars[i].speed = randomFloat(MIN_CAR_SPEED, MAX_CAR_SPEED);
        cars[i].wheelAngle = 0.0f;
        cars[i].carColorR = randomFloat(0.3f, 1.0f);
        cars[i].carColorG = randomFloat(0.3f, 1.0f);
        cars[i].carColorB = randomFloat(0.3f, 1.0f);

        if (i % 2 == 0) { // Upper lane
            cars[i].direction = 1;

            cars[i].y = upper_lane_y_center - (scaled_car_actual_height / 2.0f);
            cars[i].x = -randomFloat(0.0f, WINDOW_WIDTH) - (i * (SCALED_CAR_BODY_WIDTH + 50.0f));
        } else { // Lower lane
            cars[i].direction = -1;
            cars[i].y = lower_lane_y_center - (scaled_car_actual_height / 2.0f);
            cars[i].x = WINDOW_WIDTH + randomFloat(0.0f, WINDOW_WIDTH) + (i * (SCALED_CAR_BODY_WIDTH + 50.0f));
        }
    }

    // Initialize buildings
    const GLfloat MAX_BUILDING_HEIGHT = (SKY_TOP_Y - SKY_BOTTOM_Y) / 2.0f;
    GLfloat current_city_x = 0.0f;
    for (int i = 0; i < NUM_BUILDINGS; ++i) {
        buildings[i].width = randomFloat(50.0f, 120.0f);
        buildings[i].height = randomFloat(50.0f, MAX_BUILDING_HEIGHT);
        buildings[i].x = current_city_x;
        buildings[i].y = SKY_BOTTOM_Y;
        int type_choice = rand() % 10;
        if (type_choice < 2) buildings[i].type = BUILDING_TYPE_RECT;
        else if (type_choice < 4) buildings[i].type = BUILDING_TYPE_SETBACK;
        else if (type_choice < 6) buildings[i].type = BUILDING_TYPE_POINTED;
        else if (type_choice < 8) buildings[i].type = BUILDING_TYPE_GILDED;
        else buildings[i].type = BUILDING_TYPE_VICTORIAN;

        if (buildings[i].type == BUILDING_TYPE_GILDED) {
            int c = rand() % 3; if(c==0){buildings[i].colorR=GILDED_COLOR_1_R;buildings[i].colorG=GILDED_COLOR_1_G;buildings[i].colorB=GILDED_COLOR_1_B;} else if(c==1){buildings[i].colorR=GILDED_COLOR_2_R;buildings[i].colorG=GILDED_COLOR_2_G;buildings[i].colorB=GILDED_COLOR_2_B;} else{buildings[i].colorR=GILDED_COLOR_3_R;buildings[i].colorG=GILDED_COLOR_3_G;buildings[i].colorB=GILDED_COLOR_3_B;}
        } else if (buildings[i].type == BUILDING_TYPE_VICTORIAN) {
            int c = rand() % 5; if(c==0){buildings[i].colorR=VICTORIAN_COLOR_1_R;buildings[i].colorG=VICTORIAN_COLOR_1_G;buildings[i].colorB=VICTORIAN_COLOR_1_B;} else if(c==1){buildings[i].colorR=VICTORIAN_COLOR_2_R;buildings[i].colorG=VICTORIAN_COLOR_2_G;buildings[i].colorB=VICTORIAN_COLOR_2_B;} else if(c==2){buildings[i].colorR=VICTORIAN_COLOR_3_R;buildings[i].colorG=VICTORIAN_COLOR_3_G;buildings[i].colorB=VICTORIAN_COLOR_3_B;} else if(c==3){buildings[i].colorR=VICTORIAN_COLOR_4_R;buildings[i].colorG=VICTORIAN_COLOR_4_G;buildings[i].colorB=VICTORIAN_COLOR_4_B;} else{buildings[i].colorR=VICTORIAN_COLOR_5_R;buildings[i].colorG=VICTORIAN_COLOR_5_G;buildings[i].colorB=VICTORIAN_COLOR_5_B;}
        } else { /* Original cityscape colors */
            int c = rand() % 3; if(c==0){buildings[i].colorR=CITYSCAPE_COLOR1_R;buildings[i].colorG=CITYSCAPE_COLOR1_G;buildings[i].colorB=CITYSCAPE_COLOR1_B;} else if(c==1){buildings[i].colorR=CITYSCAPE_COLOR2_R;buildings[i].colorG=CITYSCAPE_COLOR2_G;buildings[i].colorB=CITYSCAPE_COLOR2_B;} else{buildings[i].colorR=CITYSCAPE_COLOR3_R;buildings[i].colorG=CITYSCAPE_COLOR3_G;buildings[i].colorB=CITYSCAPE_COLOR3_B;}
        }
        current_city_x += randomFloat(buildings[i].width * 0.3f, buildings[i].width * 0.8f);
        if (current_city_x > WINDOW_WIDTH) current_city_x = 0.0f;
    }

    // Initialize clouds
    for (int i = 0; i < MAX_CLOUD_PACKS; ++i) {
        cloudPacks[i].base_x = randomFloat(0, WINDOW_WIDTH);
        cloudPacks[i].base_y = randomFloat(SKY_BOTTOM_Y + 50.0f, SKY_TOP_Y - 50.0f);
        cloudPacks[i].speed = randomFloat(CLOUD_PACK_SPEED_MIN, CLOUD_PACK_SPEED_MAX);
        cloudPacks[i].numCloudsInPack = rand() % (MAX_CLOUDS_PER_PACK_MAX - 1) + 2;
        for (int j = 0; j < cloudPacks[i].numCloudsInPack; ++j) {
            cloudPacks[i].individualClouds[j].x_offset = randomFloat(-50.0f, 50.0f);
            cloudPacks[i].individualClouds[j].y_offset = randomFloat(-20.0f, 20.0f);
            cloudPacks[i].individualClouds[j].size = randomFloat(0.2f, 0.5f);
            cloudPacks[i].individualClouds[j].r = randomFloat(CLOUD_COLOR_SHADE_R, CLOUD_COLOR_BASE_R);
            cloudPacks[i].individualClouds[j].g = randomFloat(CLOUD_COLOR_SHADE_G, CLOUD_COLOR_BASE_G);
            cloudPacks[i].individualClouds[j].b = randomFloat(CLOUD_COLOR_SHADE_B, CLOUD_COLOR_BASE_B);
            cloudPacks[i].individualClouds[j].a = randomFloat(0.9f, 1.0f);
        }
    }

    // Initialize bird groups
    for (int i = 0; i < MAX_BIRD_GROUPS; ++i) {
        birdGroups[i].speed = randomFloat(BIRD_GROUP_SPEED_MIN, BIRD_GROUP_SPEED_MAX);
        birdGroups[i].direction = (rand() % 2 == 0) ? 1 : -1;
        birdGroups[i].numBirdsInGroup = rand() % (MAX_BIRDS_PER_GROUP - 1) + 2;
        if (birdGroups[i].direction == 1) birdGroups[i].base_x = -randomFloat(50.0f, 200.0f);
        else birdGroups[i].base_x = WINDOW_WIDTH + randomFloat(50.0f, 200.0f);
        birdGroups[i].base_y = randomFloat(SKY_BOTTOM_Y + 10.0f, SKY_TOP_Y - 10.0f);
        for (int j = 0; j < birdGroups[i].numBirdsInGroup; ++j) {
            birdGroups[i].individualBirds[j].x_offset = randomFloat(-20.0f, 20.0f);
            birdGroups[i].individualBirds[j].y_offset = randomFloat(-10.0f, 10.0f);
            birdGroups[i].individualBirds[j].wing_angle = 0.0f;
        }
    }
}

// Timer function for animation updates
void timer(int value) {
    leafOffset = 5.0f * sin(value * 0.02f); // Leaf movement

    timerTickCount++; // Car speed changes and movement
    for (int i = 0; i < MAX_CARS; ++i) {
        cars[i].x += cars[i].speed * cars[i].direction;
        cars[i].wheelAngle -= cars[i].speed * WHEEL_ROTATION_FACTOR * cars[i].direction;
        if (cars[i].direction == 1 && cars[i].x > WINDOW_WIDTH) {
            cars[i].x = -SCALED_CAR_BODY_WIDTH; cars[i].speed = randomFloat(MIN_CAR_SPEED, MAX_CAR_SPEED);
        } else if (cars[i].direction == -1 && cars[i].x < -SCALED_CAR_BODY_WIDTH) {
            cars[i].x = WINDOW_WIDTH; cars[i].speed = randomFloat(MIN_CAR_SPEED, MAX_CAR_SPEED);
        }
        if (timerTickCount % SPEED_CHANGE_INTERVAL == 0) {
            cars[i].speed = randomFloat(MIN_CAR_SPEED, MAX_CAR_SPEED);
        }
    }

    // Cloud pack movement and regeneration
    for (int i = 0; i < MAX_CLOUD_PACKS; ++i) {
        cloudPacks[i].base_x += cloudPacks[i].speed;
        GLfloat max_pack_width = 0.0f;
        for(int j = 0; j < cloudPacks[i].numCloudsInPack; ++j) {
            GLfloat cloud_right_edge = cloudPacks[i].individualClouds[j].x_offset + 35.0f * cloudPacks[i].individualClouds[j].size;
            if (cloud_right_edge > max_pack_width) max_pack_width = cloud_right_edge;
        }
        if (cloudPacks[i].base_x > WINDOW_WIDTH + max_pack_width) {
            cloudPacks[i].base_x = -max_pack_width - randomFloat(50.0f, 150.0f);
            cloudPacks[i].base_y = randomFloat(SKY_BOTTOM_Y + 50.0f, SKY_TOP_Y - 50.0f);
            cloudPacks[i].speed = randomFloat(CLOUD_PACK_SPEED_MIN, CLOUD_PACK_SPEED_MAX);
            cloudPacks[i].numCloudsInPack = rand() % (MAX_CLOUDS_PER_PACK_MAX - 1) + 2;
            for (int j = 0; j < cloudPacks[i].numCloudsInPack; ++j) { /* Re-init individual clouds */
                cloudPacks[i].individualClouds[j].x_offset = randomFloat(-50.0f, 50.0f); cloudPacks[i].individualClouds[j].y_offset = randomFloat(-20.0f, 20.0f); cloudPacks[i].individualClouds[j].size = randomFloat(0.2f, 0.5f); cloudPacks[i].individualClouds[j].r = randomFloat(CLOUD_COLOR_SHADE_R, CLOUD_COLOR_BASE_R); cloudPacks[i].individualClouds[j].g = randomFloat(CLOUD_COLOR_SHADE_G, CLOUD_COLOR_BASE_G); cloudPacks[i].individualClouds[j].b = randomFloat(CLOUD_COLOR_SHADE_B, CLOUD_COLOR_BASE_B); cloudPacks[i].individualClouds[j].a = randomFloat(0.9f, 1.0f);
            }
        }
    }

    // Bird group movement and animation
    for (int i = 0; i < MAX_BIRD_GROUPS; ++i) {
        birdGroups[i].base_x += birdGroups[i].speed * birdGroups[i].direction;
        for (int j = 0; j < birdGroups[i].numBirdsInGroup; ++j) {
            birdGroups[i].individualBirds[j].wing_angle = 15.0f * sin(value * WING_FLAP_SPEED + j * 0.5f);
        }
        if ((birdGroups[i].direction == 1 && birdGroups[i].base_x > WINDOW_WIDTH + 100) ||
            (birdGroups[i].direction == -1 && birdGroups[i].base_x < -100)) {
            if (birdGroups[i].direction == 1) birdGroups[i].base_x = -100.0f - randomFloat(0.0f, 100.0f);
            else birdGroups[i].base_x = WINDOW_WIDTH + 100.0f + randomFloat(0.0f, 100.0f);
            birdGroups[i].base_y = randomFloat(SKY_BOTTOM_Y + 10.0f, SKY_TOP_Y - 10.0f);
            birdGroups[i].speed = randomFloat(BIRD_GROUP_SPEED_MIN, BIRD_GROUP_SPEED_MAX);
            birdGroups[i].numBirdsInGroup = rand() % (MAX_BIRDS_PER_GROUP - 1) + 2;
        }
    }

    // Water stripe animation
    waterStripeAnimationX -= WATER_STRIPE_FLOW_SPEED;
    if (waterStripeAnimationX < -(WATER_STRIPE_MAX_LEN + WATER_STRIPE_MAX_GAP)) {
        waterStripeAnimationX += (WATER_STRIPE_MAX_LEN + WATER_STRIPE_MAX_GAP);
    }

    glutPostRedisplay();
    glutTimerFunc(30, timer, value + 1);
}

// Render the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Sky gradient
    glBegin(GL_QUADS);
        glColor3f(SUNSET_TOP_R, SUNSET_TOP_G, SUNSET_TOP_B);
        glVertex2f(0.0f, SKY_TOP_Y); glVertex2f((GLfloat)WINDOW_WIDTH, SKY_TOP_Y);
        glColor3f(SUNSET_HORIZON_R, SUNSET_HORIZON_G, SUNSET_HORIZON_B);
        glVertex2f((GLfloat)WINDOW_WIDTH, SKY_BOTTOM_Y); glVertex2f(0.0f, SKY_BOTTOM_Y);
    glEnd();

    drawSun(WINDOW_WIDTH * 0.8f, SKY_TOP_Y * 0.7f, SUN_RADIUS, 50); // Sun

    // Clouds
    for (int i = 0; i < MAX_CLOUD_PACKS; ++i) {
        for (int j = 0; j < cloudPacks[i].numCloudsInPack; ++j) {
            Cloud c = cloudPacks[i].individualClouds[j];
            drawCloud(cloudPacks[i].base_x + c.x_offset, cloudPacks[i].base_y + c.y_offset, c.size, c.r, c.g, c.b, c.a);
        }
    }

    // Birds
    for (int i = 0; i < MAX_BIRD_GROUPS; ++i) {
        for (int j = 0; j < birdGroups[i].numBirdsInGroup; ++j) {
            Bird b = birdGroups[i].individualBirds[j];
            drawBird(birdGroups[i].base_x + b.x_offset, birdGroups[i].base_y + b.y_offset, b.wing_angle, birdGroups[i].direction);
        }
    }

    // Water layer
    glColor4f(107.0f / 255.0f, 182.0f / 255.0f, 205.0f / 255.0f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, WATER_BOTTOM_Y); glVertex2f((GLfloat)WINDOW_WIDTH, WATER_BOTTOM_Y);
        glVertex2f((GLfloat)WINDOW_WIDTH, WATER_TOP_Y); glVertex2f(0.0f, WATER_TOP_Y);
    glEnd();

    // Bridge layer
    glColor4f(187.0f / 255.0f, 138.0f / 255.0f, 45.0f / 255.0f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, BRIDGE_BOTTOM_Y); glVertex2f((GLfloat)WINDOW_WIDTH, BRIDGE_BOTTOM_Y);
        glVertex2f((GLfloat)WINDOW_WIDTH, BRIDGE_TOP_Y); glVertex2f(0.0f, BRIDGE_TOP_Y);
    glEnd();

    // Static bridge stripes
    for (int i = 0; i < NUM_STATIC_BRIDGE_STRIPES; ++i) {
        BridgeStripe s = staticBridgeStripes[i];
        glColor4f(s.color_r, s.color_g, s.color_b, 1.0f);
        glLineWidth(s.width);
        glBegin(GL_LINES); glVertex2f(s.x, s.y); glVertex2f(s.x + s.length, s.y); glEnd();
    }

    // Road layer
    glColor4f(86.0f / 255.0f, 99.0f / 255.0f, 115.0f / 255.0f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, ROAD_BOTTOM_Y); glVertex2f((GLfloat)WINDOW_WIDTH, ROAD_BOTTOM_Y);
        glVertex2f((GLfloat)WINDOW_WIDTH, ROAD_TOP_Y); glVertex2f(0.0f, ROAD_TOP_Y);
    glEnd();

    // Road lines
    glColor4f(55.0f / 255.0f, 35.0f / 255.0f, 21.0f / 255.0f, 1.0f); glLineWidth(6.0f);
    glBegin(GL_LINES); glVertex2f(0.0f, ROAD_TOP_Y); glVertex2f((GLfloat)WINDOW_WIDTH, ROAD_TOP_Y); glEnd();
    glBegin(GL_LINES); glVertex2f(0.0f, ROAD_BOTTOM_Y); glVertex2f((GLfloat)WINDOW_WIDTH, ROAD_BOTTOM_Y); glEnd();

    // Striped road line (center)
    glColor4f(133.0f / 255.0f, 152.0f / 255.0f, 166.0f / 255.0f, 1.0f); glLineWidth(ROAD_STRIPE_WIDTH);
    GLfloat road_middle_y = (ROAD_BOTTOM_Y + ROAD_TOP_Y) / 2.0f;
    for (GLfloat cx = 0.0f; cx < WINDOW_WIDTH; cx += (ROAD_STRIPE_LEN + ROAD_STRIPE_GAP)) {
        glBegin(GL_LINES); glVertex2f(cx, road_middle_y); glVertex2f(cx + ROAD_STRIPE_LEN, road_middle_y); glEnd();
    }

    // Cars -
    for (int i = 0; i < MAX_CARS; ++i) {
        drawAnimatedCar(cars[i].x, cars[i].y, cars[i].wheelAngle, cars[i].carColorR, cars[i].carColorG, cars[i].carColorB, cars[i].direction);
    }

    // Bridge barrier
    glColor4f(BARRIER_COLOR_R, BARRIER_COLOR_G, BARRIER_COLOR_B, 1.0f);
    glLineWidth(BARRIER_LINE_THICKNESS);
    glBegin(GL_LINES); glVertex2f(0.0f, BRIDGE_TOP_Y + BARRIER_HEIGHT); glVertex2f((GLfloat)WINDOW_WIDTH, BRIDGE_TOP_Y + BARRIER_HEIGHT); glEnd();
    glBegin(GL_LINES); glVertex2f(0.0f, BRIDGE_TOP_Y); glVertex2f((GLfloat)WINDOW_WIDTH, BRIDGE_TOP_Y); glEnd();
    glLineWidth(BARRIER_VERTICAL_LINE_WIDTH);
    for (GLfloat x_barrier = BARRIER_VERTICAL_LINE_SPACING / 2.0f; x_barrier < WINDOW_WIDTH; x_barrier += BARRIER_VERTICAL_LINE_SPACING) {
        glBegin(GL_LINES); glVertex2f(x_barrier, BRIDGE_TOP_Y); glVertex2f(x_barrier, BRIDGE_TOP_Y + BARRIER_HEIGHT); glEnd();
    }

    // Cityscape buildings
    for (int i = 0; i < NUM_BUILDINGS; ++i) {
        Building bld = buildings[i]; // Use a temp variable
        drawBuilding(bld.x, bld.y, bld.width, bld.height, bld.colorR, bld.colorG, bld.colorB, bld.type);
    }

    // Bridge arches
    glColor4f(36.0f / 255.0f, 23.0f / 255.0f, 14.0f / 255.0f, 1.0f);
    for (int i = 0; i < NUM_ARCHES; ++i) {
        drawFilledArch(ARCH_START_OFFSET + (i * ARCH_DISTANCE), BRIDGE_BOTTOM_Y, ARCH_RADIUS, ARCH_SEGMENTS);
    }

    // Water stripes
    glColor4f(171.0f / 255.0f, 225.0f / 255.0f, 236.0f / 255.0f, 1.0f); glLineWidth(WATER_STRIPE_WIDTH);
    for (GLfloat y_stripe = WATER_BOTTOM_Y + WATER_STRIPE_VERT_SPACING / 2.0f; y_stripe < WATER_TOP_Y; y_stripe += (WATER_STRIPE_VERT_SPACING * WATER_STRIPE_DENSITY_FACTOR)) {
        GLfloat current_x_water_stripe = (fmod(y_stripe / (WATER_STRIPE_VERT_SPACING * WATER_STRIPE_DENSITY_FACTOR), 2.0f) < 1.0f) ? 0.0f : -(WATER_STRIPE_MAX_LEN / 2.0f + WATER_STRIPE_MAX_GAP / 2.0f);
        GLfloat randomized_y = y_stripe + randomFloat(-WATER_STRIPE_RANDOM_Y_MAX, WATER_STRIPE_RANDOM_Y_MAX);
        GLfloat loop_start_x = current_x_water_stripe + waterStripeAnimationX;
        while (loop_start_x < WINDOW_WIDTH) {
            GLfloat stripe_length = randomFloat(WATER_STRIPE_MIN_LEN, WATER_STRIPE_MAX_LEN);
            GLfloat stripe_gap = randomFloat(WATER_STRIPE_MIN_GAP, WATER_STRIPE_MAX_GAP);
            glBegin(GL_LINES);
                glVertex2f(loop_start_x, randomized_y); glVertex2f(loop_start_x + stripe_length, randomized_y);
            glEnd();
            loop_start_x += (stripe_length + stripe_gap);
        }
    }

    drawForegroundGround();

    // Trees and Bushes
    drawTree(TREE1_X, getGroundYAtX(TREE1_X) - GROUND_SINK_OFFSET, false);
    drawTree(TREE2_X, getGroundYAtX(TREE2_X) - GROUND_SINK_OFFSET, true);
    drawTree(TREE3_X, getGroundYAtX(TREE3_X) - GROUND_SINK_OFFSET, false);
    drawTree(TREE4_X, getGroundYAtX(TREE4_X) - GROUND_SINK_OFFSET, true);
    drawTree(TREE5_X, getGroundYAtX(TREE5_X) - GROUND_SINK_OFFSET, false);
    drawTree(TREE6_X, getGroundYAtX(TREE6_X) - GROUND_SINK_OFFSET, true);

    drawBush(BUSH1_X, getGroundYAtX(BUSH1_X) - GROUND_SINK_OFFSET, BUSH1_W, BUSH1_H);
    drawBush(BUSH2_X, getGroundYAtX(BUSH2_X) - GROUND_SINK_OFFSET, BUSH2_W, BUSH2_H);


    glFlush();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Cityscape");
    srand(static_cast<unsigned int>(time(NULL)));
    initGL();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    sndPlaySound("traffic_audio", SND_ASYNC);
    glutMainLoop();
    return 0;
}
