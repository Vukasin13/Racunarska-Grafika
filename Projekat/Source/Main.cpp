#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

#include "../Header/Util.h"
#include "../Header/MathUtils.h"
#include "../Header/TextRenderer.h"

int windowWidth = 1200;
int windowHeight = 800;
int windowPosX = 100;
int windowPosY = 100;
bool isFullScreen = true;

const char* WINDOW_TITLE = "Mapa Projekat - Resursi u folderu";

double lastTime = 0.0;
const double TARGET_FPS = 75.0;

float mapX = 0.5f;
float mapY = 0.5f;
float mapSpeed = 0.3f;
float zoomLevel = 0.15f;
float walkerPosX = 0.5f;
float walkerPosY = 0.5f;

bool isMeasuringMode = false;
float walkingDistance = 0.0f;
float measuringDistance = 0.0f;

std::vector<Point> measurePoints;

unsigned int VAO, VBO;
unsigned int lineVAO, lineVBO;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    if (!isFullScreen) {
        windowWidth = width;
        windowHeight = height;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int w, h;
        glfwGetWindowSize(window, &w, &h);

        float ndcX = (xpos / w) * 2.0f - 1.0f;
        float ndcY = -((ypos / h) * 2.0f - 1.0f);

        // IKONICA ZA PROMENU MODA
        float iconX = -0.9f;
        float iconY = -0.85f;
        float iconSize = 0.15f;
        if (ndcX >= iconX - iconSize / 2 && ndcX <= iconX + iconSize / 2 &&
            ndcY >= iconY - iconSize / 2 && ndcY <= iconY + iconSize / 2) {
            isMeasuringMode = !isMeasuringMode;
            std::cout << "Rezim: " << (isMeasuringMode ? "MERENJE" : "HODANJE") << std::endl;
            return;
        }

        // RESET DUGME
        float resetX = -0.38f;
        float resetY = -0.85f;
        float resetSize = 0.1f;
        if (ndcX >= resetX - resetSize / 2 && ndcX <= resetX + resetSize / 2 &&
            ndcY >= resetY - resetSize / 2 && ndcY <= resetY + resetSize / 2) {

            if (isMeasuringMode) {
                // merenje
                measurePoints.clear();
                measuringDistance = 0.0f;
                std::cout << "MERENJE RESETOVANO." << std::endl;
            }
            else {
                // hodanje
                walkingDistance = 0.0f;
                walkerPosX = 0.5f;
                walkerPosY = 0.5f;
                std::cout << "HODANJE RESETOVANO." << std::endl;
            }
            return;
        }

        // MERENJE RAZDALJINE
        if (isMeasuringMode) {
            float screenU = (ndcX + 1.0f) / 2.0f;
            float screenV = (ndcY + 1.0f) / 2.0f;

            float mapU = (mapX - zoomLevel / 2.0f) + screenU * zoomLevel;
            float mapV = (mapY - zoomLevel / 2.0f) + screenV * zoomLevel;

            int indexZaBrisanje = -1;
            float prag = 0.02f;

            for (int i = 0; i < measurePoints.size(); ++i) {
                float dx = measurePoints[i].x - mapU;
                float dy = measurePoints[i].y - mapV;
                if (std::sqrt(dx * dx + dy * dy) < prag) {
                    indexZaBrisanje = i;
                    break;
                }
            }

            if (indexZaBrisanje != -1) {
                measurePoints.erase(measurePoints.begin() + indexZaBrisanje);
            }
            else {
                Point novaTacka = { mapU, mapV };
                measurePoints.push_back(novaTacka);
            }

            measuringDistance = calculateTotalDistance(measurePoints);
        }
    }
}

int main()
{
    if (!glfwInit()) return endProgram("GLFW greska");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    windowWidth = mode->width;
    windowHeight = mode->height;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, WINDOW_TITLE, primaryMonitor, NULL);

    if (window == NULL) return endProgram("Prozor greska");
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) return endProgram("GLEW greska");

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    unsigned int shaderProgram = createShader("basic.vert", "basic.frag");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, 1000 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    unsigned int mapTexture = loadImageToTexture("../Resources/GTA-put.png");
    unsigned int pinTexture = loadImageToTexture("../Resources/Pin1.png");
    unsigned int signatureTexture = loadImageToTexture("../Resources/potpis2.png");
    unsigned int walkIcon = loadImageToTexture("../Resources/walk.png");
    unsigned int rulerIcon = loadImageToTexture("../Resources/ruler.png");
    unsigned int numbersTexture = loadImageToTexture("../Resources/numbers.png");
    unsigned int plateTexture = loadImageToTexture("../Resources/plate2.png");
    unsigned int resetTexture = loadImageToTexture("../Resources/reset1.png");
    unsigned int gtaHudTexture = loadImageToTexture("../Resources/money.png");

    unsigned int redTexture;
    glGenTextures(1, &redTexture);
    glBindTexture(GL_TEXTURE_2D, redTexture);
    unsigned char redPixel[] = { 255, 0, 0, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, redPixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unsigned int whiteTexture;
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    unsigned char whitePixel[] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLFWcursor* cursor = loadImageToCursor("../Resources/compass.png");
    if (cursor) glfwSetCursor(window, cursor);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);

    lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        if (deltaTime < 1.0 / TARGET_FPS) continue;
        lastTime = currentTime;

        // F11 Toggle
        static bool f11Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
            if (!f11Pressed) {
                if (isFullScreen) {
                    glfwSetWindowMonitor(window, NULL, windowPosX, windowPosY, 1200, 800, 0);
                    isFullScreen = false;
                }
                else {
                    glfwGetWindowPos(window, &windowPosX, &windowPosY);
                    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                    isFullScreen = true;
                }
                f11Pressed = true;
            }
        }
        else {
            f11Pressed = false;
        }

        // R Toggle
        static bool rPressed = false;
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            if (!rPressed) {
                isMeasuringMode = !isMeasuringMode;
                rPressed = true;
            }
        }
        else {
            rPressed = false;
        }

        // GLAVNA LOGIKA KRETANJA
        if (!isMeasuringMode) {
            // HODANJE
            zoomLevel = 0.15f;
            float moveAmount = mapSpeed * deltaTime * zoomLevel;
            bool moved = false;

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { walkerPosX -= moveAmount; moved = true; }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { walkerPosX += moveAmount; moved = true; }
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { walkerPosY += moveAmount; moved = true; }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { walkerPosY -= moveAmount; moved = true; }

            float half = zoomLevel / 2.0f;
            if (walkerPosX < half) walkerPosX = half; if (walkerPosX > 1 - half) walkerPosX = 1 - half;
            if (walkerPosY < half) walkerPosY = half; if (walkerPosY > 1 - half) walkerPosY = 1 - half;

            if (moved) walkingDistance += moveAmount * 5000.0f;

            mapX = walkerPosX;
            mapY = walkerPosY;
        }
        else {
            // MERENJE
            mapX = 0.5f;
            mapY = 0.5f;
            zoomLevel = 1.0f;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // MAPA
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        float uStart = mapX - (zoomLevel / 2.0f);
        float vStart = mapY - (zoomLevel / 2.0f);
        float mapVertices[16];
        updateRectangleData(mapVertices, 0.0f, 0.0f, 2.0f, 2.0f, uStart, vStart, zoomLevel, zoomLevel);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mapVertices), mapVertices);
        glBindTexture(GL_TEXTURE_2D, mapTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        float uiY = -0.85f;
        float iconX = -0.9f;
        float plateX = -0.78f;
        float textX = -0.76f;
        float resetX = -0.38f;

        float resetVertices[16];
        updateRectangleData(resetVertices, resetX, uiY, 0.08f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(resetVertices), resetVertices);
        glBindTexture(GL_TEXTURE_2D, resetTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // MOD
        if (!isMeasuringMode) {
            // HODANJE: Pin
            float pinVertices[16];
            updateRectangleData(pinVertices, 0.0f, 0.0f, 0.1f, 0.15f, 0.0f, 0.0f, 1.0f, 1.0f);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pinVertices), pinVertices);
            glBindTexture(GL_TEXTURE_2D, pinTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // HODANJE: Ikonica
            float iconVertices[16];
            updateRectangleData(iconVertices, iconX, uiY, 0.15f, 0.15f, 0.0f, 0.0f, 1.0f, 1.0f);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(iconVertices), iconVertices);
            glBindTexture(GL_TEXTURE_2D, walkIcon);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // HODANJE: hud
            float hudVertices[16];
            updateRectangleData(hudVertices, 0.7f, 0.8f, 0.5f, 0.4f, 0.0f, 0.0f, 1.0f, 1.0f);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hudVertices), hudVertices);
            glBindTexture(GL_TEXTURE_2D, gtaHudTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // HODANJE: Plocica i Brojevi
            float plateVertices[16];
            updateRectangleData(plateVertices, plateX + 0.15f, uiY, 0.4f, 0.12f, 0.0f, 0.0f, 1.0f, 1.0f);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(plateVertices), plateVertices);
            glBindTexture(GL_TEXTURE_2D, plateTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            drawNumber(walkingDistance, textX, uiY, 0.05f, numbersTexture);

        }
        else {
            // MERENJE: Ikonica
            float iconVertices[16];
            updateRectangleData(iconVertices, iconX, uiY, 0.15f, 0.15f, 0.0f, 0.0f, 1.0f, 1.0f);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(iconVertices), iconVertices);
            glBindTexture(GL_TEXTURE_2D, rulerIcon);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // MERENJE: Plocica i Brojevi
            float plateVertices[16];
            updateRectangleData(plateVertices, plateX + 0.15f, uiY, 0.4f, 0.12f, 0.0f, 0.0f, 1.0f, 1.0f);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(plateVertices), plateVertices);
            glBindTexture(GL_TEXTURE_2D, plateTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            drawNumber(measuringDistance, textX, uiY, 0.05f, numbersTexture);

            // MERENJE: hud
            float hudVertices[16];
            updateRectangleData(hudVertices, 0.7f, 0.8f, 0.5f, 0.4f, 0.0f, 0.0f, 1.0f, 1.0f);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hudVertices), hudVertices);
            glBindTexture(GL_TEXTURE_2D, gtaHudTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // MERENJE: Linija
            if (measurePoints.size() > 1) {
                std::vector<float> lineVerts;
                for (const auto& p : measurePoints) {
                    lineVerts.push_back(p.x * 2.0f - 1.0f);
                    lineVerts.push_back(p.y * 2.0f - 1.0f);
                }
                glBindVertexArray(lineVAO);
                glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, lineVerts.size() * sizeof(float), lineVerts.data());
                glBindTexture(GL_TEXTURE_2D, redTexture);
                glLineWidth(4.0f);
                glDrawArrays(GL_LINE_STRIP, 0, measurePoints.size());
            }

            // MERENJE: Bele Tacke
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glEnableVertexAttribArray(1);
            glBindTexture(GL_TEXTURE_2D, whiteTexture);
            for (const auto& p : measurePoints) {
                float sx = p.x * 2.0f - 1.0f;
                float sy = p.y * 2.0f - 1.0f;
                float ptVertices[16];
                updateRectangleData(ptVertices, sx, sy, 0.015f, 0.02f, 0.0f, 0.0f, 1.0f, 1.0f);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ptVertices), ptVertices);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }

        // 3. POTPIS
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glEnableVertexAttribArray(1);
        float sigVertices[16];
        updateRectangleData(sigVertices, 0.7f, -0.8f, 0.5f, 0.2f, 0.0f, 0.0f, 1.0f, 1.0f);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sigVertices), sigVertices);
        glBindTexture(GL_TEXTURE_2D, signatureTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
    glfwTerminate();
    return 0;
}