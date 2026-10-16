#include "Map.h"
#include "Config.h"
#include <Novice.h>
#include <cmath>

void Map::Initialize() {
	// Build the shaft: thick walls on both sides, empty in the middle.
	// (You can also write the rows by hand to add ledges later.)
	rows_.clear();
	for (int y = 0; y < kMapRows; y++) {
		std::string row(kWallTiles, '#');
		row += std::string(kMapCols - kWallTiles * 2, '.');
		row += std::string(kWallTiles, '#');
		rows_.push_back(row);
	}
}

bool Map::IsSolid(int tx, int ty) const {
	if (tx < 0 || tx >= Cols()) {
		return true; // outside left / right = wall
	}
	if (ty < 0) {
		ty = 0;
	} // above the screen: same as the top row (walls continue up)
	if (ty >= Rows()) {
		ty = Rows() - 1;
	} // below the screen: walls continue down
	return rows_[ty][tx] == '#';
}

bool Map::IsSolidAtPixel(float px, float py) const { return IsSolid((int)std::floor(px / kTileSize), (int)std::floor(py / kTileSize)); }

void Map::Draw() const {
	// PLACEHOLDER ART: replace with your own wall sprites later
	for (int ty = 0; ty < Rows(); ty++) {
		for (int tx = 0; tx < Cols(); tx++) {
			if (rows_[ty][tx] == '#') {
				Novice::DrawBox(tx * kTileSize, ty * kTileSize, kTileSize, kTileSize, 0.0f, 0x23465AFF, kFillModeSolid);
			}
		}
	}
}