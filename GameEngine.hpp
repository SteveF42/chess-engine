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
    void selectPieceOrSquare();
    void placePiece();
    void drawBoard();
    void drawPieces();
    void updatePosition();
    void movePiece();
    void drawHighLightedSquare();
    Square* highLightedSquare = nullptr;

public:
    static const int BOARDSIZE = 8;
    static constexpr float SQUARESIZE = 25.f;
    static std::map<std::string, sf::Texture*> textures;

    GameEngine()
    {
        loadTextures();
        gameBoard = ReadFen::readFenString("r1k5/1p1n4/p1p2p2/P1Pp4/3p3r/4q3/3KB3/3R1R2 w - - 0 1");
        gameBoard->generateMovesInCurrentPosition();
    }

    bool isActive() { return window->isOpen(); }

    void update();
};

#endif