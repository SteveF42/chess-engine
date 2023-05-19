#ifndef GameEngine_H
#define GameEngine_H
#include <SFML/Graphics.hpp>
#include"Board.hpp"

class GameEngine
{
private:
    const sf::Color LIGHTSQUARE = sf::Color(210, 140, 69);
    const sf::Color DARKSQUARE = sf::Color(255, 207, 159);
    const sf::Color RED = sf::Color(255,0,0,100);
    const sf::Color ORANGE = sf::Color(255,165,0,100);
    const float screenOffsetMultiplyer = 1.5;
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(200, 200), "chess engine!");
    Board *gameBoard;

    static void loadTextures();
    void drawBoard();
    void drawPieces();
    void updatePosition();
    void movePiece();
    void drawHighLightedSquare();
    bool movingPiece = false;
    Square* highLightedSquare = nullptr;

public:
    static const int BOARDSIZE = 8;
    static constexpr float SQUARESIZE = 25.f;
    static std::map<std::string, sf::Texture*> textures;

    GameEngine()
    {
        loadTextures();
        gameBoard = ReadFen::readFenString("r1b1k1nr/p2p1pNp/n2B4/1p1NP2P/6P1/3P1Q2/P1P1K3/q5b1 b - - 0 1");
        gameBoard->generateMovesInCurrentPosition();
    }

    bool isActive() { return window->isOpen(); }

    void update();
};

#endif