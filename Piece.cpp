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
    if(this->sprite != nullptr){
        previousSprite = sprite;
    }
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

void Piece::setPiecePosition(int positionOnBoard, bool overridePosition /*=false*/)
{
    currentPosition = positionOnBoard;
    int rank = currentPosition / 8;
    int file = currentPosition % 8;
    if (overridePosition)
    {
        this->setPieceCoordinates(GameEngine::SQUARESIZE * file, GameEngine::SQUARESIZE * rank);
    }
}