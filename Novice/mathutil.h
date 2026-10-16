#pragma once
#include <cmath>
#include <random>

struct Vector2 {
	float x;
	float y;
};

// Axis-aligned rectangle (top-left + size)
struct Rect {
	float x, y, w, h;
};

inline bool Intersects(const Rect& a, const Rect& b) { return a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h; }

inline float Clamp(float v, float min, float max) {
	if (v < min) {
		return min;
	}
	if (v > max) {
		return max;
	}
	return v;
}

// Random number helpers
inline std::mt19937& RandomEngine() {
	static std::mt19937 engine{std::random_device{}()};
	return engine;
}
inline float RandomFloat(float min, float max) { return std::uniform_real_distribution<float>(min, max)(RandomEngine()); }
inline int RandomInt(int min, int max) { return std::uniform_int_distribution<int>(min, max)(RandomEngine()); }