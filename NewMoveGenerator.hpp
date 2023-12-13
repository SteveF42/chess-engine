#ifndef NewMoveGenerator_H
#define NewMoveGenerator_H

#include <vector>
#include "Move.hpp"
#include "PreComputedMoveData.hpp"
#include "Magic.hpp"

class Board;

class NewMoveGenerator
{
public:
	std::vector<Move> &generateMoves(Board *board, bool includeQuietMoves = true);
	PrecomputedMoveData preComputedMoveData;
	const static int noEnPessant = -1;

	// Note, this will only return correct value after GenerateMoves() has been called in the current position
	bool isCheck()
	{
		return inCheck;
	}

	bool blackCastleKingSide = false;
	bool whiteCastleKingSide = false;
	bool blackCastleQueenSide = false;
	bool whiteCastleQueenSide = false;
	int possibleEnPassant = noEnPessant;

	bool containsSquareInPawnAttackMap(int square);
	std::vector<Move> &getMoves() { return moves; }
	uint64_t getAttackMap() { return opponentAttackMap; }
	uint64_t getPawnAttackMap() { return opponentPawnAttackMap; }
	bool getGameOver()
	{
		return moves.size() == 0;
	}

private:
	const int slidingMovesOffsets[8] = {1, -1, 8, -8, 7, -7, 9, -9};
	const int kingMovesOffsets[8] = {1, 7, 8, 9, -1, -7, -8, -9};
	const int knightOffset[8] = {6, 10, 15, 17, -6, -10, -15, -17};
	const int MAX_MOVES = 218;
	int currentMoveIndex;

	std::vector<Move> moves;
	bool isWhiteToMove;
	int friendlyColor;
	int opponentColor;
	Piece *friendlyKing;
	int friendlyColorIndex;
	int opponentColorIndex;

	uint64_t friendlyPieces;
	uint64_t opponentPieces;
	uint64_t moveTypeMask;
	uint64_t allPieces;
	uint64_t emptySquares;
	uint64_t emptyOrEnemySquares;

	bool inCheck;
	bool inDoubleCheck;
	bool pinsExistInPosition;
	uint64_t checkRayBitmask;
	uint64_t pinRayBitmask;
	uint64_t opponentKnightAttacks;
	uint64_t opponentAttackMapNoPawns;
	uint64_t opponentAttackMap;
	uint64_t opponentPawnAttackMap;
	uint64_t opponentSlidingAttackMap;

	bool genQuiets;
	Board *board;
	Magic magic;

	void Init();
	void makePromotionMoves(int start, int target);
	void generateKingMoves();
	bool squareIsAttacked(int square);
	bool squareIsInCheckRay(int square);
	void getCastleMoves();
	void calculateAttackData();
	void genSlidingAttackMap();
	void updateSlidingAttackPiece(int startSquare, int startDirIndex, int endDirIndex);
	bool containsSquare(uint64_t bitboard, int square);
	void generateSlidingPieceMoves();
	void generateKnightMoves();
	bool isPinned(int square);
	bool inCheckAfterEnPassant(int startSquare, int targetSquare, int epCapturedPawnSquare);
	void generatePawnMoves();
};

#endif