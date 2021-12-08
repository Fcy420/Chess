#pragma once
#include "constants.h"
#include <SDL.h>
#include "SquareData.h"
#include "Move.h"
#include <vector>
#include <string>
#include <array>

class Board {
public:
	void RenderGraphicalBoard(SDL_Renderer* renderer);
	Square FindSquareAtPosition(int& x, int& y);
	void LoadFenPosition(std::string fen);
	int FindPieceFromSymbol(char symbol);
	void SelectSquare(int square);
	int selectedSquare = -1;
	void ShowAttackingSquares(std::string favour);
	void MakeMove(Move move, bool record);
	std::vector<int> GenerateAttackingSquares(std::string favour);
	void ClearSelection();
	void UndoMove();
	std::string ColorToMove;
	Move FindRandomMove();
	bool finished = false;
	int EvaluateBoard();
private:
	Square squares[64];
	Square previousBoard[64];
	std::vector<int> visualSquares;
	double Distance(int x1, int y1, int x2, int y2);
	void RemovePiece(int index);
	void AddPiece(int index, int piece, std::string color, int moves, int orgSquare);
	std::vector<Move> FindPsuedoLegalMoves(int square);
	std::vector<Move> GenerateSlidingPieceMoves(int square);
	std::vector<Move> GenerateKnightMoves(int square);
	std::vector<Move> GeneratePawnMoves(int square);
	std::vector<Move> FindLegalMoves(int& square);
	int IsEdgeSquare(int square);
	int whiteKingSquare = -1;
	int blackKingSquare = -1;
	Move recentMove;
	char FindSymbolFromPiece(int piece);
	void RenderText(std::string text, SDL_Renderer* renderer, int x, int y);
	bool IsMate = false;
	bool IsStale = false;
	bool IsCheck = false;
	void FindMate();
	std::vector<Move> GenerateMoves();
};