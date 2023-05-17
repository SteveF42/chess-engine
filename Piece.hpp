#ifndef Piece_H
#define Piece_H
#include <SFML/Graphics.hpp>


class Piece
{

private:
    static const int typeMask = 0b00111;
    static const int blackMask = 0b10000;
    static const int whiteMask = 0b01000;
    static const int colourMask = whiteMask | blackMask;
    sf::Sprite *getSprite(int currentSprite);
    sf::Sprite *sprite = nullptr;
    sf::Vector2f spritePosition;

    int pieceType;

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

    int getPiece() { return getPieceType(pieceType); }
    int getColor() { return getPieceColor(pieceType); }

    Piece(int pieceType, sf::Vector2f pos);

    void setPiecePosition(float x, float y);
    void drawPiece(sf::RenderWindow *window);
};

#endif