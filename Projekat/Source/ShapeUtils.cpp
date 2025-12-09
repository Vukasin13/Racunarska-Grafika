#include "../Header/Util.h"

void updateRectangleData(float* vertices, float x, float y, float w, float h, float uOffset, float vOffset, float uScale, float vScale) {
    vertices[0] = x + w / 2;  vertices[1] = y + h / 2;   vertices[2] = uOffset + uScale; vertices[3] = vOffset + vScale;
    vertices[4] = x + w / 2;  vertices[5] = y - h / 2;   vertices[6] = uOffset + uScale; vertices[7] = vOffset;
    vertices[8] = x - w / 2;  vertices[9] = y + h / 2;   vertices[10] = uOffset;    vertices[11] = vOffset + vScale;
    vertices[12] = x - w / 2; vertices[13] = y - h / 2;  vertices[14] = uOffset;    vertices[15] = vOffset;
}