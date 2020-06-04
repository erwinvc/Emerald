#include "stdafx.h"

Color8 Color::ToColor8() {
	uint8 r = (uint8)Math::Round(R * 255);
	uint8 g = (uint8)Math::Round(G * 255);
	uint8 b = (uint8)Math::Round(B * 255);
	uint8 a = (uint8)Math::Round(A * 255);
	return Color8(r, g, b, a);
}
void Color::Set(float r, float g, float b) {
	R = r;
	G = g;
	B = b;
}

void Color::Set(float r, float g, float b, float a) {
	Set(r, g, b);
	A = a;
}

void Color::clamp() {
	if (R < 0) R = 0;
	if (G < 0) G = 0;
	if (B < 0) B = 0;
	if (A < 0) A = 0;

	if (R > 1.0f) R = 1.0f;
	if (G > 1.0f) G = 1.0f;
	if (B > 1.0f) B = 1.0f;
	if (A > 1.0f) A = 1.0f;
}

float Color::GetCombined() { return R + G + B; }

Color Color::GetOffset(float offset) {
	Color col = { R + offset, G + offset, B + offset, A };
	col.clamp();
	return col;
}

Color Color::Random(float min) {
	float rr = Random::Float(0, 1);
	float gg = Random::Float(0, 1);
	float bb = Random::Float(0, 1);
	if (rr + gg + bb < min) return Random(min);
	return Color(rr, gg, bb);
}

Color Color::RandomPrimary() {
	switch (Random::Int(0, 5)) {
		case 0: return Red();
		case 1: return Green();
		case 2: return Blue();
		case 3: return Yellow();
		case 4: return Magenta();
		case 5: return Cyan();
	}
	return White();
}

Color Color::Mix(const Color& col1, const Color& col2, float val) {
	return col1 * (1 - val) + col2 * val;
}