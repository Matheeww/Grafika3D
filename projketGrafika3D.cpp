#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#ifdef _WIN32
#include <windows.h>
#include <GL/glu.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#else
#include <GL/glu.h>
#endif

#include <vector>
#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>

const float CUBE_SIZE = 0.04f;

struct CubeBit {
    int val;
    sf::Vector3f pos;
    sf::Vector3f vel;
};

std::vector<CubeBit> keyBits;
bool broken = false;
float keyRotationX = 0.0f;
bool autoRotate = false;
bool breakingStarted = false;
float breakTimer = 0.0f;
const float CRACK_TIME = 1.5f;
const float EXPLODE_TIME = 3.0f;
bool cracked = false;
GLuint textureID = 0;

bool loadTexture(const char* filename) {
    sf::Image image;

    if (!image.loadFromFile(filename)) {
        std::cerr << "OSTRZEŻENIE: Nie można załadować tekstury: " << filename << "\n";
        return false;
    }

    image.flipVertically();

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        image.getSize().x, image.getSize().y,
        0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    std::cout << "✓ Tekstura załadowana: " << filename << "\n";
    return true;
}

void makeCryptoKey() {
    keyBits.clear();

    int headRadius = 30;
    int holeRadius = 22;
    int headThickness = 8;

    for (int y = -headThickness / 2; y <= headThickness / 2; ++y) {
        for (int z = -headRadius; z <= headRadius; ++z) {
            for (int x = -headRadius; x <= headRadius; ++x) {
                float dist = sqrt(float(x * x + z * z));
                if (dist <= headRadius && dist >= holeRadius) {
                    CubeBit cb;
                    cb.val = rand() % 2;
                    cb.pos = sf::Vector3f(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);
                    cb.vel = sf::Vector3f(0, 0, 0);
                    keyBits.push_back(cb);
                }
            }
        }
    }

    int shaftLength = 120;
    int shaftWidth = 10;
    int shaftHeight = 8;
    int shaftStartX = headRadius;

    for (int x = shaftStartX; x < shaftStartX + shaftLength; ++x) {
        for (int y = -shaftHeight / 2; y <= shaftHeight / 2; ++y) {
            for (int z = -shaftWidth / 2; z <= shaftWidth / 2; ++z) {
                CubeBit cb;
                cb.val = rand() % 2;
                cb.pos = sf::Vector3f(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);
                cb.vel = sf::Vector3f(0, 0, 0);
                keyBits.push_back(cb);
            }
        }
    }

    int teethStartX = shaftStartX + shaftLength - 25;
    int tooth1Depth = 9;
    int tooth1Length = 12;
    int tooth1Height = 6;

    for (int x = teethStartX; x < teethStartX + tooth1Length; ++x) {
        for (int y = -tooth1Height / 2; y <= tooth1Height / 2; ++y) {
            for (int z = shaftWidth / 2; z < shaftWidth / 2 + tooth1Depth; ++z) {
                CubeBit cb;
                cb.val = rand() % 2;
                cb.pos = sf::Vector3f(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);
                cb.vel = sf::Vector3f(0, 0, 0);
                keyBits.push_back(cb);
            }
        }
    }

    int tooth2StartX = teethStartX + 15;
    int tooth2Depth = 12;
    int tooth2Length = 18;
    int tooth2Height = 6;

    for (int x = tooth2StartX; x < tooth2StartX + tooth2Length; ++x) {
        for (int y = -tooth2Height / 2; y <= tooth2Height / 2; ++y) {
            for (int z = shaftWidth / 2; z < shaftWidth / 2 + tooth2Depth; ++z) {
                CubeBit cb;
                cb.val = rand() % 2;
                cb.pos = sf::Vector3f(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);
                cb.vel = sf::Vector3f(0, 0, 0);
                keyBits.push_back(cb);
            }
        }
    }

    int gapStartX = teethStartX + tooth1Length;
    for (int x = gapStartX; x < tooth2StartX; ++x) {
        for (int y = -tooth2Height / 2; y <= tooth2Height / 2; ++y) {
            for (int z = shaftWidth / 2; z < shaftWidth / 2 + 4; ++z) {
                CubeBit cb;
                cb.val = rand() % 2;
                cb.pos = sf::Vector3f(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);
                cb.vel = sf::Vector3f(0, 0, 0);
                keyBits.push_back(cb);
            }
        }
    }

    std::cout << "Wygenerowano klucz: " << keyBits.size() << " szescianow\n";
}

void drawCube(const sf::Vector3f& pos, float size, int val) {
    float h = size / 2.0f;
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    if (val == 1)
        glColor3f(0.9f, 0.9f, 0.9f);
    else
        glColor3f(0.7f, 0.7f, 0.7f);

    if (textureID > 0) {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    glBegin(GL_QUADS);

    // Front
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-h, -h, h);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(h, -h, h);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(h, h, h);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-h, h, h);

    // Back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-h, -h, -h);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-h, h, -h);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(h, h, -h);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(h, -h, -h);

    // Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-h, -h, -h);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-h, -h, h);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-h, h, h);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-h, h, -h);

    // Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(h, -h, -h);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(h, h, -h);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(h, h, h);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(h, -h, h);

    // Bottom
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-h, -h, -h);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(h, -h, -h);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(h, -h, h);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-h, -h, h);

    // Top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-h, h, -h);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-h, h, h);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(h, h, h);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(h, h, -h);

    glEnd();

    // Krawędzie
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glLineWidth(3.0f);
    glColor3f(0.0f, 1.0f, 0.0f);

    glBegin(GL_LINES);
    glVertex3f(-h, -h, -h); glVertex3f(h, -h, -h);
    glVertex3f(h, -h, -h); glVertex3f(h, -h, h);
    glVertex3f(h, -h, h); glVertex3f(-h, -h, h);
    glVertex3f(-h, -h, h); glVertex3f(-h, -h, -h);
    glVertex3f(-h, h, -h); glVertex3f(h, h, -h);
    glVertex3f(h, h, -h); glVertex3f(h, h, h);
    glVertex3f(h, h, h); glVertex3f(-h, h, h);
    glVertex3f(-h, h, h); glVertex3f(-h, h, -h);
    glVertex3f(-h, -h, -h); glVertex3f(-h, h, -h);
    glVertex3f(h, -h, -h); glVertex3f(h, h, -h);
    glVertex3f(h, -h, h); glVertex3f(h, h, h);
    glVertex3f(-h, -h, h); glVertex3f(-h, h, h);
    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPosition[] = { 5.0f, 10.0f, 5.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    GLfloat matAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat matDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat matShininess = 64.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, matShininess);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);
}

void reshapeScreen(sf::Vector2u size) {
    if (size.y == 0) size.y = 1;
    glViewport(0, 0, size.x, size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, double(size.x) / double(size.y), 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

sf::Vector3f camPos(0.0f, 0.5f, 4.0f);
sf::Vector3f camTarget(0.0f, 0.0f, 0.0f);
float cam_yaw = 0.0f, cam_pitch = 0.0f;
sf::Vector2i lastMousePos;
bool mouseDragging = false;

void handleCamera(const sf::RenderWindow& window, float dt) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (!mouseDragging) {
            lastMousePos = sf::Mouse::getPosition(window);
            mouseDragging = true;
        }
        sf::Vector2i nowPos = sf::Mouse::getPosition(window);
        cam_yaw += 0.003f * (nowPos.x - lastMousePos.x);
        cam_pitch -= 0.003f * (nowPos.y - lastMousePos.y);
        if (cam_pitch > 1.5f) cam_pitch = 1.5f;
        if (cam_pitch < -1.5f) cam_pitch = -1.5f;
        lastMousePos = nowPos;
    }
    else mouseDragging = false;

    float speed = 2.0f * dt;
    sf::Vector3f forward(sin(cam_yaw), 0, cos(cam_yaw));
    sf::Vector3f right(cos(cam_yaw), 0, -sin(cam_yaw));

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) camPos += forward * speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) camPos -= forward * speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) camPos -= right * speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) camPos += right * speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) camPos.y += speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) camPos.y -= speed;

    camTarget = camPos + sf::Vector3f(
        sin(cam_yaw) * cos(cam_pitch),
        sin(cam_pitch),
        cos(cam_yaw) * cos(cam_pitch)
    );
}

void updateRotation(float dt) {
    if (autoRotate && !broken) {
        keyRotationX += 1.0f * dt;
    }
}

void breakKey() {
    if (broken || breakingStarted) return;
    breakingStarted = true;
    breakTimer = 0.0f;
    std::cout << "Odliczanie do pęknięcia..." << std::endl;
}

void crackKey() {
    if (cracked) return;
    cracked = true;

    float minX = 1000, maxX = -1000;
    for (const auto& cb : keyBits) {
        if (cb.pos.x < minX) minX = cb.pos.x;
        if (cb.pos.x > maxX) maxX = cb.pos.x;
    }
    float breakPoint = (minX + maxX) / 2.0f;

    float angleLeft = ((rand() % 80) - 40) / 180.0f * 3.14159f;
    float angleRight = ((rand() % 80) - 40) / 180.0f * 3.14159f;
    float tiltLeftY = ((rand() % 80) - 40) / 100.0f;
    float tiltRightY = ((rand() % 80) - 40) / 100.0f;
    float tiltLeftZ = ((rand() % 80) - 40) / 100.0f;
    float tiltRightZ = ((rand() % 80) - 40) / 100.0f;

    for (auto& cb : keyBits) {
        float direction = (cb.pos.x > breakPoint) ? 1.0f : -1.0f;
        if (direction < 0) {
            float vx = -0.5f + ((rand() % 20) - 10) / 50.0f;
            float vy = sin(angleLeft) + tiltLeftY;
            float vz = cos(angleLeft) + tiltLeftZ;
            cb.vel = sf::Vector3f(vx, vy * 0.5f, vz * 0.5f);
        }
        else {
            float vx = 0.5f + ((rand() % 20) - 10) / 50.0f;
            float vy = sin(angleRight) + tiltRightY;
            float vz = cos(angleRight) + tiltRightZ;
            cb.vel = sf::Vector3f(vx, vy * 0.5f, vz * 0.5f);
        }
    }
    std::cout << "PEK! Klucz się rozdziela nierówno." << std::endl;
}

void explodeKey() {
    if (broken) return;
    broken = true;

    float minX = 1000, maxX = -1000;
    for (const auto& cb : keyBits) {
        if (cb.pos.x < minX) minX = cb.pos.x;
        if (cb.pos.x > maxX) maxX = cb.pos.x;
    }
    float breakPoint = (minX + maxX) / 2.0f;
    for (auto& cb : keyBits) {
        float direction = (cb.pos.x > breakPoint) ? 1.0f : -1.0f;
        float rx = (rand() % 400 - 200) / 20.0f + direction * 5.0f;
        float ry = (rand() % 400 - 200) / 20.0f;
        float rz = (rand() % 400 - 200) / 20.0f;
        cb.vel = sf::Vector3f(rx, ry, rz);
    }
    std::cout << "WYBUCH!" << std::endl;
}

void updateKey(float dt) {
    if (breakingStarted && !cracked) {
        breakTimer += dt;
        if (breakTimer >= CRACK_TIME) {
            crackKey();
        }
    }
    if (cracked && !broken) {
        breakTimer += dt;
        if (breakTimer >= EXPLODE_TIME) {
            explodeKey();
        }
    }
    if (cracked) {
        for (auto& cb : keyBits) {
            cb.pos += cb.vel * dt;
            cb.vel *= 0.98f;
        }
    }
}

int main() {
    srand(unsigned(time(0)));
    sf::RenderWindow window(
        sf::VideoMode(1280, 900),
        "Wizualizacja 3D - Klucz kryptograficzny",
        sf::Style::Default,
        sf::ContextSettings(24)
    );
    window.setVerticalSyncEnabled(true);

    reshapeScreen(window.getSize());
    initOpenGL();
    makeCryptoKey();

    loadTexture("C:/Users/pc/Pictures/Screenshots/matrix.png");


    std::cout << "Program uruchomiony:\n";
    std::cout << "WASD - ruch, Mysz LPM - obrót kamery\n";
    std::cout << "O - obrót klucza automatycznie, R - pęknięcie + wybuch\n";
    std::cout << "N - reset sceny, ESC - wyjście\n";

    sf::Clock clk;
    while (window.isOpen()) {
        float dt = clk.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) window.close();
                if (event.key.code == sf::Keyboard::R) breakKey();
                if (event.key.code == sf::Keyboard::O) autoRotate = !autoRotate;
                if (event.key.code == sf::Keyboard::N) {
                    cracked = false;
                    broken = false;
                    breakingStarted = false;
                    breakTimer = 0.0f;
                    keyRotationX = 0.0f;
                    makeCryptoKey();
                    std::cout << "Wyczyszczono scenę i zbudowano nowy klucz!\n";
                }
            }
            if (event.type == sf::Event::Resized) reshapeScreen(window.getSize());
        }

        handleCamera(window, dt);
        updateKey(dt);
        updateRotation(dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(camPos.x, camPos.y, camPos.z, camTarget.x, camTarget.y, camTarget.z, 0, 1, 0);

        glPushMatrix();
        glRotatef(keyRotationX * 57.2958f, 1.0f, 0.0f, 0.0f);

        for (const auto& cb : keyBits) {
            drawCube(cb.pos, CUBE_SIZE, cb.val);
        }
        glPopMatrix();

        window.display();
    }
    return 0;
}