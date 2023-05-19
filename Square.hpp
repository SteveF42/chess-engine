#ifndef Square_H
#define Square_H
#include "Piece.hpp"

class Square
{
private:
    Piece *piece = nullptr;
    int squarePosition;

public:
    Square(int pos){
        squarePosition = pos;
    }

    int getSquarePosition() {return squarePosition;}
    bool hasNullPiece() { return piece == nullptr; }
    Piece *getPiece() { return piece; }
    void setPiece(Piece *other) { piece = other; }
};

#endif