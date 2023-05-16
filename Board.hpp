#ifndef Board_H
#define Board_H
#include <string>
#include "ReadFen.hpp"
#include "Square.hpp"
#include <stack>

struct Position
{
    Position()
    {
        for (int i = 0; i < 64; i++)
        {
            board[i] = 0;
        }
    }
    int board[64];
    bool whiteToMove = true;
    bool blackCastleKingSide = false;
    bool whiteCastleKingSide = false;
    bool blackCastleQueenSide = false;
    bool whiteCastleQueenSide = false;
};

class Board
{
    Position currentPosition;
    std::stack<Position *> history;

private:
public:
    Board()
    {
        currentPosition = ReadFen::readFenString(ReadFen::startingString);
    }

    int *getBoard() { return currentPosition.board; }
    void loadBoard();

    void makeMove();
};

#endif