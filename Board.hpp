#ifndef Board_H
#define Board_H
#include <string>
#include "ReadFen.hpp"
#include "Square.hpp"
#include <stack>

struct Move
{
    int start;
    int target;
    Move(int s, int t)
    {
        start = s;
        target = t;
    }
};

class Board
{
private:
    void boardEdgeData();
    int numSquaresToEdge[64][8];

    Square *board[64];
    bool whiteToMove = true;
    bool blackCastleKingSide = false;
    bool whiteCastleKingSide = false;
    bool blackCastleQueenSide = false;
    bool whiteCastleQueenSide = false;

    std::vector<Move> getSlidingTypeMoves(Piece *other);
    std::vector<Move> getPawnMoves(Piece *other);
    std::vector<Move> getKnightMoves(Piece *other);
    std::vector<Move> getKingMoves(Piece *other);
    const int slidingMovesOffsets[8] = {1, -1, 8, -8, 7, -7, 9, -9};
    const int kingMovesOffsets[8] = {1, 7, 8, 9, -1, -7, -8, -9};
    const int knightOffset[8] = {6, 10, 15, 17, -6, -10, -15, -17};
    std::vector<std::vector<Move>> moveset;
    std::vector<Move> pieceAvailableMoves(Piece *piece);

public:
    // first four offsets are rook type moves and the second are bishop like moves, all can be used for the queen
    void generateMovesInCurrentPosition();
    std::vector<Move> getPieceMoves(int idx);
    bool validateMove(int startIdx, int target)
    {
        for (auto move : moveset[startIdx])
        {
            if (move.target == target)
            {
                // call a move swap function or something
                return true;
            }
        }
        return false;
    }

    Board()
    {
        for (int i = 0; i < 64; i++)
        {
            board[i] = new Square(i);
        }
        boardEdgeData();
    }

    Square **getBoard() { return board; }
    void setSquarePiece(int idx, Piece *other)
    {
        board[idx]->setPiece(other);
    }
    void getBoardMoves(int *moves);
    void makeMove(Piece *current, int indxTo);

    void setWhiteToMove(bool t) { whiteToMove = t; }
    void setBlackCastleKingSide(bool t) { blackCastleKingSide = t; }
    void setWhiteCastleKingSide(bool t) { whiteCastleKingSide = t; }
    void setBlackCastleQueenSide(bool t) { blackCastleQueenSide = t; }
    void setWhiteCastleQueenSide(bool t) { whiteCastleQueenSide = t; }

    bool getWhiteToMove() { return whiteToMove; }
    bool getBlackCastleKingSide() { return blackCastleKingSide; }
    bool getWhiteCastleKingSide() { return whiteCastleKingSide; }
    bool getBlackCastleQueenSide() { return blackCastleQueenSide; }
    bool getWhiteCastleQueenSide() { return whiteCastleQueenSide; }
};

#endif