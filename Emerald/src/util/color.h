#pragma once

class Color {
public:
	static Color White() { return Color(1, 1, 1); }
	static Color Black() { return Color(0, 0, 0); }
	static Color Gray() { return Color(0.5f, 0.5f, 0.5f); }
	static Color Silver() { return Color(0.75f, 0.75f, 0.75f); }

	static Color Red() { return Color(1, 0, 0); }
	static Color Green() { return Color(0, 1, 0); }
	static Color Blue() { return Color(0, 0, 1); }

	static Color Yellow() { return Color(1, 1, 0); }
	static Color Magenta() { return Color(1, 0, 1); }
	static Color Cyan() { return Color(0, 1, 1); }
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

	static Color& Random(float min = 0) {
		float rr = Math::RandomFloat(0, 1);
		float gg = Math::RandomFloat(0, 1);
		float bb = Math::RandomFloat(0, 1);
		if (rr + gg + bb < min) return Random(min);
		return Color(rr, gg, bb);
	}

	static Color RandomPrimary() {
		switch (Math::RandomInt(0, 5)) {
		case 0: return Red();
		case 1: return Green();
		case 2: return Blue();
		case 3: return Yellow();
		case 4: return Magenta();
		case 5: return Cyan();
		}
		return White();
	}
};
