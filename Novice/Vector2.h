#pragma once

struct Vector2 {
	float x;
	float y;
};

inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }

inline float Clamp(float v, float min, float max) {
	if (v < min) {
		return min;
	}
	if (v > max) {
		return max;
	}
	return v;
}