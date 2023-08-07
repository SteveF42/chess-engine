#ifndef MoveGeneration_H
#define MoveGeneration_H

#include <vector>
#include "Move.hpp"
#include "Square.hpp"
#include "PieceList.hpp"

struct CheckOrPin
{
    int position;
    int direction;

    CheckOrPin(int pos, int dir)
    {
        position = pos;
        direction = dir;
    }
    CheckOrPin()
    {
        position = -1;
        direction = -1;
    }
};

class MoveGeneration
{
private:
    const int slidingMovesOffsets[8] = {1, -1, 8, -8, 7, -7, 9, -9};
    const int kingMovesOffsets[8] = {1, 7, 8, 9, -1, -7, -8, -9};
    const int knightOffset[8] = {6, 10, 15, 17, -6, -10, -15, -17};

    bool checkFlag;
    bool quietSearch;

    std::vector<CheckOrPin> pins;
    std::vector<CheckOrPin> checks;
    std::map<int, std::vector<Move>> moveset;
    bool gameOver = false;

    Square **board;
    std::vector<Move> getSlidingTypeMoves(Piece *other);
    std::vector<Move> getPawnMoves(Piece *other);
    std::vector<Move> getKnightMoves(Piece *other);
    std::vector<Move> getKingMoves(Piece *other);
    void checkForPinsAndChecks(std::vector<CheckOrPin> &pins, std::vector<CheckOrPin> &checks, bool &inCheck);
    void getCastleMoves(std::vector<Move> &validMoves, Piece *kingPiece);
    bool squareUnderAttack(int square, int color);
    void boardEdgeData();
    std::map<int, std::vector<Move>> pieceAvailableMoves();
    int numSquaresToEdge[64][8];

    Piece *blackKing;
    Piece *whiteKing;

public:
    bool blackCastleKingSide = false;
    bool whiteCastleKingSide = false;
    bool blackCastleQueenSide = false;
    bool whiteCastleQueenSide = false;
    int possibleEnPassant;

    PieceList pieceList;
    bool getCheck() { return checkFlag; }
    std::map<int, std::vector<Move>> getMoves() { return moveset; }
    void setMoves(std::map<int, std::vector<Move>> moves) { moveset = moves; }
    void generateMovesInCurrentPosition(bool quietSearch = false);
    void setWhiteKing(Piece *king) { whiteKing = king; }
    void setblackKing(Piece *king) { blackKing = king; }
    Piece *getWhiteKing() { return whiteKing; }
    Piece *getBlackKing() { return blackKing; }
    bool getGameOver()
    {
        return moveset.size() == 0;
    }

    void setBoardRef(Square **boardRef)
    {
        board = boardRef;
        boardEdgeData();
    }
    int getPossibleEnPassant() { return possibleEnPassant; }

    MoveGeneration() {}
};

#endif