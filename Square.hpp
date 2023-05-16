#ifndef Square_H
#define Square_H
#include "Piece.hpp"


class Square
{
private:
    int pieceType;
    sf::Sprite *sprite;
    int squarePosition;

    sf::Sprite* getSprite(int currentSprite);

public:
    Square(int piece, int position)
    {
        squarePosition = position;
        piece = piece;
    }
    int getSquarePosition() { return squarePosition; }
    int getPieceType() { return pieceType; }
    sf::Sprite* getSprite(){return sprite;}
    void setSquarePosition(int pos) { squarePosition = pos; }
    void setPieceType(int piece)
    {
        pieceType = piece;
        sprite = getSprite(pieceType);   
    }
    void swapPiece(Square& other);

    bool operator==(int& otherPiece){
        return pieceType == otherPiece;
    }
};

#endif