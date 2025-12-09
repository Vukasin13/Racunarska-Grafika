#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

int endProgram(std::string message);
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned loadImageToTexture(const char* filePath);
GLFWcursor* loadImageToCursor(const char* filePath);
void updateRectangleData(float* vertices, float x, float y, float w, float h, float uOffset, float vOffset, float uScale, float vScale);