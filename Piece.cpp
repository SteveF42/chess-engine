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
void Piece::setPieceSprite(sf::Sprite *other, sf::Vector2f pos)
{
    sprite = other;
    spritePosition = pos;
    sprite->setPosition(pos);
    scale = GameEngine::SQUARESIZE / sprite->getLocalBounds().height;
    sprite->setScale(scale, scale);
}

void Piece::setPieceCoordinates(float x, float y)
{
    spritePosition = sf::Vector2f(x, y);
    sprite->setPosition(spritePosition);
}
void Piece::drawPiece(sf::RenderWindow *window)
{
    window->draw(*sprite);
}