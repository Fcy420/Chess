#pragma once
#include <string>
#include <vector>
#include <cmath>

namespace consts {
	static const int WIDTH = 1620;
	static const int HEIGHT = 880;
	static const int BOARD_SIZE = 8;
	static const int SQUARE_SIZE = 60;
	static const int PIECE_SIZE = 80;

	static const int PAWN = 1;
	static const int BISHOP = 2;
	static const int TOWER = 4;
	static const int KNIGHT = 3;
	static const int QUEEN = 5;
	static const int KING = 6;

	static const std::string BLACK = "black";
	static const std::string WHITE = "white";

	static int directions[] = { -8, 8, 1, -1, 7, -7, 9, -9 };
};
