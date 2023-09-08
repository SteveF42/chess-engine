#ifndef NewMoveGenerator_H
#define NewMoveGenerator_H

#include <vector>
#include "Move.hpp"
#include "PreComputedMoveData.hpp"

class Board;

class NewMoveGenerator
{
public:
	std::vector<Move> generateMoves(Board *board, bool includeQuietMoves = true);
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

	void setMoves(std::vector<Move> moves) { this->moves = moves; }
	bool containsSquareInPawnAttackMap(int square);
	std::vector<Move> getMoves() { return moves; }
	bool getGameOver()
	{
		return moves.size() == 0;
	}

private:
	const int slidingMovesOffsets[8] = {1, -1, 8, -8, 7, -7, 9, -9};
	const int kingMovesOffsets[8] = {1, 7, 8, 9, -1, -7, -8, -9};
	const int knightOffset[8] = {6, 10, 15, 17, -6, -10, -15, -17};

	std::vector<Move> moves;
	bool isWhiteToMove;
	int friendlyColour;
	int opponentColour;
	Piece *friendlyKing;
	int friendlyColourIndex;
	int opponentColourIndex;

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

	void Init();

	void generateKingMoves();
	bool squareIsAttacked(int square);
	bool squareIsInCheckRay(int square);
	void getCastleMoves();
	void calculateAttackData();
	void genSlidingAttackMap();
	void updateSlidingAttackPiece(int startSquare, int startDirIndex, int endDirIndex);
	bool containsSquare(uint64_t bitboard, int square);
	void generateSlidingMoves();
	void generateSlidingPieceMoves(int startSquare, int startDirIndex, int endDirIndex);
	void generateKnightMoves();
	bool isPinned(int square);
	bool inCheckAfterEnPassant(int startSquare, int targetSquare, int epCapturedPawnSquare);
	bool squareAttackedAfterEPCapture(int epCaptureSquare, int capturingPawnStartSquare);
	void generatePawnMoves();
	bool isMovingAlongRay(int rayDir, int startSquare, int targetSquare);

	// void MakePromotionMoves(int fromSquare, int toSquare)
	// {
	// 	moves.Add(new Move(fromSquare, toSquare, Move.Flag.PromoteToQueen));
	// 	if (promotionsToGenerate == PromotionMode.All)
	// 	{
	// 		moves.Add(new Move(fromSquare, toSquare, Move.Flag.PromoteToKnight));
	// 		moves.Add(new Move(fromSquare, toSquare, Move.Flag.PromoteToRook));
	// 		moves.Add(new Move(fromSquare, toSquare, Move.Flag.PromoteToBishop));
	// 	}
	// 	else if (promotionsToGenerate == PromotionMode.QueenAndKnight)
	// 	{
	// 		moves.Add(new Move(fromSquare, toSquare, Move.Flag.PromoteToKnight));
	// 	}
	// }
};

#endif