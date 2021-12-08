#pragma once
#include "Piece.h"
#include <SDL.h>

class Square {
public:
	int x = -1;
	int y = -1;
	int index = -1;
	Piece piece;
	bool isNULL = true;
	bool hasPiece = false;
};
