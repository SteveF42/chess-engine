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
    const int BOARDSIZE = 8;
    const float SQUARESIZE = 25.f;
    const sf::Color LIGHTSQUARE = sf::Color(210, 140, 69);
    const sf::Color DARKSQUARE = sf::Color(255, 207, 159);
    const float screenOffsetMultiplyer = 1.5;
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(200, 200), "chess engine!");
    Board *gameBoard;

    static void loadTextures();
    void drawBoard();
    void drawPieces();
    void updatePosition();

public:
    static std::map<std::string, sf::Texture*> textures;

    GameEngine()
    {
        loadTextures();
        gameBoard = new Board();
    }

    bool isActive() { return window->isOpen(); }
    void update()
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
        }
        window->setSize(sf::Vector2u(1000.f, 1000.f));

        window->clear();

        this->drawBoard();
        this->drawPieces();

        window->display();
    }
};

#endif