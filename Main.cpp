#include <stdio.h>
#include <SDL.h>
#include <iostream>
#include "constants.h"
#include "Board.h"
#include "Timer.h"
#include <SDL_image.h>



int main(int argc, char* args[]) {
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	SDL_Renderer* renderer = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL Error");
	}
	else {
		window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, consts::WIDTH, consts::HEIGHT, SDL_WINDOW_OPENGL);
		if (window == NULL) {
			printf("Window could not initialize");
		}
		else {
			surface = SDL_GetWindowSurface(window);
			SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));

			SDL_UpdateWindowSurface(window);

			renderer = SDL_CreateRenderer(window, -1, 0);
			Board board;
			board.LoadFenPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
			while (true) {
				SDL_Event e;
				int buttons;
				int mouseX, mouseY;
				buttons = SDL_GetMouseState(&mouseX, &mouseY);
				if (board.ColorToMove == consts::BLACK) {
					if (!board.finished) {
						{
							Timer timer;
							board.MakeMove(board.FindRandomMove(), true);
						}
					}
				}
				if (SDL_WaitEvent(&e) != 0) {
					if (e.type == SDL_QUIT) {
						break;
					}
					else if (e.type == SDL_KEYDOWN) {
						if (e.key.keysym.sym == SDLK_w) {
							board.ShowAttackingSquares(consts::WHITE);
						}
						else if (e.key.keysym.sym == SDLK_b) {
							board.ShowAttackingSquares(consts::BLACK);
						}
						else if (e.key.keysym.sym == SDLK_1) {
							board.UndoMove();
						}
						else if (e.key.keysym.sym == SDLK_ESCAPE) {
							board.ClearSelection();
						}
						else if (e.key.keysym.sym == SDLK_c) {
							int value = board.EvaluateBoard();
							std::cout << value << "\n";
						}
					}
					else if (e.type == SDL_MOUSEBUTTONDOWN) {
						Square data = board.FindSquareAtPosition(mouseX, mouseY);
						if (data.index != -1) {
							board.SelectSquare(data.index);
						}
					}
				}

				board.RenderGraphicalBoard(renderer);
				SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
				SDL_RenderPresent(renderer);
				SDL_RenderClear(renderer);
			}
		}
	}

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}