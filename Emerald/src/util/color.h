#pragma once

class Color8 {
public:
	uint8 R;
	uint8 G;
	uint8 B;
	uint8 A;

	Color8() : R(0), G(0), B(0), A(0) {}
	Color8(uint8 r, uint8 g, uint8 b, uint8 a) : R(r), G(g), B(b), A(a) {}

	operator uint8*() const { return (uint8*)this; }
};

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

	static Color Transparent() { return Color(0, 0, 0, 0); }
	static Color NormalMap() { return Color(0.5f, 0.5f, 1); }

union
	{
	struct {
		float R;
		float G;
		float B;
		float A;
	};
	float values[4];
	};

	Color() : R(0), G(0), B(0), A(0) {}
	Color(int32 hexValue) {
		R = ((hexValue >> 16) & 0xFF) / 255.0f;
		G = ((hexValue >> 8) & 0xFF) / 255.0f;
		B = ((hexValue) & 0xFF) / 255.0f;
		A = 1;
	}
	Color(float val) : R(val), G(val), B(val), A(val) {}
	Color(float r, float g, float b, float a = 1.0f) : R(r), G(g), B(b), A(a) {}

	Color8 ToColor8() {
		uint8 r = (uint8)Math::Round(R * 255);
		uint8 g = (uint8)Math::Round(G * 255);
		uint8 b = (uint8)Math::Round(B * 255);
		uint8 a = (uint8)Math::Round(A * 255);
		return Color8(r, g, b, a);
	}
	void Set(float r, float g, float b) {
		R = r;
		G = g;
		B = b;
	}

	void Set(float r, float g, float b, float a) {
		Set(r, g, b);
		A = a;
	}

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

	Color getOffset(float offset) {
		Color col = { R + offset, G + offset, B + offset, A };
		col.clamp();
		return col;
	}

	friend Color operator*(const Color& color, float value) {
		return Color(color.R * value, color.G * value, color.B * value, color.A * value);
	}

	friend Color operator+(const Color& col1, const Color& col2) {
		return Color(col1.R + col2.R, col1.G + col2.G, col1.B + col2.B, col1.A + col2.A);
	}

	friend Color operator-(const Color& col1, const Color& col2) {
		return Color(col1.R - col2.R, col1.G - col2.G, col1.B - col2.B, col1.A - col2.A);
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

	static Color Mix(const Color& col1, const Color& col2, float val) {
		return col1 * (1 - val) + col2 * val;
	}
};
