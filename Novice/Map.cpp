
#include "Map.h"
#include "Config.h"
#include <cmath>

// ===================== 初期化 =====================

void Map::Initialize() {
	rows_.clear();

	// 左右の壁を作らず、すべて空白にする
	for (int y = 0; y < kMapRows; y++) {
		std::string row(kMapCols, '.');
		rows_.push_back(row);
	}
}

// ===================== 当たり判定 =====================

bool Map::IsSolid(int tx, int ty) const {
	// マップ外も壁として扱わない
	if (tx < 0 || tx >= Cols()) {
		return false;
	}

	if (ty < 0 || ty >= Rows()) {
		return false;
	}

	// '#' のタイルだけ当たり判定あり
	return rows_[ty][tx] == '#';
}

// ===================== ピクセル座標での判定 =====================

bool Map::IsSolidAtPixel(float px, float py) const { return IsSolid((int)std::floor(px / kTileSize), (int)std::floor(py / kTileSize)); }

// ===================== 描画 =====================

void Map::Draw() const {
	// 左右の壁を描画しない
}
