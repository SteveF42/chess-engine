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
    // sprite = getSprite(pieceType);
    // spritePosition = pos;
    // sprite->setPosition(pos);
    // scale = GameEngine::SQUARESIZE / sprite->getLocalBounds().height;
    // sprite->setScale(scale, scale);
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

sf::Sprite *Piece::getSprite(int currentPiece)
{
    int pieceType = Piece::getPieceType(currentPiece);
    sf::Sprite *returnSprite = nullptr;
    if (Piece::getPieceColor(currentPiece) == Piece::BLACK)
    {
        switch (pieceType)
        {
        case Piece::PAWN:
            returnSprite = new sf::Sprite(*GameEngine::textures["bp"]);
            break;
        case Piece::BISHOP:
            returnSprite = new sf::Sprite(*GameEngine::textures["bb"]);
            break;
        case Piece::KNIGHT:
            returnSprite = new sf::Sprite(*GameEngine::textures["bn"]);
            break;
        case Piece::KING:
            returnSprite = new sf::Sprite(*GameEngine::textures["bk"]);
            break;
        case Piece::QUEEN:
            returnSprite = new sf::Sprite(*GameEngine::textures["bq"]);
            break;
        case Piece::ROOK:
            returnSprite = new sf::Sprite(*GameEngine::textures["br"]);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (pieceType)
        {
        case Piece::PAWN:
            returnSprite = new sf::Sprite(*GameEngine::textures["wp"]);
            break;
        case Piece::BISHOP:
            returnSprite = new sf::Sprite(*GameEngine::textures["wb"]);
            break;
        case Piece::KNIGHT:
            returnSprite = new sf::Sprite(*GameEngine::textures["wn"]);
            break;
        case Piece::KING:
            returnSprite = new sf::Sprite(*GameEngine::textures["wk"]);
            break;
        case Piece::QUEEN:
            returnSprite = new sf::Sprite(*GameEngine::textures["wq"]);
            break;
        case Piece::ROOK:
            returnSprite = new sf::Sprite(*GameEngine::textures["wr"]);
            break;
        default:
            break;
        }
    }
    return returnSprite;
}