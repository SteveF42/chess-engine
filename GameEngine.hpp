#ifndef GameEngine_H
#define GameEngine_H
#include <SFML/Graphics.hpp>
#include"Board.hpp"
#include <map>
#include <stack>
#include <iostream>

class GameEngine
{
private:
    const sf::Color LIGHTSQUARE = sf::Color(210, 140, 69);
    const sf::Color DARKSQUARE = sf::Color(255, 207, 159);
    const float screenOffsetMultiplyer = 1.5;
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(200, 200), "chess engine!");
    Board *gameBoard;

    static void loadTextures();
    void drawBoard();
    void drawPieces();
    void updatePosition();
    void movePiece();
    bool movingPiece = false;
    Piece* clickedPiece = nullptr;

public:
    static const int BOARDSIZE = 8;
    static constexpr float SQUARESIZE = 25.f;
    static std::map<std::string, sf::Texture*> textures;

    GameEngine()
    {
        loadTextures();
        gameBoard = ReadFen::readFenString(ReadFen::startingString);
    }

    bool isActive() { return window->isOpen(); }

    void update();
};

#endif