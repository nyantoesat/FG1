
#pragma once

#include <string>
#include <vector>

// マップ管理クラス
// '.' = 空白
// '#' = 壁（現在は使用しない）
class Map {
public:
	// 初期化
	void Initialize();

	// 描画
	void Draw() const;

	// タイル座標での当たり判定
	bool IsSolid(int tx, int ty) const;

	// ピクセル座標での当たり判定
	bool IsSolidAtPixel(float px, float py) const;

	// マップの横幅
	int Cols() const { return rows_.empty() ? 0 : (int)rows_[0].size(); }

	// マップの縦幅
	int Rows() const { return (int)rows_.size(); }

private:
	std::vector<std::string> rows_;
};
