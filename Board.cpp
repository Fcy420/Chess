#include "Board.h"
#include "constants.h"
#include <SDL.h>
#include <ctype.h>
#include <cmath>
#include <iostream>
#include "Move.h"
#include <vector>
#include "Timer.h"
#include <algorithm>
#include <SDL_ttf.h>


#pragma region GraphicalBoard
void Board::RenderGraphicalBoard(SDL_Renderer* renderer) {
	if (IsMate && !IsStale)
		RenderText("Checkmate", renderer, (consts::WIDTH / 2 - 100) + (consts::WIDTH / 3), consts::HEIGHT / 2 - 50);
	if(IsStale && !IsMate)
		RenderText("Stalemate", renderer, (consts::WIDTH / 2 - 100) + (consts::WIDTH / 3), consts::HEIGHT / 2 - 50);
	if(IsCheck && !IsMate)
		RenderText("Check", renderer, (consts::WIDTH / 2 - 100) + (consts::WIDTH / 3), consts::HEIGHT / 2 - 50);
	for (int y = 0; y < consts::BOARD_SIZE; y++)
	{
		for (int x = 0; x < consts::BOARD_SIZE; x++)
		{
			int index = y * consts::BOARD_SIZE + x;
			bool isLightSquare = (x + y) % 2 != 0;
			int cX = (x * consts::SQUARE_SIZE + consts::WIDTH / 2) - consts::SQUARE_SIZE / 2 * consts::BOARD_SIZE;
			int cY = (y * consts::SQUARE_SIZE + consts::HEIGHT / 2) - consts::SQUARE_SIZE / 2 * consts::BOARD_SIZE;
			if (squares[index].isNULL) {
				Square data;
				data.x = cX;
				data.y = cY;
				data.index = index;
				data.isNULL = false;
				data.hasPiece = squares[index].hasPiece;
				data.piece = squares[index].piece;
				data.piece.x = cX;
				data.piece.y = cY;
				squares[index] = data;
			}
			SDL_Rect rect = { cX, cY, consts::SQUARE_SIZE, consts::SQUARE_SIZE };
			if (isLightSquare) SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255); else SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);
			SDL_RenderFillRect(renderer, &rect);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			if (selectedSquare == index) {
				SDL_Rect selRect = { cX, cY, consts::SQUARE_SIZE, consts::SQUARE_SIZE };
				SDL_SetRenderDrawColor(renderer, 255, 180, 0, 255);
				SDL_RenderFillRect(renderer, &selRect);
			}
			if ((std::find(visualSquares.begin(), visualSquares.end(), index) != visualSquares.end())) {
				SDL_Rect visualRect = { cX, cY, consts::SQUARE_SIZE, consts::SQUARE_SIZE };
				if (squares[index].hasPiece) SDL_SetRenderDrawColor(renderer, 200, 0, 0, 200); else SDL_SetRenderDrawColor(renderer, 0, 160, 0, 100);
				SDL_RenderFillRect(renderer, &visualRect);
			}
			if (squares[index].hasPiece) {
				squares[index].piece.Render(renderer);
			}
		}
	}
}
#pragma endregion
#pragma region Executing Moves
void Board::SelectSquare(int square) {
	if (selectedSquare == -1 && squares[square].hasPiece) {
		if (squares[square].piece.color != ColorToMove) return;
		std::vector<Move> moves;
		moves = FindLegalMoves(square);
		visualSquares.clear();
		selectedSquare = square;
		for (int i = 0; i < moves.size(); i++)
		{
			if (moves[i].targetSquare == -1) continue;
			visualSquares.push_back(moves[i].targetSquare);
		}
	}
	else if (selectedSquare != -1) {
		std::vector<Move> moves = FindLegalMoves(selectedSquare);
		for (int i = 0; i < moves.size(); i++)
		{
			//if (moves[i].startSquare == 11) std::cout << "Illegal move" << "\n";
			//std::cout << moves[i].startSquare << "\n";
			if (square == moves[i].targetSquare && moves[i].targetSquare != -1) {
				MakeMove(moves[i], true);
				break;
			}
		}
		selectedSquare = -1;
		visualSquares.clear();
	}
}
void Board::AddPiece(int index, int piece, std::string color, int moves, int orgSquare) {
	squares[index].piece.color = color;
	squares[index].piece.piece = piece;
	squares[index].piece.orgSquare = orgSquare;
	if (piece == consts::KING) {
		if (color == consts::BLACK) {
			blackKingSquare = index;
		}
		else {
			whiteKingSquare = index;
		}
	}
	squares[index].piece.moves = moves;
	squares[index].hasPiece = true;
}
void Board::RemovePiece(int index) {
	squares[index].hasPiece = false;
}
void Board::MakeMove(Move move, bool record) {
	std::string color = squares[move.startSquare].piece.color;
	if (squares[move.startSquare].piece.color != color) return;
	std::copy(std::begin(squares), std::end(squares), previousBoard);
	RemovePiece(move.startSquare);
	if (move.additionalRemovedPiece != -1) RemovePiece(move.additionalRemovedPiece);
	AddPiece(move.targetSquare, squares[move.startSquare].piece.piece, squares[move.startSquare].piece.color, squares[move.startSquare].piece.moves + 1, squares[move.startSquare].piece.orgSquare);
	if (move.additionalPiece != -1) {
		AddPiece(move.additionalSquare, move.additionalPiece, squares[move.startSquare].piece.color, 0, move.targetSquare);
	}
	if (record) {
		recentMove = move;
		if (color == consts::BLACK) ColorToMove = consts::WHITE; else ColorToMove = consts::BLACK;
		FindMate();
	}
}
#pragma endregion
#pragma region Move Generation

Move Board::FindRandomMove() {
	srand(time(NULL));
	std::vector<Move> moves = GenerateMoves();
	int index = rand() % moves.size();
	//std::cout << index << "\n";
	return moves[index];
}

std::vector<Move> Board::GenerateMoves() {
	std::vector<Move> moves;
	for (int i = 0; i < 64; i++)
	{
		if (squares[i].hasPiece) {
			if (squares[i].piece.color == ColorToMove) {
				std::vector<Move> squareMoves = FindLegalMoves(i);
				for (int i = 0; i < squareMoves.size(); i++)
				{
					moves.push_back(squareMoves[i]);
				}
			}
		}
	}
	return moves;
}

std::vector<int> Board::GenerateAttackingSquares(std::string favour) {
	std::vector<int> targetSquares;
	for (int i = 0; i < 64; i++)
	{
		if (squares[i].hasPiece) {
			if (squares[i].piece.color == favour) {
				std::vector<Move> attackingSquares = FindPsuedoLegalMoves(squares[i].index);
				for (int x = 0; x < attackingSquares.size(); x++)
				{
					targetSquares.push_back(attackingSquares[x].targetSquare);
				}
			}
		}
	}
	return targetSquares;
}
std::vector<Move> Board::FindLegalMoves(int& square) {
	std::string color = squares[square].piece.color;
	std::vector<Move> legalMoves;
	std::vector<Move> moves = FindPsuedoLegalMoves(square);
	for (int i = 0; i < moves.size(); i++)
	{
		//if (moves[i].targetSquare < 0) continue;
		MakeMove(moves[i], false);
		int myKingSquare = (color == consts::BLACK) ? blackKingSquare : whiteKingSquare;
		std::vector<int> oppositeAttackingSquares;
		if (ColorToMove == consts::WHITE) oppositeAttackingSquares = GenerateAttackingSquares(consts::BLACK); else oppositeAttackingSquares = GenerateAttackingSquares(consts::WHITE);
		if (std::find(oppositeAttackingSquares.begin(), oppositeAttackingSquares.end(), myKingSquare) != oppositeAttackingSquares.end()) goto cnt;
		legalMoves.push_back(moves[i]);
		cnt:;
		UndoMove();
	}
	return legalMoves;
}
void Board::UndoMove() {
	std::copy(std::begin(previousBoard), std::end(previousBoard), squares);
	for (int i = 0; i < 64; i++)
	{
		if (squares[i].hasPiece) {
			if (squares[i].piece.piece == consts::KING) {
				if (squares[i].piece.color == consts::WHITE)
					whiteKingSquare = i;
				else
					blackKingSquare = i;
			}
		}
	}
	//if (ColorToMove == consts::WHITE) ColorToMove = consts::BLACK; else ColorToMove = consts::WHITE;
}
std::vector<Move> Board::FindPsuedoLegalMoves(int square) {
	std::vector<Move> psuedoLegalMoves;
	switch (squares[square].piece.piece) {
		case consts::BISHOP:
		case consts::TOWER:
		case consts::KING:
		case consts::QUEEN:
			psuedoLegalMoves = GenerateSlidingPieceMoves(square);
			break;
		case consts::KNIGHT:
			psuedoLegalMoves = GenerateKnightMoves(square);
			break;
		case consts::PAWN:
			psuedoLegalMoves = GeneratePawnMoves(square);
			break;
	}
	std::vector<Move> computedList;
	for (int i = 0; i < psuedoLegalMoves.size(); i++)
	{
		if (psuedoLegalMoves[i].targetSquare != -1) {
			computedList.push_back(psuedoLegalMoves[i]);
		}
	}
	return computedList;
}

std::vector<Move> Board::GenerateKnightMoves(int square) {
	int nextSquares[] = { square + 15,square + 17, square + 10,square + 6,square - 15,square - 17, square - 10,square - 6 };
	std::vector<Move> moves;
	Move move;
	move.startSquare = square;
	for (int i = 0; i < 8; i++)
	{
		int nextSquare = nextSquares[i];
		if (Distance(squares[nextSquare].x, squares[nextSquare].y, squares[square].x, squares[square].y) < 240) {
			if (squares[nextSquare].hasPiece) {
				if (squares[nextSquare].piece.color != squares[square].piece.color)
					move.targetSquare = nextSquare;
					moves.push_back(move);
				continue;
			}
			else {
				move.targetSquare = nextSquare;
				moves.push_back(move);
			}
		}
	}
	return moves;
}

std::vector<Move> Board::GeneratePawnMoves(int square) {
	int direction = (squares[square].piece.color == consts::WHITE) ? -1 : 1;
	int targetSquares[] = { square + 16 * direction, square + 8 * direction, square + 9 * direction, square + 7 * direction };
	std::vector<Move> moves;
	for (int i = 0; i < 4; i++)
	{
		Move move;
		move.startSquare = square;
		switch (i) {
			case 0:
				if (squares[square].piece.moves == 0 && !squares[targetSquares[i]].hasPiece && !squares[square + 8 * direction].hasPiece) {
					move.targetSquare = targetSquares[i];
				}
				break;
			case 1:
				if (!squares[targetSquares[i]].hasPiece) {
					move.targetSquare = targetSquares[i];
				}
				break;
			case 2:
				if (squares[targetSquares[i]].hasPiece && squares[targetSquares[i]].piece.color != squares[square].piece.color) {
					move.targetSquare = targetSquares[i];
				}
				else if (squares[square + direction].hasPiece) {
					if (squares[square + direction].piece.orgSquare == square + direction + 16 * direction && recentMove.targetSquare == square+direction) {
						move.targetSquare = targetSquares[i];
						move.additionalRemovedPiece = square + direction;
					}
				}
				break;
			case 3:
				if (squares[targetSquares[i]].hasPiece && squares[targetSquares[i]].piece.color != squares[square].piece.color) {
					move.targetSquare = targetSquares[i];
				}
				else if (squares[square - direction].hasPiece) {
					if (squares[square - direction].piece.orgSquare == square - direction + 16 * direction && recentMove.targetSquare == square-direction) {
						move.targetSquare = targetSquares[i];
						move.additionalRemovedPiece = square - direction;
					}
				}
				break;
		}
		//if (Distance(squares[targetSquares[i]].x, squares[targetSquares[i]].y, squares[square].x, squares[square].y) > consts::SQUARE_SIZE + consts::SQUARE_SIZE) continue;
		if (targetSquares[i] >= 7 * 8 && targetSquares[i] <= 7 * 8 + 7 || targetSquares[i] >= 0 && targetSquares[i] <= 7) {
			move.additionalPiece = consts::QUEEN;
			move.additionalSquare = targetSquares[i];
		}
		moves.push_back(move);
	}

	return moves;
}

std::vector<Move> Board::GenerateSlidingPieceMoves(int square) {
	int piece = squares[square].piece.piece;
	std::vector<Move> moves;
	int startIndex = -1;
	int endIndex = -1;
	int length = 8;
	Move move;
	move.startSquare = square;
	switch (piece) {
	case consts::BISHOP:
		startIndex = 4;
		endIndex = 8;
		break;
	case consts::TOWER:
		startIndex = 0;
		endIndex = 4;
		break;
	case consts::QUEEN:
		startIndex = 0;
		endIndex = 8;
		break;
	case consts::KING:
		startIndex = 0;
		endIndex = 8;
		length = 2;
		break;
	}
	for (int i = startIndex; i < endIndex; i++)
	{
		if (IsEdgeSquare(square) == 1) {
			if (consts::directions[i] == -1 || consts::directions[i] == 7 || consts::directions[i] == -9) {
				continue;
			}
		}
		else if (IsEdgeSquare(square) == 2) {
			if (consts::directions[i] == 1 || consts::directions[i] == -7 || consts::directions[i] == 9) {
				continue;
			}
		}
		for (int x = 1; x < length; x++)
		{
			int nextSquare = square + consts::directions[i] * x;
			if (nextSquare < 0 || nextSquare > 63) break;
			if (IsEdgeSquare(nextSquare) != 0 && i > 1) {
				if (squares[nextSquare].hasPiece && squares[nextSquare].piece.color == squares[square].piece.color) {
					break;
				}
				else {
					move.targetSquare = nextSquare;
					moves.push_back(move);
					break;
				}
			}
			else {
				if (squares[nextSquare].hasPiece && squares[nextSquare].piece.color == squares[square].piece.color) {
					break;
				}
				else if (squares[nextSquare].hasPiece && squares[nextSquare].piece.color != squares[square].piece.color) {
					move.targetSquare = nextSquare;
					moves.push_back(move);
					break;
				}
				else {
					move.targetSquare = nextSquare;
					moves.push_back(move);
				}
			}
		}
	}
	/*if (piece == consts::KING) {
		Move castle;
		int rightCastleSquare = square + 3;
		int leftCastleSquare = square - 5;
		if (!(rightCastleSquare < 0 || rightCastleSquare > 63)) {
			//std::cout << rightCastleSquare << "\n";
			if (squares[rightCastleSquare].hasPiece) {
				if (squares[rightCastleSquare].piece.moves == 0 && squares[square].piece.moves == 0) {
					if (!squares[square + 1].hasPiece && !squares[square + 2].hasPiece) {
						castle.targetSquare = square + 2;
						castle.additionalPiece = consts::TOWER;
						castle.additionalRemovedPiece = square + 3;
						castle.additionalSquare = square + 1;
						moves.push_back(castle);
					}
				}
			}
		}
	}*/
	return moves;
}



#pragma endregion
#pragma region Tools
void Board::FindMate() {
	int numOfWhiteMoves = 0;
	int numOfBlackMoves = 0;
	bool isCheck = false;
	int myKingSquare = (ColorToMove == consts::WHITE) ? whiteKingSquare : blackKingSquare;
	std::vector<int> oppositeAttackingSquares;
	if (ColorToMove == consts::WHITE) oppositeAttackingSquares = GenerateAttackingSquares(consts::BLACK); else oppositeAttackingSquares = GenerateAttackingSquares(consts::WHITE);
	if (std::find(oppositeAttackingSquares.begin(), oppositeAttackingSquares.end(), myKingSquare) != oppositeAttackingSquares.end()) isCheck = true;
	for (int i = 0; i < 64; i++)
	{
		if (squares[i].hasPiece) {
			if (squares[i].piece.color == consts::WHITE) {
				if (FindLegalMoves(i).size() > 0) numOfWhiteMoves++;
			}
			else {
				if (FindLegalMoves(i).size() > 0) numOfBlackMoves++;
			}

		}
	}
	bool noMoves = (numOfBlackMoves <= 0 || numOfWhiteMoves <= 0);
	if (!IsMate && noMoves) IsStale = true;
	if (!IsMate && !noMoves && isCheck) IsCheck = true; else IsCheck = false;
	if (IsMate || IsStale) finished = true;
	IsMate = noMoves && isCheck;
}
Square Board::FindSquareAtPosition(int& x, int& y) {
	int size = sizeof(squares) / sizeof(squares[0]);
	for (int i = 0; i < size; i++)
	{
		if (Distance(squares[i].x, squares[i].y, x, y) < consts::SQUARE_SIZE) {
			if (squares[i].index >= 0 && squares[i].index <= 63) {
				return squares[i];
			}
		}
	}
	Square nullData;
	nullData.index = -1;
	return nullData;
}

double Board::Distance(int x1, int y1, int x2, int y2) {
	int dstX = (x1 - x2) * (x1 - x2);
	int dstY = (y1 - y2) * (y1 - y2);
	double distance = sqrt(dstX + dstY);
	return distance;
}
int Board::FindPieceFromSymbol(char symbol) {
	switch (symbol) {
	case 'k':
		return consts::KING;
		break;
	case 'p':
		return consts::PAWN;
		break;
	case 'b':
		return consts::BISHOP;
		break;
	case 'r':
		return consts::TOWER;
		break;
	case 'n':
		return consts::KNIGHT;
		break;
	case 'q':
		return consts::QUEEN;
		break;
	}
	return 0;
}
char Board::FindSymbolFromPiece(int piece) {
	switch (piece) {
		case consts::PAWN:
			return 'p';
			break;
		case consts::TOWER:
			return 'r';
			break;
		case consts::QUEEN:
			return 'q';
			break;
		case consts::BISHOP:
			return 'b';
			break;
		case consts::KNIGHT:
			return 'n';
			break;
		case consts::KING:
			return 'k';
			break;
	}
	return '0';
}

void Board::LoadFenPosition(std::string fen) {

	std::string fenBoard = fen.substr(0, fen.find(' '));
	int x = 0;
	int y = 7;
	for (int i = 0; i < fenBoard.size(); i++)
	{
		if (fenBoard[i] == '/') {
			x = 0;
			y--;
		}
		else {
			if (isdigit(fenBoard[i])) {
				int num = fenBoard[i] - '0';
				x += num;
			}
			else {
				std::string color = (isupper(fenBoard[i])) ? consts::BLACK : consts::WHITE;
				int value = FindPieceFromSymbol((char)tolower(fenBoard[i]));
				int square = y * 8 + x;
				int moves = 0;
				if(value == consts::PAWN && ((square < 1*8 && square > 1*8+7) || (square < 6*8 && square > 6*8+7))) moves = 1;
				AddPiece(square, value, color, moves, square);
				x++;
			}
		}
	}
	ColorToMove = consts::WHITE;
}


int Board::IsEdgeSquare(int square) {
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (square == y * 8) return 1;
			if (square == y * 8 + 7) return 2;
		}
	}
	return 0;
}

void Board::ShowAttackingSquares(std::string favour) {
	std::vector<int> moves = GenerateAttackingSquares(favour);
	visualSquares = moves;
}
void Board::ClearSelection() {
	selectedSquare = -1;
	visualSquares.clear();
}
int Board::EvaluateBoard() {
	int values[] = { 100, 300, 400, 250, 900, 100 };
	int value = 0;
	for (int i = 0; i < 8; i++)
	{
		for (int i = 0; i < 8; i++)
		{
			if (squares[i].hasPiece) {
				//std::cout << ColorToMove << "\n";
				if (squares[i].piece.color == ColorToMove) {
					value += values[squares[i].piece.piece];
				}
			}
		}
	}
	return value;
}

void Board::RenderText(std::string text, SDL_Renderer* renderer, int x, int y) {
	TTF_Init();
	TTF_Font* font = TTF_OpenFont("Fonts/title.ttf", 128);
	SDL_Color color = { 255, 255, 255 };
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = 250;
	rect.h = 100;
	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(texture);
}
#pragma endregion
