#include "Piece.hpp"
#include "GameEngine.hpp"
#include <iostream>

// const int Piece::PAWN = 1;
// const int Piece::KNIGHT = 2;
// const int Piece::ROOK = 3;
// const int Piece::BISHOP = 4;
// const int Piece::QUEEN = 5;
// const int Piece::KING = 6;
// const int Piece::BLACK = 8;
// const int Piece::WHITE = 16;

// const int typeMask = 0b00111;
// const int blackMask = 0b10000;
// const int whiteMask = 0b01000;
// const int colourMask = whiteMask | blackMask;
float scale;

Piece::Piece(int pieceType, int piecePosition)
{
    currentPosition = piecePosition;
    this->pieceType = pieceType;
}

void Piece::setPiecePosition(int positionOnBoard)
{
    currentPosition = positionOnBoard;
}

void Piece::promoteType(int pieceType)
{
    this->pieceType = pieceType | this->getPieceColor();
}