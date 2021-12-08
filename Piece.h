#pragma once
#include <SDL.h>
#include <string>


class Piece {
public:
	void Render(SDL_Renderer* renderer);
	int piece = -1;
	std::string color;
	int x = -1, y = -1;
	int orgSquare = -1;
	int moves = 0;
};
