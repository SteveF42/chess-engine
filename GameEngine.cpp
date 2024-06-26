#include <iostream>
#include "GameEngine.hpp"
#include "AI.hpp"

std::map<std::string, sf::Texture *> GameEngine::textures = {};
// this is very very bad, I really shouldn't be doing it this way
// I'm too lazy to change it, it works.
bool flag = false;
bool placed = false;

GameEngine::GameEngine(std::string fenString, bool playAsWhite /* = false*/)
{
    this->playAsWhite = playAsWhite;
    gameBoard = ReadFen::readFenString(fenString);
    aiPlayer = new AI(gameBoard);
    loadTextures();
    loadSprites();
    flippedView = playAsWhite ? 0 : 63;
    (void)gameBoard->moveGeneration.generateMoves(gameBoard);
    copyPieces();
}

void GameEngine::update()
{

    // if (gameBoard->moveGeneration.getGameOver())
    // {
    //     window->setActive(false);
    //     std::cout<<"Game Over " << (gameBoard->getWhiteToMove() ? "Black Wins" : "White Wins") << '\n';
    //     return;
    // }
    this->events();
    window->setSize(sf::Vector2u(window->getSize().x, window->getSize().x));
    window->clear();

    this->drawBoard();
    this->drawLastMove();
    this->drawHighLightedSquare();
    this->drawPieces();
    this->movePiece();
    this->aiMove();
    window->display();
}

void GameEngine::events()
{
    sf::Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window->close();

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        {
            placed = false;
            if (highLightedSquare != nullptr && pieces[highLightedSquare->getSquarePosition()] != -1)
            {
                placePiece("");
            }
            if (!placed)
            {
                placed = false;
                selectPieceOrSquare();
            }
            // sets a square to be highlighted alongside a piece moves if that's the case
        }
        else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        {
            if (highLightedSquare != nullptr && pieces[highLightedSquare->getSquarePosition()] != -1)
            {
                placePiece("drop");
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && event.type == sf::Event::KeyPressed && !search)
        {
            // undo move
            lastMove = this->gameBoard->unmakeMove();
            gameBoard->moveGeneration.generateMoves(gameBoard);
            copyPieces();
            pauseMoves = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B) && event.type == sf::Event::KeyPressed)
        {
            // switch player sides and board view
            playAsWhite = !playAsWhite;
            flippedView = playAsWhite ? 0 : 63;
            pauseMoves = true;
            std::cout << (playAsWhite ? "Playing as white" : "Playing as black") << '\n';
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::V) && event.type == sf::Event::KeyPressed)
        {
            // switch board view
            flippedView = std::abs(63 - flippedView);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && event.type == sf::Event::KeyPressed)
        {
            pauseMoves = false;
        }
    }
}

void GameEngine::aiMove()
{
    if (!pauseMoves)
    {
        if (moveSearch == nullptr && !search)
        {
            // dude I don't even know if this is safe
            // I just want the UI to not hang when the AI does its little search thing
            this->moveSearch = new std::thread([this]
                                               { this->aiPlayer->generateBestMove(); });
            search = true;
        }
        else if (aiPlayer->getTimeout() && search)
        {
            this->moveSearch->join();
            Move &bestMove = aiPlayer->bestMove;
            if (bestMove.start != bestMove.target)
            {
                gameBoard->makeMove(bestMove);
                gameBoard->moveGeneration.generateMoves(gameBoard);
                lastMove = bestMove;
                copyPieces();
            }
            delete this->moveSearch;
            this->moveSearch = nullptr;
            search = false;
            pauseMoves = true;
        }
    }
}

void GameEngine::placePiece(std::string s)
{
    // int squarePosition = ((mousePosition.y / (windowSize.y))  8) + mousePosition.x % (windowSize.x);
    int squarePosition = getSquarePosition();

    int file = squarePosition % 8;
    int rank = squarePosition / 8;
    // validate the move

    int pieceType = pieces[highLightedSquare->getSquarePosition()];
    int startPos = highLightedSquare->getSquarePosition();
    // pawn promotion

    bool validMove = !search ? gameBoard->validateMove(startPos, squarePosition) : false;
    if (validMove)
    {
        sf::Sprite &sprite = pieceSprites[pieceType];
        sprite.setPosition(file * GameEngine::SQUARESIZE, rank * GameEngine::SQUARESIZE);
        if (Piece::getPieceType(pieceType) == Piece::PAWN)
        {
            if (Piece::getPieceColor(pieceType) == Piece::WHITE && rank == 0)
            {
                // pawn promotion idk
                drawPromotionPieces(squarePosition, Piece::WHITE);
            }
            else if (Piece::getPieceColor(pieceType) == Piece::BLACK && rank == 7)
            {
                // pawn promotion idk
                drawPromotionPieces(squarePosition, Piece::BLACK);
            }
        }
        this->lastMove.start = startPos;
        this->lastMove.target = squarePosition;
        highLightedSquare = nullptr;
        flag = false;
        placed = true;
        pauseMoves = false;
        // I have to break out the generate moves function from the board class to take into account the potential pawn promotion
        gameBoard->moveGeneration.generateMoves(gameBoard);
        copyPieces();
    }
    else
    {
        int homeSquare = highLightedSquare->getSquarePosition();
        int homeFile = homeSquare % 8;
        int homeRank = homeSquare / 8;
        sf::Sprite &sprite = pieceSprites[pieceType];
        sprite.setPosition(homeFile * SQUARESIZE, homeRank * SQUARESIZE);
        if (s == "drop" && flag && squarePosition == highLightedSquare->getSquarePosition())
        {
            highLightedSquare = nullptr;
            flag = false;
        }
        else if (s == "drop")
        {
            flag = true;
            placed = true;
        }
    }
}

void GameEngine::drawLastMove()
{
    if (lastMove.start == lastMove.target)
        return;

    // draws start square
    int boardView = std::abs(lastMove.start - flippedView);
    int file = boardView % 8;
    int rank = boardView / 8;
    sf::RectangleShape rectangle(sf::Vector2(SQUARESIZE, SQUARESIZE));
    rectangle.setPosition(sf::Vector2((file * SQUARESIZE), rank * SQUARESIZE));
    rectangle.setFillColor(GOLD);
    window->draw(rectangle);

    // draws end square
    boardView = std::abs(lastMove.target - flippedView);
    file = boardView % 8;
    rank = boardView / 8;
    rectangle.setPosition(sf::Vector2((file * SQUARESIZE), rank * SQUARESIZE));
    rectangle.setFillColor(GOLD);
    window->draw(rectangle);
}

void GameEngine::selectPieceOrSquare()
{

    sf::Vector2u windowSize = window->getSize();
    // this looks disgusting but it breaks the window resolution down in ratios to check the current square being highlighted
    int squarePosition = getSquarePosition();
    if (squarePosition >= 64 || squarePosition < 0)
        return;

    if (pieces[squarePosition] == -1)
    {
        highLightedSquare = nullptr;
        flag = false;
        return;
    }

    if (highLightedSquare != nullptr && highLightedSquare->getSquarePosition() != squarePosition)
        flag = false;

    highLightedSquare = gameBoard->getBoard()[squarePosition];
}

void GameEngine::drawHighLightedSquare()
{
    if (highLightedSquare == nullptr)
        return;

    int squarePos = highLightedSquare->getSquarePosition();
    squarePos = std::abs(flippedView - squarePos);
    int file = squarePos % 8;
    int rank = squarePos / 8;

    sf::RectangleShape rectangle(sf::Vector2(SQUARESIZE, SQUARESIZE));
    auto pos = sf::Vector2((file * SQUARESIZE), rank * SQUARESIZE);
    rectangle.setPosition(pos);
    rectangle.setFillColor(RED);
    window->draw(rectangle);

    if (pieces[highLightedSquare->getSquarePosition()] == -1)
        return;

    std::vector<Move> moves = getPieceMoves(highLightedSquare->getSquarePosition());

    for (const auto &i : moves)
    {
        int targetSquare = std::abs(flippedView - i.target);
        int file = targetSquare % 8;
        int rank = targetSquare / 8;
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
    if (highLightedSquare == nullptr || pieces[highLightedSquare->getSquarePosition()] == -1)
        return;

    sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
    sf::Vector2u windowSize = window->getSize();
    auto pos = window->mapPixelToCoords(mousePosition);

    // this looks disgusting but it breaks the window resolution down in ratios to check the current square being highlighted
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        int piece = pieces[highLightedSquare->getSquarePosition()];
        if (piece == -1)
        {
            return;
        }
        sf::Sprite &clickedSprite = pieceSprites[piece];
        int offset = clickedSprite.getGlobalBounds().width / 2;
        clickedSprite.setPosition(pos.x - offset, pos.y - offset);
        window->draw(clickedSprite);
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
            if (file == 0)
            {
                // draw letters
            }
            if (rank == 0)
            {
                // draw numbers
            }
            window->draw(rectangle);
        }
    }
}

void GameEngine::drawPieces()
{

    for (int rank = 0; rank < BOARDSIZE; rank++)
    {
        for (int file = 0; file < BOARDSIZE; file++)
        {
            int position = (rank * 8) + file;
            position = std::abs(position - flippedView);

            if (highLightedSquare != nullptr && highLightedSquare->getSquarePosition() == position && placed == false)
                continue;
            if (pieces[position] == -1)
                continue;

            int piece = pieces[position];
            sf::Sprite &sprite = pieceSprites[piece];
            float scale = GameEngine::SQUARESIZE / sprite.getLocalBounds().height;
            sprite.setScale(scale, scale);
            sprite.setPosition(file * SQUARESIZE, rank * SQUARESIZE);

            window->draw(sprite);
        }
    }
}

// overides the main game loop to allow for pawn promotion pieces to be drawn
void GameEngine::drawPromotionPieces(int squareIndx, int color)
{

    int offset = color == Piece::WHITE ? 1 : -1;
    sf::Texture *horsieTexture = color == Piece::WHITE ? textures["wn"] : textures["bn"];
    sf::Texture *queenTexture = color == Piece::WHITE ? textures["wq"] : textures["bq"];
    sf::Texture *rookTexture = color == Piece::WHITE ? textures["wr"] : textures["br"];
    sf::Texture *bishopTexture = color == Piece::WHITE ? textures["wb"] : textures["bb"];

    auto queen = new sf::Sprite(*queenTexture);
    auto rook = new sf::Sprite(*rookTexture);
    auto bishop = new sf::Sprite(*bishopTexture);
    auto horsie = new sf::Sprite(*horsieTexture);

    sf::Sprite *sprites[] = {queen, rook, bishop, horsie};

    int chosenPiece = -1;
    sf::Event event;

    sf::RectangleShape rectangle(sf::Vector2(SQUARESIZE, SQUARESIZE));
    int x = (squareIndx % 8) * GameEngine::SQUARESIZE;
    int y = (squareIndx / 8) * GameEngine::SQUARESIZE;
    auto pos = sf::Vector2((float)x, (float)y);

    // this looks disgusting but it breaks the window resolution down in ratios to check the current square being highlighted
    bool done = false;
    while (true)
    {
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window->close();
                throw;
            }

            // main logic for choosing an upgraded piece
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                // this stuff can probably be broken out into a function when I get to implmenting the AI
                sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
                sf::Vector2u windowSize = window->getSize();
                int clickedSquare = ((mousePosition.y / (windowSize.y / 8)) * 8) + mousePosition.x / (windowSize.x / 8);
                int pieceChoice = std::abs(squareIndx - clickedSquare);
                Piece *piece = gameBoard->getBoard()[squareIndx]->getPiece();

                if (pieceChoice == 0)
                {
                    gameBoard->promotePiece(piece, Piece::QUEEN, squareIndx);
                    done = true;
                }
                else if (pieceChoice == 8)
                {
                    gameBoard->promotePiece(piece, Piece::ROOK, squareIndx);
                    done = true;
                }
                else if (pieceChoice == 16)
                {
                    gameBoard->promotePiece(piece, Piece::BISHOP, squareIndx);
                    done = true;
                }
                else if (pieceChoice == 24)
                {
                    gameBoard->promotePiece(piece, Piece::KNIGHT, squareIndx);
                    done = true;
                }
            }
        }
        if (done)
        {
            delete queen;
            delete rook;
            delete bishop;
            delete horsie;
            break;
        }

        window->setSize(sf::Vector2u(1000.f, 1000.f));
        window->clear();
        this->drawBoard();
        this->drawPieces();

        // draw over the origin square
        rectangle.setPosition(pos);
        rectangle.setFillColor(LIGHTSQUARE);
        if ((x + y) % 2 == 0)
        {
            rectangle.setFillColor(DARKSQUARE);
        }
        window->draw(rectangle);

        // draws the piece choices
        for (int i = 0; i < 4; i++)
        {
            int squareToDrawOn = squareIndx + (i * 8 * offset);
            int rank = squareToDrawOn / 8;
            int file = squareToDrawOn % 8;
            float scale = GameEngine::SQUARESIZE / sprites[i]->getLocalBounds().height;
            sprites[i]->setPosition(file * GameEngine::SQUARESIZE, rank * GameEngine::SQUARESIZE);
            sprites[i]->setScale(scale, scale);

            window->draw(*sprites[i]);
        }

        window->display();
    }
}

// assigns sprite pointers to each board in a given position, this way each board isn't instantuating hundreds of new sprites for each position

int GameEngine::getSquarePosition()
{
    sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
    sf::Vector2u windowSize = window->getSize();
    // this looks disgusting but it breaks the window resolution down in ratios to check the current square being highlighted
    int x = mousePosition.x / (windowSize.x / BOARDSIZE);
    int y = mousePosition.y / (windowSize.x / BOARDSIZE);
    int squarePosition = (y * 8) + (x);
    // int squarePosition = ((mousePosition.y / (windowSize.y / 8)) * 8) + mousePosition.x / (windowSize.x / 8);

    squarePosition = std::abs(flippedView - squarePosition);
    return squarePosition;
}

void GameEngine::loadSprites()
{

    sf::Sprite w_pawn = sf::Sprite(*GameEngine::textures["wp"]);
    sf::Sprite w_rook = sf::Sprite(*GameEngine::textures["wr"]);
    sf::Sprite w_knight = sf::Sprite(*GameEngine::textures["wn"]);
    sf::Sprite w_bishop = sf::Sprite(*GameEngine::textures["wb"]);
    sf::Sprite w_queen = sf::Sprite(*GameEngine::textures["wq"]);
    sf::Sprite w_king = sf::Sprite(*GameEngine::textures["wk"]);

    pieceSprites[Piece::PAWN | Piece::WHITE] = w_pawn;
    pieceSprites[Piece::ROOK | Piece::WHITE] = w_rook;
    pieceSprites[Piece::KNIGHT | Piece::WHITE] = w_knight;
    pieceSprites[Piece::BISHOP | Piece::WHITE] = w_bishop;
    pieceSprites[Piece::QUEEN | Piece::WHITE] = w_queen;
    pieceSprites[Piece::KING | Piece::WHITE] = w_king;

    sf::Sprite b_pawn = sf::Sprite(*GameEngine::textures["bp"]);
    sf::Sprite b_rook = sf::Sprite(*GameEngine::textures["br"]);
    sf::Sprite b_knight = sf::Sprite(*GameEngine::textures["bn"]);
    sf::Sprite b_bishop = sf::Sprite(*GameEngine::textures["bb"]);
    sf::Sprite b_queen = sf::Sprite(*GameEngine::textures["bq"]);
    sf::Sprite b_king = sf::Sprite(*GameEngine::textures["bk"]);

    pieceSprites[Piece::PAWN | Piece::BLACK] = b_pawn;
    pieceSprites[Piece::ROOK | Piece::BLACK] = b_rook;
    pieceSprites[Piece::KNIGHT | Piece::BLACK] = b_knight;
    pieceSprites[Piece::BISHOP | Piece::BLACK] = b_bishop;
    pieceSprites[Piece::QUEEN | Piece::BLACK] = b_queen;
    pieceSprites[Piece::KING | Piece::BLACK] = b_king;
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

void GameEngine::copyPieces()
{
    for (int i = 0; i < 64; i++)
    {
        Piece *piece = gameBoard->getBoard()[i]->getPiece();
        if (piece != nullptr)
        {
            pieces[i] = piece->getPieceTypeRaw();
        }
        else
        {
            pieces[i] = -1;
        }
    }
    movesCopy = gameBoard->moveGeneration.getMoves();
}

std::vector<Move> GameEngine::getPieceMoves(int idx)
{
    std::vector<Move> pieceMoves;

    for (auto &move : movesCopy)
    {
        if (move.start == idx)
        {
            pieceMoves.push_back(move);
        }
    }
    return pieceMoves;
}