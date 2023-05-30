#ifndef GameEngine_H
#define GameEngine_H
#include <SFML/Graphics.hpp>
#include "Board.hpp"

struct SpriteData
{
    sf::Sprite *sprite;
    int pieceType;
    SpriteData(sf::Sprite *other, int type)
    {
        sprite = other;
        pieceType = type;
    }
    SpriteData(){}
};

class GameEngine
{
private:
    const sf::Color LIGHTSQUARE = sf::Color(210, 140, 69);
    const sf::Color DARKSQUARE = sf::Color(255, 207, 159);
    const sf::Color RED = sf::Color(255, 0, 0, 100);
    const sf::Color ORANGE = sf::Color(255, 165, 0, 100);
    const float screenOffsetMultiplyer = 1.5;
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(200, 200), "chess engine!");
    Board *gameBoard;


    // SpriteData pieces[2][6];
    SpriteData *allSprites[32];
    SpriteData *activeSprites[32];

    static void loadTextures();
    void loadSprites();
    void assignSprites();
    void selectPieceOrSquare();
    void placePiece(std::string s);
    void drawBoard();
    void drawPieces();
    void updatePosition();
    void movePiece();
    void drawHighLightedSquare();
    Square *highLightedSquare = nullptr;

public:
    static const int BOARDSIZE = 8;
    static constexpr float SQUARESIZE = 25.f;
    static std::map<std::string, sf::Texture *> textures;

    GameEngine()
    {
        loadTextures();
        loadSprites();
        gameBoard = ReadFen::readFenString(ReadFen::startingString);
        gameBoard->generateMovesInCurrentPosition();
        assignSprites();
    }

    bool isActive() { return window->isOpen(); }

    void update();
};

#endif