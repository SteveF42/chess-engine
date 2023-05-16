#ifndef Square_H
#define Square_H

#include "Piece.hpp"
class Square
{
private:
    int piece;
    int squarePosition;

public:
    Square(int position)
    {
        squarePosition = position;
    }
    int getSquarePosition() { return squarePosition; }
    int getPiece() { return piece; }
    void setSquarePosition(int pos) { squarePosition = pos; }
    void setPieceType(Piece* piece) { piece = piece; }
};

#endif