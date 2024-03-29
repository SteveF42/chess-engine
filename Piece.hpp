#ifndef Piece_H
#define Piece_H
#include <iostream>

class Piece
{

private:
    static const int typeMask = 0b00111;
    static const int blackMask = 0b10000;
    static const int whiteMask = 0b01000;
    static const int colourMask = whiteMask | blackMask;

    int pieceType;
    int currentPosition;

public:
    static const int PAWN = 1;
    static const int KNIGHT = 2;
    static const int ROOK = 3;
    static const int BISHOP = 4;
    static const int QUEEN = 5;
    static const int KING = 6;

    static const int BLACK = 8;
    static const int WHITE = 16;
    static int getPieceColor(int piece) { return piece & colourMask; }
    static int getPieceType(int piece) { return piece & typeMask; }
    static int isColor(int piece, int color) { return (piece & colourMask) == color; }

    Piece(int pieceType, int piecePosition);

    int getPieceType()
    {
        return getPieceType(pieceType);
    }
    int getPieceTypeRaw() { return this->pieceType; }
    void promoteType(int pieceType);

    int getPieceColor() { return getPieceColor(pieceType); }
    int getPiecePosition() { return currentPosition; }
    void setPiecePosition(int positionOnBoard);
};

#endif