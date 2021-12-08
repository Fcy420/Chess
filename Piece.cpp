#include "Piece.h"
#include <SDL.h>
#include <SDL_image.h>
#include "constants.h"
#include <iostream>

void Piece::Render(SDL_Renderer* renderer) {
	if (IMG_Init(IMG_INIT_PNG)) {
		SDL_Surface* surface;
		std::string path = "";
		bool located = false;
		switch (piece) {
			case 1:
				path = "../Chess/" + color + "/pawn.png";
				located = true;
				break;
			case 2:
				path = "../Chess/" + color + "/bishop.png";
				located = true;
				break;
			case 3:
				path = "../Chess/" + color + "/knight.png";
				located = true;
				break;
			case 4:
				path = "../Chess/" + color + "/tower.png";
				located = true;
				break;
			case 5:
				path = "../Chess/" + color + "/queen.png";
				located = true;
				break;
			case 6:
				path = "../Chess/" + color + "/king.png";
				located = true;
				break;
		}
		if (located) {
			surface = IMG_Load(path.c_str());
			SDL_Texture* texture;
			texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_Rect rect = { x-consts::PIECE_SIZE/8, y-consts::PIECE_SIZE/8, consts::PIECE_SIZE, consts::PIECE_SIZE };
			SDL_RenderCopy(renderer, texture, NULL, &rect);
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(surface);
		}
	}
}