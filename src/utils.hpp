#ifndef UTILS_H
#define UTILS_H
#include <raylib-cpp.hpp>

void DrawTextCentered(const char *text, int posX, int posY, int fontSize, Color color) {
    int width = MeasureText(text, fontSize);
    DrawText(text, posX - width / 2, posY - fontSize / 2, fontSize, color);
}

#endif