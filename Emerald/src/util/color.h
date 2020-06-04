#pragma once

class Color8 {
public:
	uint8 R;
	uint8 G;
	uint8 B;
	uint8 A;

	Color8() : R(0), G(0), B(0), A(0) {}
	Color8(uint8 r, uint8 g, uint8 b, uint8 a) : R(r), G(g), B(b), A(a) {}

	operator uint8* () const { return (uint8*)this; }
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

	union {
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

	Color8 ToColor8();
	void Set(float r, float g, float b);
	void Set(float r, float g, float b, float a);
	void clamp();
	float GetCombined();
	Color GetOffset(float offset);
	
	friend Color Color::operator*(const Color& color, float value) {
		return Color(color.R * value, color.G * value, color.B * value, color.A * value);
	}

	friend Color Color::operator+(const Color& col1, const Color& col2) {
		return Color(col1.R + col2.R, col1.G + col2.G, col1.B + col2.B, col1.A + col2.A);
	}

	friend Color Color::operator-(const Color& col1, const Color& col2) {
		return Color(col1.R - col2.R, col1.G - col2.G, col1.B - col2.B, col1.A - col2.A);
	}
	
	static Color Random(float min = 0);
	static Color RandomPrimary();

	static Color Mix(const Color& col1, const Color& col2, float val);
};
