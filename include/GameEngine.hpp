#ifndef GameEngine_H
#define GameEngine_H
#include <SFML/Graphics.hpp>
#include "ReadFen.hpp"
#include "Board.hpp"
#include "Move.hpp"
#include "AI.hpp"
#include <thread>
#include <map>

class GameEngine
{
private:
    const sf::Color LIGHTSQUARE = sf::Color(220, 140, 69);
    const sf::Color DARKSQUARE = sf::Color(255, 207, 159);
    const sf::Color RED = sf::Color(255, 0, 0, 100);
    const sf::Color ORANGE = sf::Color(255, 165, 0, 100);
    const sf::Color GOLD = sf::Color(214, 108, 102, 200);
    const float screenOffsetMultiplyer = 1.5;
    const unsigned int TARGETSIZE = 1000;
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(TARGETSIZE, TARGETSIZE), "chess engine!");
    Board *gameBoard;
    bool pauseMoves = true;
    bool playAsWhite;
    int flippedView;
    bool search = false;

    std::map<int, sf::Sprite> pieceSprites;
    Move lastMove = Move(-1, -1, -1);
    std::thread* moveSearch = nullptr;
    int pieces[64];
    std::vector<Move> movesCopy;
    
    std::vector<Move> getPieceMoves(int idx);
    void copyPieces();
    int getSquarePosition();
    static void loadTextures();
    void loadSprites();
    void selectPieceOrSquare();
    void placePiece(std::string s);
    void drawBoard();
    void drawPieces();
    void movePiece();
    void drawHighLightedSquare();
    void drawLastMove();
    void drawPromotionPieces(int file, int color);
    void aiMove();
    void events();
    Square *highLightedSquare = nullptr;
    AI *aiPlayer;
public:
    GameEngine(std::string fenString, bool playAsWhite = true);
    static const int BOARDSIZE = 8;
    const float SQUARESIZE = (int)(window->getSize().x / BOARDSIZE);
    static std::map<std::string, sf::Texture *> textures;
    bool isActive() { return window->isOpen(); }

    void update();
};

#endif