#ifndef Board_H
#define Board_H
#include <string>
#include "ReadFen.hpp"
#include "Square.hpp"
#include <stack>

class Board
{
private:
    Square *board[64];
    bool whiteToMove = true;
    bool blackCastleKingSide = false;
    bool whiteCastleKingSide = false;
    bool blackCastleQueenSide = false;
    bool whiteCastleQueenSide = false;

public:
    Board()
    {
        for (int i = 0; i < 64; i++)
        {
            board[i] = new Square();
        }
    }

    Square **getBoard() { return board; }
    void setSquarePiece(int idx, Piece *other)
    {
        board[idx]->setPiece(other);
    }

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