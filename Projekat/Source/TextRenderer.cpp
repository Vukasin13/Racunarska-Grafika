#include "../Header/TextRenderer.h"

void drawNumber(float number, float x, float y, float size, unsigned int textureID) {
    std::string s = std::to_string((int)number);
    s += " m";

    glBindTexture(GL_TEXTURE_2D, textureID);

    float width = size;
    float height = size * 1.42f;

    for (int i = 0; i < s.length(); ++i) {
        char c = s[i];
        float uOffset = 0.0f;
        float uWidth = 1.0f / 10.0f;

        if (c >= '0' && c <= '9') {
            int digit = c - '0';
            uOffset = digit * uWidth;

            float charVertices[16];
            updateRectangleData(charVertices, x, y, width, height, uOffset, 0.0f, uWidth, 1.0f);

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(charVertices), charVertices);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        if (c == ' ' || c == 'm') {
            x += width * 0.6f;
        }
        else {
            x += width * 1.0f;
        }
    }
}