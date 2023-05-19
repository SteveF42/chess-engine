#include "GameEngine.hpp"
#include <iostream>

std::map<std::string, sf::Texture *> GameEngine::textures = {};

void GameEngine::update()
{
    sf::Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window->close();
    }
    window->setSize(sf::Vector2u(1000.f, 1000.f));
    window->clear();

    this->movePiece();

    this->drawBoard();
    this->drawHighLightedSquare();
    this->drawPieces();

    window->display();
}

void GameEngine::drawHighLightedSquare()
{
    if (highLightedSquare == nullptr)
        return;

    int squarePos = highLightedSquare->getSquarePosition();
    int file = squarePos % 8;
    int rank = squarePos / 8;

    sf::RectangleShape rectangle(sf::Vector2(SQUARESIZE, SQUARESIZE));
    auto pos = sf::Vector2((file * SQUARESIZE), rank * SQUARESIZE);
    rectangle.setPosition(pos);
    rectangle.setFillColor(RED);
    window->draw(rectangle);

    if (highLightedSquare->hasNullPiece())
        return;

    std::vector<Move> moves = gameBoard->getPieceMoves(highLightedSquare->getSquarePosition());

    for (auto i : moves)
    {
        int file = i.target % 8;
        int rank = i.target / 8;
        sf::RectangleShape rectangle(sf::Vector2(SQUARESIZE, SQUARESIZE));
        auto pos = sf::Vector2((file * SQUARESIZE), rank * SQUARESIZE);
        rectangle.setPosition(pos);
        rectangle.setFillColor(RED);
        window->draw(rectangle);
    }
    rectangle.setFillColor(ORANGE);
    window->draw(rectangle);
}

void GameEngine::movePiece()
{
    sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
    Square **board = gameBoard->getBoard();
    sf::Vector2u windowSize = window->getSize();
    // this looks disgusting but it breaks the window resolution down in ratios to check the current square being highlighted
    int squarePosition = ((mousePosition.y / (windowSize.y / 8)) * 8) + mousePosition.x / (windowSize.x / 8);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !movingPiece)
    {
        if (squarePosition > 64 || squarePosition < 0)
            return;

        highLightedSquare = board[squarePosition];
        if (highLightedSquare->hasNullPiece())
            return;
        movingPiece = true;
    }
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && movingPiece)
    {
        // display piecemoves
        //  int calculatedSquares[] = clickedPiece->getMoves();
        if (highLightedSquare->hasNullPiece())
            return;
        highLightedSquare->getPiece()->setPiecePosition(mousePosition.x, mousePosition.y);
    }
    else
    {
        movingPiece = false;
    }
}

void GameEngine::drawBoard()
{
    for (int rank = 0; rank < BOARDSIZE; rank++)
    {
        for (int file = 0; file < BOARDSIZE; file++)
        {
            sf::RectangleShape rectangle(sf::Vector2(SQUARESIZE, SQUARESIZE));
            auto pos = sf::Vector2((file * SQUARESIZE), rank * SQUARESIZE);
            rectangle.setPosition(pos);
            rectangle.setFillColor(LIGHTSQUARE);
            if ((file + rank) % 2 == 0)
            {
                rectangle.setFillColor(DARKSQUARE);
            }
            window->draw(rectangle);
        }
    }
}

void GameEngine::drawPieces()
{

    Square **arr = gameBoard->getBoard();
    for (int rank = 0; rank < BOARDSIZE; rank++)
    {
        for (int file = 0; file < BOARDSIZE; file++)
        {
            int position = (rank * 8) + file;
            if (arr[position]->hasNullPiece())
                continue;

            Piece *piece = arr[position]->getPiece();
            piece->drawPiece(window);
        }
    }
}

void GameEngine::loadTextures()
{
    sf::Texture *b_queen = new sf::Texture;
    sf::Texture *b_king = new sf::Texture;
    sf::Texture *b_knight = new sf::Texture;
    sf::Texture *b_bishop = new sf::Texture;
    sf::Texture *b_rook = new sf::Texture;
    sf::Texture *b_pawn = new sf::Texture;
    b_queen->loadFromFile("./ChessPieces/black_queen.png");
    b_king->loadFromFile("./ChessPieces/black_king.png");
    b_knight->loadFromFile("./ChessPieces/black_knight.png");
    b_bishop->loadFromFile("./ChessPieces/black_bishop.png");
    b_rook->loadFromFile("./ChessPieces/black_rook.png");
    b_pawn->loadFromFile("./ChessPieces/black_pawn.png");

    sf::Texture *w_queen = new sf::Texture;
    sf::Texture *w_king = new sf::Texture;
    sf::Texture *w_knight = new sf::Texture;
    sf::Texture *w_bishop = new sf::Texture;
    sf::Texture *w_rook = new sf::Texture;
    sf::Texture *w_pawn = new sf::Texture;
    w_queen->loadFromFile("./ChessPieces/white_queen.png");
    w_king->loadFromFile("./ChessPieces/white_king.png");
    w_knight->loadFromFile("./ChessPieces/white_knight.png");
    w_bishop->loadFromFile("./ChessPieces/white_bishop.png");
    w_rook->loadFromFile("./ChessPieces/white_rook.png");
    w_pawn->loadFromFile("./ChessPieces/white_pawn.png");

    GameEngine::textures["bk"] = b_king;
    GameEngine::textures["bq"] = b_queen;
    GameEngine::textures["bn"] = b_knight;
    GameEngine::textures["bb"] = b_bishop;
    GameEngine::textures["br"] = b_rook;
    GameEngine::textures["bp"] = b_pawn;
    GameEngine::textures["wk"] = w_king;
    GameEngine::textures["wq"] = w_queen;
    GameEngine::textures["wn"] = w_knight;
    GameEngine::textures["wb"] = w_bishop;
    GameEngine::textures["wr"] = w_rook;
    GameEngine::textures["wp"] = w_pawn;
}
