#ifndef GameEngine_H
#define GameEngine_H
#include <SFML/Graphics.hpp>
#include "Board.hpp"
#include <map>

struct SpriteData
{
    sf::Sprite *sprite;
    int pieceType;
    bool inUse = false;
    Piece *piecePtr;
    SpriteData(sf::Sprite *other, int type)
    {
        sprite = other;
        pieceType = type;
    }
    SpriteData() {}
};

class GameEngine
{
private:
    const sf::Color LIGHTSQUARE = sf::Color(220, 140, 69);
    const sf::Color DARKSQUARE = sf::Color(255, 207, 159);
    const sf::Color RED = sf::Color(255, 0, 0, 100);
    const sf::Color ORANGE = sf::Color(255, 165, 0, 100);
    const float screenOffsetMultiplyer = 1.5;
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(200, 200), "chess engine!");
    Board *gameBoard;

    // SpriteData pieces[2][6];
    SpriteData *allSprites[32];
    std::map<int, sf::Sprite> pieceSprites;

    static void loadTextures();
    void loadSprites();
    void selectPieceOrSquare();
    void placePiece(std::string s);
    void drawBoard();
    void drawPieces();
    void movePiece();
    void drawHighLightedSquare();
    void drawPromotionPieces(int file, int color);
    Square *highLightedSquare = nullptr;

public:
    static const int BOARDSIZE = 8;
    static constexpr float SQUARESIZE = 25.f;
    static std::map<std::string, sf::Texture *> textures;

    GameEngine()
    {
        gameBoard = ReadFen::readFenString(ReadFen::startingString);
        loadTextures();
        loadSprites();
        gameBoard->generateMovesInCurrentPosition();
    }

    bool isActive() { return window->isOpen(); }

    void update();
};

#endif