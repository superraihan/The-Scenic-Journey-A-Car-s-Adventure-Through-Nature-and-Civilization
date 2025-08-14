#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include<mmsystem.h>
// Define PI for calculations
#define PI 3.14159265358979323846

// --- Global Variables ---
// Window dimensions
#define WIDTH 800 // Increased width for more landscape
#define HEIGHT 600 // Increased height

// Animation variables
float bladeAngle = 0.0f; // For turbine blades
float carPosition = -150.0f; // For animating the car

// Cloud positions and speeds
struct Cloud {
    float x, y, speed;
    float alpha; // For fading in/out
};
Cloud clouds[] = {
    {-50.0f, 450.0f, 0.3f, 0.8f},
    {150.0f, 500.0f, 0.2f, 0.9f},
    {350.0f, 480.0f, 0.4f, 0.75f},
    {550.0f, 520.0f, 0.35f, 0.85f},
    {700.0f, 460.0f, 0.25f, 0.95f}
};
const int numClouds = sizeof(clouds) / sizeof(clouds)[0];

// Smoke particles
struct SmokeParticle {
    float x, y;
    float size;
    float alpha;
    float speedY;
    float speedX;
    float initialX; // Added for waving effect
    float life; // Added for waving effect
};
// Changed to a C-style array as std::vector is removed
SmokeParticle smokeParticles[50]; // Assuming MAX_SMOKE_PARTICLES is 50
int numSmokeParticles = 0; // Current number of active smoke particles

const int MAX_SMOKE_PARTICLES = 50; // Limit number of particles

// Fog/Haze color (for atmospheric perspective)
// Subtle light blue-grey for distant objects
const float FOG_R = 0.8f;
const float FOG_G = 0.9f;
const float FOG_B = 1.0f;

// --- Initialization Function ---
void init() {
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f); // Light blue sky color
    glMatrixMode(GL_PROJECTION); // Set up projection matrix
    gluOrtho2D(0, WIDTH, 0, HEIGHT); // Define the 2D orthographic projection area
    glEnable(GL_BLEND); // Enable blending for transparency effects
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blending
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Enable anti-aliased lines
    glEnable(GL_LINE_SMOOTH); // Enable anti-aliased lines
    srand(time(NULL)); // Initialize random seed for varied elements like grass and smoke
}

// --- Helper Function: Draw Filled Ellipse (or Circle if rx=ry) ---
void drawFilledEllipse(float cx, float cy, float rx, float ry, float alpha, float r, float g, float b, int segments=36) {
    glColor4f(r, g, b, alpha); // Set color with alpha (transparency)
    glBegin(GL_TRIANGLE_FAN); // Draw using a triangle fan for solid fill
    glVertex2f(cx, cy); // Center of the shape
    for (int i = 0; i <= segments; i++) { // Iterate through 360 degrees
        float angle = i * 2 * PI / segments; // Convert degrees to radians
        glVertex2f(cx + rx * cos(angle), cy + ry * sin(angle)); // Calculate vertex on ellipse perimeter
    }
    glEnd();
}

// --- Helper Function: Draw Gradient Rectangle ---
void drawGradientRectangle(float x1, float y1, float x2, float y2,
                           float r1, float g1, float b1, // Top color
                           float r2, float g2, float b2) { // Bottom color
    glBegin(GL_POLYGON);
        glColor3f(r2, g2, b2); glVertex2f(x1, y1); // Bottom-left
        glColor3f(r2, g2, b2); glVertex2f(x2, y1); // Bottom-right
        glColor3f(r1, g1, b1); glVertex2f(x2, y2); // Top-right
        glColor3f(r1, g1, b1); glVertex2f(x1, y2); // Top-left
    glEnd();
}

// --- Draw Wind Turbine Function (FIXED CONNECTION) ---
void drawWindTurbine() {
    // Tower Base (darker grey-brown for concrete base)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_POLYGON);
        glVertex2f(WIDTH * 0.5f - 25, HEIGHT * 0.2f - 5);
        glVertex2f(WIDTH * 0.5f + 25, HEIGHT * 0.2f - 5);
        glVertex2f(WIDTH * 0.5f + 22, HEIGHT * 0.2f);
        glVertex2f(WIDTH * 0.5f - 22, HEIGHT * 0.2f);
    glEnd();

    // Tower Main Body (tapered with subtle shading)
    float towerBaseW = 20.0f;
    float towerTopW = 12.0f; // Slightly wider at the top for better connection
    float towerHeight = HEIGHT * 0.6f;
    float towerCenterX = WIDTH * 0.5f;
    float towerBaseY = HEIGHT * 0.2f;

    // Left side (shadowed)
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_POLYGON);
        glVertex2f(towerCenterX - towerBaseW, towerBaseY);
        glVertex2f(towerCenterX - towerTopW, towerBaseY + towerHeight);
        glVertex2f(towerCenterX - towerTopW + 2, towerBaseY + towerHeight); // Slight inner edge
        glVertex2f(towerCenterX - towerBaseW + 2, towerBaseY); // Slight inner edge
    glEnd();

    // Front/Lit side (lighter ash)
    glColor3f(0.85f, 0.85f, 0.85f);
    glBegin(GL_POLYGON);
        glVertex2f(towerCenterX - towerBaseW + 2, towerBaseY);
        glVertex2f(towerCenterX + towerBaseW - 2, towerBaseY);
        glVertex2f(towerCenterX + towerTopW - 2, towerBaseY + towerHeight);
        glVertex2f(towerCenterX - towerTopW + 2, towerBaseY + towerHeight);
    glEnd();

    // Right side (more shadowed)
    glColor3f(0.55f, 0.55f, 0.55f);
    glBegin(GL_POLYGON);
        glVertex2f(towerCenterX + towerBaseW - 2, towerBaseY);
        glVertex2f(towerCenterX + towerBaseW, towerBaseY);
        glVertex2f(towerCenterX + towerTopW, towerBaseY + towerHeight);
        glVertex2f(towerCenterX + towerTopW - 2, towerBaseY + towerHeight);
    glEnd();

    // Nacelle (The box at the top)
    float nacelleWidth = 35;
    float nacelleHeight = 20;
    float nacelleX = towerCenterX - (nacelleWidth / 2); // Centered on tower's top X
    float nacelleY = towerBaseY + towerHeight - 2; // Adjusted to sit directly on tower

    // Main body (darker side)
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_POLYGON);
        glVertex2f(nacelleX, nacelleY);
        glVertex2f(nacelleX + nacelleWidth, nacelleY);
        glVertex2f(nacelleX + nacelleWidth, nacelleY + nacelleHeight);
        glVertex2f(nacelleX, nacelleY + nacelleHeight);
    glEnd();

    // Front face (lighter grey for sunlit effect)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_POLYGON);
        glVertex2f(nacelleX, nacelleY + nacelleHeight);
        glVertex2f(nacelleX + nacelleWidth, nacelleY + nacelleHeight);
        glVertex2f(nacelleX + nacelleWidth - 5, nacelleY + nacelleHeight + 5); // Top-right offset
        glVertex2f(nacelleX + 5, nacelleY + nacelleHeight + 5); // Top-left offset
    glEnd();

    // Rotor Hub with Nose Cone
    float hubX = towerCenterX; // Directly centered on tower's top X
    float hubY = nacelleY + nacelleHeight / 2 + 3; // Centered within nacelle height

    // Hub (darker grey, inner circle lighter)
    drawFilledEllipse(hubX, hubY, 12, 12, 1.0f, 0.35f, 0.35f, 0.35f);
    drawFilledEllipse(hubX, hubY, 10, 10, 1.0f, 0.45f, 0.45f, 0.45f);

    // Nose Cone (elongated, with subtle shading)
    glColor3f(0.5f, 0.5f, 0.5f); // Lighter side
    glBegin(GL_POLYGON);
        glVertex2f(hubX, hubY - 5);
        glVertex2f(hubX + 25, hubY); // Tip
        glVertex2f(hubX, hubY + 5);
        glVertex2f(hubX - 10, hubY); // Back of cone
    glEnd();
    glColor3f(0.4f, 0.4f, 0.4f); // Darker side
    glBegin(GL_TRIANGLES);
        glVertex2f(hubX, hubY - 5);
        glVertex2f(hubX, hubY + 5);
        glVertex2f(hubX + 25, hubY);
    glEnd();


    // --- Blades ---
    glPushMatrix(); // Save current transformation matrix
    glTranslatef(hubX, hubY, 0); // Translate to hub center for rotation
    glRotatef(bladeAngle, 0, 0, 1); // Rotate around Z-axis

    for (int i = 0; i < 3; ++i) { // Draw 3 blades
        float angle_offset = i * 120.0f; // Angle for each blade (120 degrees apart)
        float blade_length = 150.0f;

        glBegin(GL_POLYGON);
            // Blade shading (simulate curve)
            // Darker edge
            glColor3f(0.45f, 0.45f, 0.45f);
            // Start from hub edge, not center
            glVertex2f(cos( (angle_offset - 2) * PI / 180.0f) * 10, sin( (angle_offset - 2) * PI / 180.0f) * 10);
            glVertex2f(cos( (angle_offset - 2) * PI / 180.0f) * blade_length, sin( (angle_offset - 2) * PI / 180.0f) * blade_length);
            glVertex2f(cos( (angle_offset + 10) * PI / 180.0f) * blade_length, sin( (angle_offset + 10) * PI / 180.0f) * blade_length);
            glVertex2f(cos( (angle_offset + 10) * PI / 180.0f) * 10, sin( (angle_offset + 10) * PI / 180.0f) * 10);

            // Lighter main surface
            glColor3f(0.75f, 0.75f, 0.75f);
            // Start from hub edge, not center
            glVertex2f(cos( (angle_offset + 5) * PI / 180.0f) * 10, sin( (angle_offset + 5) * PI / 180.0f) * 10);
            glVertex2f(cos( (angle_offset + 2) * PI / 180.0f) * blade_length, sin( (angle_offset + 2) * PI / 180.0f) * blade_length);
            glVertex2f(cos( (angle_offset - 2) * PI / 180.0f) * blade_length, sin( (angle_offset - 2) * PI / 180.0f) * blade_length);
            glVertex2f(cos( (angle_offset - 5) * PI / 180.0f) * 10, sin( (angle_offset - 5) * PI / 180.0f) * 10);
        glEnd();

        // Simulate dark leading edge (subtle line)
        glColor3f(0.3f, 0.3f, 0.3f);
        glLineWidth(1.5);
        glBegin(GL_LINE_STRIP);
            glVertex2f(cos( (angle_offset + 5) * PI / 180.0f) * 10, sin( (angle_offset + 5) * PI / 180.0f) * 10);
            glVertex2f(cos( (angle_offset + 2) * PI / 180.0f) * blade_length, sin( (angle_offset + 2) * PI / 180.0f) * blade_length);
        glEnd();
        glLineWidth(1.0);
    }
    glPopMatrix(); // Restore previous transformation matrix
}

// --- Draw Sun Function (Softer, more atmospheric) ---
void drawSun(float x, float y, float baseRadius) {
    // Inner core (brightest, least transparent, pure yellow)
    drawFilledEllipse(x, y, baseRadius, baseRadius, 1.0f, 1.0f, 1.0f, 0.0f, 60);

    // Outer glow (more transparent, slightly orange/yellow)
    drawFilledEllipse(x, y, baseRadius * 1.3, baseRadius * 1.3, 0.3f, 1.0f, 0.9f, 0.0f, 60);
    drawFilledEllipse(x, y, baseRadius * 1.6, baseRadius * 1.6, 0.15f, 1.0f, 0.85f, 0.0f, 60);

    // Subtle sun rays (using very thin, transparent quads/lines)
    glColor4f(1.0f, 1.0f, 0.5f, 0.1f); // Faint yellow, highly transparent
    for (int i = 0; i < 12; ++i) {
        float angle = i * (360.0f / 12) * PI / 180.0f;
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

// --- Draw Cloud Function (More irregular, softer edges) ---
void drawCloud(float x, float y, float alpha) {
    // Base cloud shapes with varying transparency and shades for depth
    // The key to realistic clouds is many overlapping, semi-transparent blobs.
    // Use slightly greyish-whites.

    // Main body
    drawFilledEllipse(x + 40, y + 12, 60, 25, alpha * 0.8f, 0.9f, 0.92f, 0.95f, 30);
    drawFilledEllipse(x + 15, y + 16, 45, 20, alpha * 0.85f, 0.92f, 0.94f, 0.97f, 30);
    drawFilledEllipse(x + 75, y + 16, 55, 22, alpha * 0.8f, 0.9f, 0.92f, 0.95f, 30);

    // Overlapping shapes for softer edges and internal variation
    drawFilledEllipse(x + 30, y + 22, 40, 20, alpha * 0.9f, 0.95f, 0.97f, 1.0f, 30);
    drawFilledEllipse(x + 65, y + 25, 45, 22, alpha * 0.92f, 0.96f, 0.98f, 1.0f, 30);
    drawFilledEllipse(x + 50, y + 30, 35, 18, alpha * 0.95f, 0.98f, 0.99f, 1.0f, 30);

    // Slightly darker underside for shadow
    drawFilledEllipse(x + 40, y + 2, 60, 20, alpha * 0.6f, 0.85f, 0.87f, 0.9f, 30);
    drawFilledEllipse(x + 25, y + 6, 30, 15, alpha * 0.55f, 0.83f, 0.85f, 0.88f, 30);

    // Very subtle light highlights
    drawFilledEllipse(x + 55, y + 33, 20, 12, alpha * 0.98f, 1.0f, 1.0f, 1.0f, 30);
    drawFilledEllipse(x + 20, y + 27, 18, 10, alpha * 0.95f, 1.0f, 1.0f, 1.0f, 30);
}

// --- Draw Dashed Road Line Function ---
void drawDashedRoadLine() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the dashed line
    glLineWidth(3.0); // Thicker line

    // Calculate the vertical center of the road
    float roadCenterY = HEIGHT * 0.06f; // (0 + HEIGHT * 0.12) / 2

    float lineBottomY = roadCenterY - 2; // 2 units below center
    float lineTopY = roadCenterY + 2;    // 2 units above center

    for (int i = 0; i < WIDTH; i += 60) { // Draw segments every 60 units
        glBegin(GL_POLYGON);
            glVertex2f(i, lineBottomY); // Bottom-left
            glVertex2f(i + 30, lineBottomY); // Bottom-right (segment length 30)
            glVertex2f(i + 30, lineTopY); // Top-right (thickness 4)
            glVertex2f(i, lineTopY); // Top-left
        glEnd();
    }
    glLineWidth(1.0); // Reset line width
}

// --- Original Realistic Car Function (No longer called directly) ---
void drawRealisticCar(float x, float y) {
    // Base car body (darker red with gradients for shape)
    // Darker bottom/shadowed side
    glColor3f(0.6f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 130, y);
        glVertex2f(x, y);
        glVertex2f(x, y + 15);
        glVertex2f(x + 130, y + 15);
    glEnd();

    // Main body (red)
    glColor3f(0.8f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 130, y + 15);
        glVertex2f(x, y + 15);
        glVertex2f(x + 10, y + 35); // Front-bottom corner
        glVertex2f(x + 120, y + 35); // Rear-bottom corner
    glEnd();

    // Top cabin (slightly lighter red)
    glColor3f(0.9f, 0.1f, 0.1f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 15, y + 35);
        glVertex2f(x + 115, y + 35);
        glVertex2f(x + 100, y + 50); // Top-right
        glVertex2f(x + 30, y + 50); // Top-left
    glEnd();

    // Windows (subtle grey-blue, slightly transparent)
    glColor4f(0.6f, 0.7f, 0.8f, 0.8f);
    glBegin(GL_POLYGON); // Front window
        glVertex2f(x + 32, y + 50);
        glVertex2f(x + 45, y + 36);
        glVertex2f(x + 75, y + 36);
        glVertex2f(x + 68, y + 50);
    glEnd();
    glBegin(GL_POLYGON); // Rear window
        glVertex2f(x + 78, y + 36);
        glVertex2f(x + 100, y + 36);
        glVertex2f(x + 90, y + 50);
        glVertex2f(x + 70, y + 50);
    glEnd();

    // Wheels (black with subtle grey highlights)
    for (int i = 0; i < 2; i++) {
        float wheel_cx = x + 30 + i * 70;
        float wheel_cy = y + 5;
        float radiusOuter = 15;
        float radiusInner = 8;

        // Tire (dark grey-black)
        drawFilledEllipse(wheel_cx, wheel_cy, radiusOuter, radiusOuter, 1.0f, 0.15f, 0.15f, 0.15f, 20);

        // Rim (darker grey)
        drawFilledEllipse(wheel_cx, wheel_cy, radiusInner, radiusInner, 1.0f, 0.3f, 0.3f, 0.3f, 20);

        // Spokes (simple lines or small quads)
        glColor3f(0.5f, 0.5f, 0.5f);
        glLineWidth(1.5);
        glBegin(GL_LINES);
            for (int j = 0; j < 4; ++j) {
                float angle = j * 90 * PI / 180.0f;
                glVertex2f(wheel_cx + cos(angle) * radiusInner, wheel_cy + sin(angle) * radiusInner);
                glVertex2f(wheel_cx + cos(angle) * (radiusInner - 2), wheel_cy + sin(angle) * (radiusInner - 2));
            }
        glEnd();
        glLineWidth(1.0);
    }

    // Headlights (yellow)
    glColor3f(1.0f, 1.0f, 0.0f);
    drawFilledEllipse(x + 125, y + 20, 5, 3, 1.0f, 1.0f, 1.0f, 0.0f);
    // Taillights (red)
    glColor3f(0.8f, 0.0f, 0.0f);
    drawFilledEllipse(x + 5, y + 20, 5, 3, 1.0f, 0.8f, 0.0f, 0.0f);

    // Grill (black lines)
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_LINES);
        glVertex2f(x + 120, y + 10); glVertex2f(x + 128, y + 10);
        glVertex2f(x + 120, y + 13); glVertex2f(x + 128, y + 13);
    glEnd();

    // Shadow under car (simple ellipse)
    drawFilledEllipse(x + 65, y - 5, 60, 8, 0.3f, 0.0f, 0.0f, 0.0f, 20);
}

// --- NEW: Draw Red Cartoon Car Function ---
void drawRedCartoonCar(float x, float y) {
    glColor3f(1.0f, 0.0f, 0.0f); // Red body
    glBegin(GL_POLYGON);
        glVertex2f(x + 120, y);
        glVertex2f(x, y);
        glVertex2f(x, y + 20);
        glVertex2f(x + 20, y + 40);
        glVertex2f(x + 80, y + 40);
        glVertex2f(x + 100, y + 20);
        glVertex2f(x + 120, y + 20);
    glEnd();

    glColor3f(0.6f, 0.8f, 1.0f); // Blue windows
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

        glColor3f(0.0f, 0.0f, 0.0f); // Black tires
        // Using drawFilledEllipse for consistency and better anti-aliasing
        drawFilledEllipse(cx, cy, radiusOuter, radiusOuter, 1.0f, 0.0f, 0.0f, 0.0f, 100);

        glColor3f(1.0f, 1.0f, 1.0f); // White rims
        drawFilledEllipse(cx, cy, radiusInner, radiusInner, 1.0f, 1.0f, 1.0f, 1.0f, 100);
    }
}


// --- Draw Realistic Farmhouse Function ---
void drawFarmHouse() {
    float fx = 200, fy = 120; // Base position

    // --- Foundation ---
    glColor3f(0.4f, 0.35f, 0.3f); // Stone-like dark grey-brown
    glBegin(GL_POLYGON);
        glVertex2f(fx - 5, fy - 5);
        glVertex2f(fx + 105, fy - 5);
        glVertex2f(fx + 100, fy);
        glVertex2f(fx, fy);
    glEnd();

    // --- Main House Body ---
    // Front Wall (subtle gradient, warmer tan)
    glBegin(GL_POLYGON);
        glColor3f(0.9f, 0.8f, 0.65f); glVertex2f(fx, fy + 100); // Top-left
        glColor3f(0.9f, 0.8f, 0.65f); glVertex2f(fx + 100, fy + 100); // Top-right
        glColor3f(0.8f, 0.7f, 0.55f); glVertex2f(fx + 100, fy); // Bottom-right
        glColor3f(0.8f, 0.7f, 0.55f); glVertex2f(fx, fy); // Bottom-left
    glEnd();

    // Side Wall (left) - darker for depth
    glBegin(GL_POLYGON);
        glColor3f(0.7f, 0.6f, 0.45f); glVertex2f(fx, fy); // Front-bottom-left
        glColor3f(0.6f, 0.5f, 0.35f); glVertex2f(fx - 30, fy + 10); // Back-bottom-left
        glColor3f(0.6f, 0.5f, 0.35f); glVertex2f(fx - 30, fy + 110); // Back-top-left
        glColor3f(0.7f, 0.6f, 0.45f); glVertex2f(fx, fy + 100); // Front-top-left
    glEnd();

    // --- Roof ---
    // Front Roof (triangle) - Dark grey-brown with subtle red/brown tint
    glBegin(GL_TRIANGLES);
        glColor3f(0.4f, 0.35f, 0.35f); glVertex2f(fx, fy + 100); // left front
        glColor3f(0.4f, 0.35f, 0.35f); glVertex2f(fx + 100, fy + 100); // right front
        glColor3f(0.3f, 0.25f, 0.25f); glVertex2f(fx + 50, fy + 150); // roof peak
    glEnd();

    // Side Roof (left side) - Even darker shade for shadow
    glBegin(GL_POLYGON);
        glColor3f(0.3f, 0.25f, 0.25f); glVertex2f(fx, fy + 100); // front-left roof edge
        glColor3f(0.2f, 0.15f, 0.15f); glVertex2f(fx - 30, fy + 110); // back-left roof edge
        glColor3f(0.2f, 0.15f, 0.15f); glVertex2f(fx + 20, fy + 160); // peak-back-left
        glColor3f(0.3f, 0.25f, 0.25f); glVertex2f(fx + 50, fy + 150); // peak-front
    glEnd();

    // --- Chimney ---
    glColor3f(0.6f, 0.5f, 0.4f); // Brick-like color for chimney
    glBegin(GL_POLYGON);
        glVertex2f(fx + 10, fy + 150); // Base on roof
        glVertex2f(fx + 25, fy + 150);
        glVertex2f(fx + 25, fy + 170);
        glVertex2f(fx + 10, fy + 170);
    glEnd();
    // Chimney top (darker)
    glColor3f(0.3f, 0.25f, 0.2f);
    glBegin(GL_POLYGON);
        glVertex2f(fx + 8, fy + 170);
        glVertex2f(fx + 27, fy + 170);
        glVertex2f(fx + 27, fy + 172);
        glVertex2f(fx + 8, fy + 172);
    glEnd();

    // Smoke emission logic
    // Only emit new particles occasionally to avoid too many
    if (rand() % 5 < 3 && numSmokeParticles < MAX_SMOKE_PARTICLES) { // 60% chance per frame to emit
        SmokeParticle newSmoke;
        newSmoke.initialX = fx + 17.5f + (rand() % 5 - 2); // Center of chimney top +/- 2
        newSmoke.x = newSmoke.initialX;
        newSmoke.y = fy + 175; // Slightly above chimney
        newSmoke.size = 5.0f + (rand() % 5); // Random size
        newSmoke.alpha = 0.6f; // Start mostly opaque
        newSmoke.speedY = 0.5f + (rand() % 50) / 100.0f; // Varying upward speed
        newSmoke.speedX = (rand() % 50 - 25) / 100.0f; // Slight horizontal drift
        newSmoke.life = 0.0f; // Initialize life
        smokeParticles[numSmokeParticles++] = newSmoke; // Add to array and increment count
    }

    // --- Door ---
    glColor3f(0.55f, 0.27f, 0.07f); // Wood brown
    glBegin(GL_POLYGON);
        glVertex2f(fx + 40, fy);
        glVertex2f(fx + 60, fy);
        glVertex2f(fx + 60, fy + 70);
        glVertex2f(fx + 40, fy + 70);
    glEnd();
    // Door knob
    glColor3f(0.8f, 0.8f, 0.0f); // Gold/brass
    drawFilledEllipse(fx + 55, fy + 45, 2, 2, 1.0f, 0.8f, 0.8f, 0.0f);
    // Door frame (darker brown)
    glColor3f(0.3f, 0.15f, 0.05f);
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(fx + 40, fy);
        glVertex2f(fx + 60, fy);
        glVertex2f(fx + 60, fy + 70);
        glVertex2f(fx + 40, fy + 70);
    glEnd();
    glLineWidth(1.0);

    // --- Windows ---
    // Window glass (subtle reflection)
    glColor4f(0.6f, 0.7f, 0.8f, 0.7f); // Light blue-grey for glass with transparency
    glBegin(GL_POLYGON); // Left front window
        glVertex2f(fx + 10, fy + 55);
        glVertex2f(fx + 30, fy + 55);
        glVertex2f(fx + 30, fy + 85);
        glVertex2f(fx + 10, fy + 85);
    glEnd();
    glBegin(GL_POLYGON); // Right front window
        glVertex2f(fx + 70, fy + 55);
        glVertex2f(fx + 90, fy + 55);
        glVertex2f(fx + 90, fy + 85);
        glVertex2f(fx + 70, fy + 85);
    glEnd();
    glBegin(GL_POLYGON); // Side window (left)
        glVertex2f(fx - 20, fy + 63);
        glVertex2f(fx - 10, fy + 60);
        glVertex2f(fx - 10, fy + 90);
        glVertex2f(fx - 20, fy + 93);
    glEnd();

    // Window frames/panes (darker for contrast)
    glColor3f(0.3f, 0.15f, 0.05f); // Dark brown for frames
    glLineWidth(1.5);
    glBegin(GL_LINE_LOOP); // Left front frame
        glVertex2f(fx + 10, fy + 55); glVertex2f(fx + 30, fy + 55);
        glVertex2f(fx + 30, fy + 85); glVertex2f(fx + 10, fy + 85);
    glEnd();
    glBegin(GL_LINES); // Left front panes
        glVertex2f(fx + 20, fy + 55); glVertex2f(fx + 20, fy + 85); // Vertical
        glVertex2f(fx + 10, fy + 70); glVertex2f(fx + 30, fy + 70); // Horizontal
    glEnd();

    glBegin(GL_LINE_LOOP); // Right front frame
        glVertex2f(fx + 70, fy + 55); glVertex2f(fx + 90, fy + 55);
        glVertex2f(fx + 90, fy + 85); glVertex2f(fx + 70, fy + 85);
    glEnd();
    glBegin(GL_LINES); // Right front panes
        glVertex2f(fx + 80, fy + 55); glVertex2f(fx + 80, fy + 85); // Vertical
        glVertex2f(fx + 70, fy + 70); glVertex2f(fx + 90, fy + 70); // Horizontal
    glEnd();

    glBegin(GL_LINE_LOOP); // Side window frame
        glVertex2f(fx - 20, fy + 63); glVertex2f(fx - 10, fy + 60);
        glVertex2f(fx - 10, fy + 90); glVertex2f(fx - 20, fy + 93);
    glEnd();
    glBegin(GL_LINES); // Side window panes
        glVertex2f(fx - 15, fy + 61.5f); glVertex2f(fx - 15, fy + 91.5f); // Vertical (approximate)
        glVertex2f(fx - 20, fy + 78); glVertex2f(fx - 10, fy + 75); // Horizontal (approximate)
    glEnd();
    glLineWidth(1.0);
}

// --- Draw Realistic Barn Function (No Windows) ---
void drawBarn() {
    // Adjusted position for gap
    float bx = 50, by = 120; // Base position, moved right by 50 units

    // --- Foundation ---
    glColor3f(0.4f, 0.35f, 0.3f); // Stone-like dark grey-brown
    glBegin(GL_POLYGON);
        glVertex2f(bx - 5, by - 5);
        glVertex2f(bx + 105, by - 5);
        glVertex2f(bx + 100, by);
        glVertex2f(bx, by);
    glEnd();

    // --- Main Barn Body ---
    // Front Wall (Classic Red Barn with subtle top-to-bottom gradient)
    glBegin(GL_POLYGON);
        glColor3f(0.8f, 0.1f, 0.1f); glVertex2f(bx, by + 90); // Top-left
        glColor3f(0.8f, 0.1f, 0.1f); glVertex2f(bx + 100, by + 90); // Top-right
        glColor3f(0.7f, 0.08f, 0.08f); glVertex2f(bx + 100, by); // Bottom-right
        glColor3f(0.7f, 0.08f, 0.08f); glVertex2f(bx, by); // Bottom-left
    glEnd();

    // Side Wall (left) - darker red for depth
    glBegin(GL_POLYGON);
        glColor3f(0.6f, 0.08f, 0.08f); glVertex2f(bx, by);
        glColor3f(0.5f, 0.06f, 0.06f); glVertex2f(bx - 30, by + 10);
        glColor3f(0.5f, 0.06f, 0.06f); glVertex2f(bx - 30, by + 100);
        glColor3f(0.6f, 0.08f, 0.08f); glVertex2f(bx, by + 90);
    glEnd();

    // Simulate vertical planks on walls
    glColor3f(0.6f, 0.05f, 0.05f); // Darker red for plank lines
    glLineWidth(1.0);
    glBegin(GL_LINES);
        for(int i = 0; i <= 10; ++i) {
            glVertex2f(bx + i * 10, by); glVertex2f(bx + i * 10, by + 90);
            glVertex2f(bx - 30 + i * 3, by + 10); glVertex2f(bx - 30 + i * 3, by + 100); // Side wall planks (approx)
        }
    glEnd();
    glLineWidth(1.0);

    // --- Roof ---
    // Front Roof (gable) - Dark grey/brown with a hint of red
    glBegin(GL_POLYGON);
        glColor3f(0.35f, 0.3f, 0.3f); glVertex2f(bx, by + 90);
        glColor3f(0.35f, 0.3f, 0.3f); glVertex2f(bx + 100, by + 90);
        glColor3f(0.25f, 0.2f, 0.2f); glVertex2f(bx + 50, by + 140); // Higher peak
    glEnd();

    // Side Roof (left) - Even darker shade for shadow
    glBegin(GL_POLYGON);
        glColor3f(0.25f, 0.2f, 0.2f); glVertex2f(bx, by + 90);
        glColor3f(0.15f, 0.1f, 0.1f); glVertex2f(bx - 30, by + 100);
        glColor3f(0.15f, 0.1f, 0.1f); glVertex2f(bx + 20, by + 150); // Matches front peak height change
        glColor3f(0.25f, 0.2f, 0.2f); glVertex2f(bx + 50, by + 140);
    glEnd();

    // --- Barn Doors (two large sliding doors) ---
    // Left Door (wood brown, with subtle gradient)
    glBegin(GL_POLYGON);
        glColor3f(0.55f, 0.27f, 0.07f); glVertex2f(bx + 5, by + 85);
        glColor3f(0.55f, 0.27f, 0.07f); glVertex2f(bx + 50, by + 85);
        glColor3f(0.45f, 0.2f, 0.05f); glVertex2f(bx + 50, by);
        glColor3f(0.45f, 0.2f, 0.05f); glVertex2f(bx + 5, by);
    glEnd();
    // Right Door
    glBegin(GL_POLYGON);
        glColor3f(0.55f, 0.27f, 0.07f); glVertex2f(bx + 50, by + 85);
        glColor3f(0.55f, 0.27f, 0.07f); glVertex2f(bx + 95, by + 85);
        glColor3f(0.45f, 0.2f, 0.05f); glVertex2f(bx + 95, by);
        glColor3f(0.45f, 0.2f, 0.05f); glVertex2f(bx + 50, by);
    glEnd();

    // Door details (darker "X" bracing, thicker lines)
    glColor3f(0.3f, 0.15f, 0.05f); // Dark brown for bracing
    glLineWidth(3.0);
    glBegin(GL_LINES);
        // Left door X
        glVertex2f(bx + 5, by); glVertex2f(bx + 50, by + 85);
        glVertex2f(bx + 50, by); glVertex2f(bx + 5, by + 85);
        // Right door X
        glVertex2f(bx + 50, by); glVertex2f(bx + 95, by + 85);
        glVertex2f(bx + 95, by); glVertex2f(bx + 50, by + 85);
    glEnd();
    glLineWidth(1.0);
}

// --- Draw Tree Function (More detailed and varied) ---
void drawTree(float x, float y, float scale) {
    float trunk_base_w = 15 * scale;
    float trunk_top_w = 8 * scale;
    float trunk_h = 80 * scale;
    float canopy_r_max = 60 * scale;

    // --- Trunk ---
    // Base (wider, darker, more irregular)
    glColor3f(0.45f, 0.22f, 0.05f); // Darker brown
    glBegin(GL_POLYGON);
        glVertex2f(x - trunk_base_w / 2, y);
        glVertex2f(x + trunk_base_w / 2, y);
        glVertex2f(x + trunk_top_w / 2 + 2 * scale, y + trunk_h * 0.4f); // Slight curve
        glVertex2f(x - trunk_top_w / 2 - 2 * scale, y + trunk_h * 0.4f);
    glEnd();

    // Upper trunk (tapering, lighter, with branches)
    glColor3f(0.55f, 0.27f, 0.07f); // Medium brown
    glBegin(GL_POLYGON);
        glVertex2f(x - trunk_top_w / 2 * 0.8f, y + trunk_h * 0.4f);
        glVertex2f(x + trunk_top_w / 2 * 0.8f, y + trunk_h * 0.4f);
        glVertex2f(x + trunk_top_w / 2 * 0.8f, y + trunk_h);
        glVertex2f(x - trunk_top_w / 2 * 0.8f, y + trunk_h);
    glEnd();

    // --- Branches (more irregular, darker) ---
    glColor3f(0.4f, 0.2f, 0.05f);
    glLineWidth(2.0); // Thicker for branches
    // Left main branch
    glBegin(GL_POLYGON);
        glVertex2f(x - trunk_top_w / 2 * 0.8f, y + trunk_h * 0.7f);
        glVertex2f(x - trunk_top_w / 1.5f, y + trunk_h * 0.8f);
        glVertex2f(x - trunk_top_w * 2.5f, y + trunk_h * 1.1f);
        glVertex2f(x - trunk_top_w * 2, y + trunk_h * 1.0f);
    glEnd();
    // Right main branch
    glBegin(GL_POLYGON);
        glVertex2f(x + trunk_top_w / 2 * 0.8f, y + trunk_h * 0.6f);
        glVertex2f(x + trunk_top_w * 1.2f, y + trunk_h * 0.7f);
        glVertex2f(x + trunk_top_w * 2.0f, y + trunk_h * 0.9f);
        glVertex2f(x + trunk_top_w * 1.5f, y + trunk_h * 0.8f);
    glEnd();
    glLineWidth(1.0);

    // --- Leaf Canopy (multiple overlapping semi-transparent irregular shapes) ---
    // Darkest base layer (shadow)
    drawFilledEllipse(x + 5 * scale, y + trunk_h + 30 * scale, canopy_r_max * 0.8f, canopy_r_max * 0.7f, 0.8f, 0.05f, 0.4f, 0.05f, 24);

    // Main volume (medium green, varied shapes)
    drawFilledEllipse(x + 10 * scale, y + trunk_h + 45 * scale, canopy_r_max, canopy_r_max * 0.9f, 0.85f, 0.1f, 0.6f, 0.1f, 28);
    drawFilledEllipse(x - 15 * scale, y + trunk_h + 35 * scale, canopy_r_max * 0.7f, canopy_r_max * 0.8f, 0.88f, 0.15f, 0.65f, 0.15f, 26);
    drawFilledEllipse(x + 25 * scale, y + trunk_h + 30 * scale, canopy_r_max * 0.6f, canopy_r_max * 0.65f, 0.87f, 0.12f, 0.62f, 0.12f, 25);

    // Lighter areas (front-facing)
    drawFilledEllipse(x + 20 * scale, y + trunk_h + 55 * scale, canopy_r_max * 0.7f, canopy_r_max * 0.65f, 0.9f, 0.2f, 0.7f, 0.2f, 24);
    drawFilledEllipse(x - 5 * scale, y + trunk_h + 60 * scale, canopy_r_max * 0.5f, canopy_r_max * 0.55f, 0.9f, 0.25f, 0.75f, 0.25f, 22);

    // Brightest highlights (small, irregular clumps)
    drawFilledEllipse(x + 30 * scale, y + trunk_h + 70 * scale, canopy_r_max * 0.3f, canopy_r_max * 0.28f, 0.95f, 0.4f, 0.85f, 0.4f, 18);
    drawFilledEllipse(x + 5 * scale, y + trunk_h + 75 * scale, canopy_r_max * 0.35f, canopy_r_max * 0.32f, 0.95f, 0.45f, 0.9f, 0.45f, 19);
    drawFilledEllipse(x - 20 * scale, y + trunk_h + 65 * scale, canopy_r_max * 0.25f, canopy_r_max * 0.22f, 0.95f, 0.4f, 0.88f, 0.4f, 17);
}

// --- Draw Distant Hills (Layered for atmospheric perspective) ---
void drawHills() {
    // Farthest hills (most desaturated, highest)
    glBegin(GL_POLYGON);
        glColor3f(FOG_R * 0.7f, FOG_G * 0.7f, FOG_B * 0.7f); // Very desaturated blue-grey
        glVertex2f(0, HEIGHT * 0.4f);
        glVertex2f(WIDTH * 0.2f, HEIGHT * 0.5f);
        glVertex2f(WIDTH * 0.4f, HEIGHT * 0.45f);
        glVertex2f(WIDTH * 0.6f, HEIGHT * 0.52f);
        glVertex2f(WIDTH * 0.8f, HEIGHT * 0.47f);
        glVertex2f(WIDTH, HEIGHT * 0.5f);
        glVertex2f(WIDTH, HEIGHT * 0.2f);
        glVertex2f(0, HEIGHT * 0.2f);
    glEnd();

    // Mid-ground hills (less desaturated, slightly lower)
    glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.6f, 0.5f); // Muted green-grey
        glVertex2f(0, HEIGHT * 0.3f);
        glVertex2f(WIDTH * 0.1f, HEIGHT * 0.35f);
        glVertex2f(WIDTH * 0.3f, HEIGHT * 0.32f);
        glVertex2f(WIDTH * 0.5f, HEIGHT * 0.4f);
        glVertex2f(WIDTH * 0.7f, HEIGHT * 0.38f);
        glVertex2f(WIDTH * 0.9f, HEIGHT * 0.32f);
        glVertex2f(WIDTH, HEIGHT * 0.3f);
        glVertex2f(WIDTH, HEIGHT * 0.2f);
        glVertex2f(0, HEIGHT * 0.2f);
    glEnd();

    // Foreground hills/fields (more vibrant, closer to view)
    glBegin(GL_POLYGON);
        glColor3f(0.3f, 0.5f, 0.2f); // Darker green
        glVertex2f(0, HEIGHT * 0.25f);
        glVertex2f(WIDTH * 0.15f, HEIGHT * 0.28f);
        glVertex2f(WIDTH * 0.35f, HEIGHT * 0.26f);
        glVertex2f(WIDTH * 0.55f, HEIGHT * 0.32f);
        glVertex2f(WIDTH * 0.75f, HEIGHT * 0.3f);
        glVertex2f(WIDTH, HEIGHT * 0.28f);
        glVertex2f(WIDTH, HEIGHT * 0.2f);
        glVertex2f(0, HEIGHT * 0.2f);
    glEnd();
}

// --- Draw Fence Function ---
void drawFence() {
    float fenceBaseY = HEIGHT * 0.12f; // Top edge of the main ground area
    float postHeight = 30.0f;
    float postWidth = 5.0f;
    float postSpacing = 50.0f; // Distance between posts

    // Fence post color (weathered wood)
    glColor3f(0.6f, 0.4f, 0.2f); // Medium brown

    // Draw vertical posts
    for (float x = 0; x < WIDTH; x += postSpacing) {
        // Draw post with slight tapering or shading
        glBegin(GL_POLYGON);
            glVertex2f(x, fenceBaseY);
            glVertex2f(x + postWidth, fenceBaseY);
            glVertex2f(x + postWidth * 0.8f, fenceBaseY + postHeight); // Top slightly narrower
            glVertex2f(x + postWidth * 0.2f, fenceBaseY + postHeight);
        glEnd();
    }

    // Fence rail color (slightly darker wood)
    glColor3f(0.5f, 0.3f, 0.15f); // Darker brown
    glLineWidth(2.0); // Make rails a bit thicker for visibility

    // Draw horizontal rails
    float railThickness = 5.0f;

    // Top rail
    glBegin(GL_POLYGON);
        glVertex2f(0, fenceBaseY + postHeight * 0.7f - railThickness / 2);
        glVertex2f(WIDTH, fenceBaseY + postHeight * 0.7f - railThickness / 2);
        glVertex2f(WIDTH, fenceBaseY + postHeight * 0.7f + railThickness / 2);
        glVertex2f(0, fenceBaseY + postHeight * 0.7f + railThickness / 2);
    glEnd();

    // Bottom rail
    glBegin(GL_POLYGON);
        glVertex2f(0, fenceBaseY + postHeight * 0.3f - railThickness / 2);
        glVertex2f(WIDTH, fenceBaseY + postHeight * 0.3f - railThickness / 2);
        glVertex2f(WIDTH, fenceBaseY + postHeight * 0.3f + railThickness / 2);
        glVertex2f(0, fenceBaseY + postHeight * 0.3f + railThickness / 2);
    glEnd();

    glLineWidth(1.0); // Reset line width
}

// --- Draw Smoke Particle Function ---
void drawSmokeParticle(float x, float y, float size, float alpha) {
    drawFilledEllipse(x, y, size, size * 0.7f, alpha, 0.7f, 0.7f, 0.7f, 10); // Greyish smoke
}


// --- Display Callback Function ---
void displayt() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // Sky Gradient (Dawn/Dusk feel or bright day)
    drawGradientRectangle(0, HEIGHT * 0.5f, WIDTH, HEIGHT,
                          0.6f, 0.8f, 1.0f, // Top: Lighter blue
                          0.8f, 0.9f, 1.0f); // Bottom: Slightly warmer/whiter blue

    // Horizon line / atmospheric haze
    glColor4f(FOG_R, FOG_G, FOG_B, 0.5f); // Subtle fog color
    glBegin(GL_POLYGON);
        glVertex2f(0, HEIGHT * 0.2f);
        glVertex2f(WIDTH, HEIGHT * 0.2f);
        glVertex2f(WIDTH, HEIGHT * 0.25f); // Slight height for haze band
        glVertex2f(0, HEIGHT * 0.25f);
    glEnd();


    drawHills(); // Distant layered hills

    // Ground (more complex, with variations)
    glBegin(GL_POLYGON);
        glColor3f(0.4f, 0.6f, 0.2f); // Base dark green
        glVertex2f(0, 0);
        glVertex2f(WIDTH, 0);
        glColor3f(0.45f, 0.65f, 0.25f); // Lighter green towards top
        glVertex2f(WIDTH, HEIGHT * 0.2f);
        glVertex2f(0, HEIGHT * 0.2f);
    glEnd();

    // Grass clumps (small, varied green ellipses)
    srand(1); // Keep random sequence consistent for drawing
    for(int i = 0; i < 50; ++i) {
        float grass_x = (float)(rand() % WIDTH);
        float grass_y = (float)(rand() % (int)(HEIGHT * 0.18)) + HEIGHT * 0.12f;
        float grass_r = 3 + (float)(rand() % 5);
        float grass_g = 0.35f + (float)(rand() % 20) / 100.0f;
        float grass_b = 0.55f + (float)(rand() % 15) / 100.0f;
        float grass_a = 0.9f + (float)(rand() % 10) / 100.0f;
        drawFilledEllipse(grass_x, grass_y, grass_r, grass_r * 0.8f, grass_a, grass_g, grass_b, 0.2f, 12);
    }
    srand(time(NULL)); // Reset seed for animation elements like smoke

    drawSun(WIDTH - 100, HEIGHT - 100, 50); // Draws the sun FIRST

    // Then draw clouds OVER the sun
    for (int i = 0; i < numClouds; ++i) {
        drawCloud(clouds[i].x, clouds[i].y, clouds[i].alpha);
    }

    drawBarn(); // Draws the realistic barn
    drawFarmHouse(); // Draws the realistic farmhouse

    // Draw smoke particles
    for (int i = 0; i < numSmokeParticles; ++i) {
        drawSmokeParticle(smokeParticles[i].x, smokeParticles[i].y, smokeParticles[i].size, smokeParticles[i].alpha);
    }

    // Draw the wind turbine *after* clouds, but before trees
    drawWindTurbine();

    // Trees (varied sizes and positions for natural look)
    drawTree(WIDTH * 0.6f, HEIGHT * 0.15f, 1.2f);
    drawTree(WIDTH * 0.7f, HEIGHT * 0.12f, 1.0f);
    drawTree(WIDTH * 0.85f, HEIGHT * 0.1f, 0.9f);
    drawTree(WIDTH * 0.45f, HEIGHT * 0.18f, 1.1f);

    // Add the fence here
    drawFence(); // Draw the fence on top of the ground, before the road

    // Road (dark grey, with subtle shading for perspective)
    glBegin(GL_POLYGON);
        glColor3f(0.2f, 0.2f, 0.2f); glVertex2f(0, 0); // Bottom-left
        glColor3f(0.2f, 0.2f, 0.2f); glVertex2f(WIDTH, 0); // Bottom-right
        glColor3f(0.3f, 0.3f, 0.3f); glVertex2f(WIDTH, HEIGHT * 0.12f); // Top-right (slightly lighter)
        glColor3f(0.3f, 0.3f, 0.3f); glVertex2f(0, HEIGHT * 0.12f); // Top-left
    glEnd();

    drawDashedRoadLine();
    // CALL YOUR CARTOON CAR HERE!
    drawRedCartoonCar(carPosition, HEIGHT * 0.08f); // Draws the animated cartoon car

    glutSwapBuffers(); // Swap the front and back buffers to display the rendered scene

}

// --- Animation Update Function ---
void update(int value) {
    // Rotate turbine blades
    bladeAngle += 2.0f; // Slightly slower rotation
    if (bladeAngle > 360) bladeAngle -= 360;

    // Animate clouds moving left
    for (int i = 0; i < numClouds; ++i) {
        clouds[i].x -= clouds[i].speed;
        if (clouds[i].x < -150) { // Reset cloud position when it goes off screen
            clouds[i].x = WIDTH + (rand() % 100); // Random starting X off-screen
            clouds[i].y = (float)(rand() % (HEIGHT / 4)) + (HEIGHT * 0.7f); // Random Y in top quarter
            clouds[i].speed = 0.2f + (float)(rand() % 50) / 100.0f; // Varying speeds
            clouds[i].alpha = 0.8f + (float)(rand() % 20) / 100.0f; // Varying transparency
        }
    }

    // Animate car moving right - MODIFIED FOR SINGLE PASS
    if (carPosition < WIDTH + 50) { // Continue moving only if not far off-screen
        carPosition += 3.0f; // Faster car
    }
    // No else condition, so car stops animating once it's off-screen


    // Animate smoke particles for billowy effect
    // Using a simple array and manual removal to avoid std::vector
    int i = 0;
    while (i < numSmokeParticles) {
        smokeParticles[i].y += smokeParticles[i].speedY;
        smokeParticles[i].x += (rand() % 3 - 1.0f) * 0.5f; // Slight horizontal drift for billowiness

        smokeParticles[i].alpha -= 0.01f; // Fade out a bit faster
        smokeParticles[i].size += 0.3f;   // Expand more noticeably

        // Decrease vertical speed over time to simulate rising and slowing
        smokeParticles[i].speedY *= 0.98f;
        if (smokeParticles[i].speedY < 0.1f) smokeParticles[i].speedY = 0.1f; // Ensure it still moves up a bit

        smokeParticles[i].life += 0.015f;

        if (smokeParticles[i].alpha <= 0.0f || smokeParticles[i].y > HEIGHT + 100 || smokeParticles[i].life >= 1.0f) { // Remove if faded or off-screen or fully lived
            // If particle needs to be removed, swap it with the last particle and decrement count
            smokeParticles[i] = smokeParticles[numSmokeParticles - 1];
            numSmokeParticles--;
        } else {
            ++i; // Only increment if the particle was not removed
        }

    }


    glutPostRedisplay(); // Request a redraw of the scene
    glutTimerFunc(16, update, 0); // Call update again after 16 milliseconds (approx 60 FPS)
}

// --- Main Function ---
int main(int argc, char** argv) {
    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Set display mode: double buffer, RGB color
    glutInitWindowSize(WIDTH, HEIGHT); // Set window size
    glutInitWindowPosition(100, 100); // Set window position on screen
    glutCreateWindow("Wind Turbine Farm Scene - Enhanced Realism"); // Create the window with a title

    init(); // Call our custom initialization function

    glutDisplayFunc(displayt); // Register the display function
    glutTimerFunc(0, update, 0); // Start the animation timer
    sndPlaySound("car_horn.wav", SND_ASYNC);
    glutMainLoop(); // Enter the GLUT event processing loop

    return 0;
}
