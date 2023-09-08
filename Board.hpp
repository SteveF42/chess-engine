#ifndef Board_H
#define Board_H
#include "PieceList.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "NewMoveGenerator.hpp"
#include "Zobrist.hpp"
#include <string>
#include <stack>

struct CastlingRights
{
    bool blackCastleKingSide = false;
    bool whiteCastleKingSide = false;
    bool blackCastleQueenSide = false;
    bool whiteCastleQueenSide = false;
};

class Board
{
private:
    std::stack<std::vector<Move>> movesetHistory;
    std::stack<Move> moveHistory;
    Square *board[64];

    std::stack<CastlingRights> castlingHistory;
    Piece *blackKing;
    Piece *whiteKing;

    int numSquaresToEdge[64][8];

    uint64_t zobristKey;
    std::stack<uint64_t> zobristKeyHistory;

    // class methods
    void updateCastlingRights(const Move &move);

public:
    static bool whiteToMove;
    // first four offsets are rook type moves and the second are bishop like moves, all can be used for the queen
    Move unmakeMove();
    void makeMove(Move move);
    std::vector<Move> getPieceMoves(int idx);
    bool validateMove(int startIdx, int target);

    NewMoveGenerator moveGeneration;
    Zobrist zobrist;
    PieceList pieceList;

    Board()
    {
        for (int i = 0; i < 64; i++)
        {
            board[i] = new Square(i);
        }
    }

    //@colorIndex PieceList index type
    Piece *getKing(int colorIndex)
    {
        if (colorIndex == PieceList::whiteIndex)
        {
            return whiteKing;
        }
        else
        {
            return blackKing;
        }
    }

    Square **getBoard() { return board; }
    void setSquarePiece(int idx, Piece *other);
    void setZobristKey(uint64_t zobrist) { zobristKey = zobrist; }

    void setWhiteToMove(bool t) { whiteToMove = t; }
    void setBlackCastleKingSide(bool t);
    void setWhiteCastleKingSide(bool t);
    void setBlackCastleQueenSide(bool t);
    void setWhiteCastleQueenSide(bool t);

    bool getWhiteToMove() { return whiteToMove; }
};

#endif