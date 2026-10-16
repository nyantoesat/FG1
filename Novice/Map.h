#pragma once
#include <string>
#include <vector>

// The shaft walls. 1 character = 1 tile (40 x 40 px).
//   '#' = wall (solid, can be climbed)
//   '.' = empty
// Top and bottom of the screen are open.
class Map {
public:
	void Initialize();
	void Draw() const;

	bool IsSolid(int tx, int ty) const;            // tile coordinates
	bool IsSolidAtPixel(float px, float py) const; // pixel coordinates
	int Cols() const { return (int)rows_[0].size(); }
	int Rows() const { return (int)rows_.size(); }

private:
	std::vector<std::string> rows_;
};