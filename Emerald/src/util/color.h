#pragma once

class Color {
public:
    float R;
    float G;
    float B;
    float A;

    Color() : R(0), G(0), B(0), A(0) {}
    Color(float val) : R(val), G(val), B(val), A(val) {}
    Color(float r, float g, float b, float a = 1.0f) : R(r), G(g), B(b), A(a) {}

    void clamp() {
        if (R < 0) R = 0;
        if (G < 0) G = 0;
        if (B < 0) B = 0;
        if (A < 0) A = 0;

        if (R > 1.0f) R = 1.0f;
        if (G > 1.0f) G = 1.0f;
        if (B > 1.0f) B = 1.0f;
        if (A > 1.0f) A = 1.0f;
    }

    float getCombined() { return R + G + B; }

    Color& getOffset(float offset) {
        Color col = { R + offset, G + offset, B + offset, A };
        col.clamp();
        return col;
    }

    static Color& Random() {
        return Color(Math::RandomF(0, 1), Math::RandomF(0, 1), Math::RandomF(0, 1), 1);
    }
};
