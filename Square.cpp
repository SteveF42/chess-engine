#include "Square.hpp"
#include"GameEngine.hpp"


sf::Sprite* Square::getSprite(int currentPiece)
{
    int pieceType = Piece::getPieceType(currentPiece);
    sf::Sprite* returnSprite = nullptr;
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