#include "GameEngine.hpp"
#include <iostream>

std::map<std::string, sf::Texture *> GameEngine::textures = {};
// this is very very bad, I really shouldn't be doing it this way
// I'm too lazy to change it, it works.
bool flag = false;
bool placed = false;

void GameEngine::update()
{
    sf::Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window->close();

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        {
            placed = false;
            if (highLightedSquare != nullptr && !highLightedSquare->hasNullPiece())
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
            if (highLightedSquare != nullptr && !highLightedSquare->hasNullPiece())
            {
                placePiece("drop");
            }
        }
        // else if (event.type == sf::Event::KeyPressed && event.KeyPressed == sf::Keyboard::A)
        // {
        //     this->gameBoard->unmakeMove();
        // }
    }

    window->setSize(sf::Vector2u(1000.f, 1000.f));
    window->clear();

    this->movePiece();
    this->drawBoard();
    this->drawHighLightedSquare();
    this->drawPieces();

    window->display();
}

void GameEngine::placePiece(std::string s)
{
    sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
    sf::Vector2u windowSize = window->getSize();
    // this looks disgusting but it breaks the window resolution down in ratios to check the current square being highlighted
    int squarePosition = ((mousePosition.y / (windowSize.y / 8)) * 8) + mousePosition.x / (windowSize.x / 8);
    // validate the move

    Piece *piece = highLightedSquare->getPiece();
    bool validMove = gameBoard->validateMove(highLightedSquare->getSquarePosition(), squarePosition);
    if (validMove)
    {
        int file = squarePosition % 8;
        int rank = squarePosition / 8;
        piece->setPieceCoordinates(file * GameEngine::SQUARESIZE, rank * GameEngine::SQUARESIZE);
        // pawn promotion
        if (piece->getPieceType() == Piece::PAWN)
        {
            if (piece->getPieceColor() == Piece::WHITE && rank == 0)
            {
                // pawn promotion idk
                drawPromotionPieces(squarePosition, Piece::WHITE);
            }
            else if (piece->getPieceColor() == Piece::BLACK && rank == 7)
            {
                // pawn promotion idk
                drawPromotionPieces(squarePosition, Piece::BLACK);
            }
        }
        highLightedSquare = nullptr;
        flag = false;
        placed = true;
    }
    else
    {
        int homeSquare = highLightedSquare->getSquarePosition();
        int file = homeSquare % 8;
        int rank = homeSquare / 8;
        piece->setPieceCoordinates(file * SQUARESIZE, rank * SQUARESIZE);
        if (s == "drop" && flag && squarePosition == highLightedSquare->getSquarePosition())
        {
            highLightedSquare = nullptr;
            flag = false;
        }
        else if (s == "drop")
        {
            flag = true;
        }
    }
}

void GameEngine::selectPieceOrSquare()
{

    sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
    Square **board = gameBoard->getBoard();
    sf::Vector2u windowSize = window->getSize();
    // this looks disgusting but it breaks the window resolution down in ratios to check the current square being highlighted
    int squarePosition = ((mousePosition.y / (windowSize.y / 8)) * 8) + mousePosition.x / (windowSize.x / 8);
    if (squarePosition >= 64 || squarePosition < 0)
        return;

    if (board[squarePosition]->hasNullPiece())
    {
        highLightedSquare = nullptr;
        flag = false;
        return;
    }

    if (highLightedSquare != nullptr && highLightedSquare->getSquarePosition() != squarePosition)
        flag = false;

    Piece *piece = board[squarePosition]->getPiece();

    highLightedSquare = nullptr;
    if (piece->getPieceColor() == Piece::BLACK && gameBoard->getWhiteToMove())
        return;
    if (piece->getPieceColor() == Piece::WHITE && !gameBoard->getWhiteToMove())
        return;

    highLightedSquare = board[squarePosition];
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
    if (highLightedSquare == nullptr || highLightedSquare->hasNullPiece())
        return;

    sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
    sf::Vector2u windowSize = window->getSize();
    // this looks disgusting but it breaks the window resolution down in ratios to check the current square being highlighted
    int squarePosition = ((mousePosition.y / (windowSize.y / 8)) * 8) + mousePosition.x / (windowSize.x / 8);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        Piece *piece = highLightedSquare->getPiece();
        // this also looks terrible but its just a ratio. for some reason the pieces like to move 5 to 1 ratio so that's why divide by 5, 12 is the center of each square since
        highLightedSquare->getPiece()->setPieceCoordinates(mousePosition.x / 5 - (SQUARESIZE / 2), mousePosition.y / 5 - (SQUARESIZE / 2));
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
                sf::Vector2 mousePosition = sf::Mouse::getPosition(*window);
                sf::Vector2u windowSize = window->getSize();
                int clickedSquare = ((mousePosition.y / (windowSize.y / 8)) * 8) + mousePosition.x / (windowSize.x / 8);
                int pieceChoice = std::abs(squareIndx - clickedSquare);
                if (pieceChoice == 0)
                {
                    // gameBoard->promotePawn(squareIndx, Piece::QUEEN | color, sprites[0]);
                    delete rook;
                    delete bishop;
                    delete horsie;
                    done = true;
                }
                else if (pieceChoice == 8)
                {
                    std::cout << 1 << '\n';
                    delete queen;
                    delete bishop;
                    delete horsie;
                    done = true;
                }
                else if (pieceChoice == 16)
                {
                    std::cout << 2 << '\n';
                    delete queen;
                    delete rook;
                    delete horsie;
                    done = true;
                }
                else if (pieceChoice == 24)
                {
                    std::cout << 3 << '\n';
                    delete queen;
                    delete rook;
                    delete bishop;
                    done = true;
                }
            }
        }
        if (done)
            break;

        window->setSize(sf::Vector2u(1000.f, 1000.f));
        window->clear();
        this->drawBoard();
        this->drawPieces();

        // draws the pice choices
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
void GameEngine::assignSprites()
{
    Square **board = gameBoard->getBoard();

    for (int i = 0; i < 64; i++)
    {
        int file = i % 8;
        int rank = i / 8;
        Square *square = board[i];
        if (square->hasNullPiece())
            continue;

        Piece *piece = square->getPiece();
        for (int j = 0; j < 32; j++)
        {
            if (allSprites[j]->inUse)
                continue;

            SpriteData *data = allSprites[j];
            int color = Piece::getPieceColor(data->pieceType);
            int type = Piece::getPieceType(data->pieceType);

            if (color == piece->getPieceColor() && type == piece->getPieceType())
            {
                sf::Vector2f pos(file * SQUARESIZE, rank * SQUARESIZE);
                piece->setPieceSprite(data->sprite, pos);
                allSprites[j]->inUse = true;
                allSprites[j]->piecePtr = piece;
                break;
            }
        }
    }
}

void GameEngine::loadSprites()
{
    // white piece sprites
    for (int i = 0; i < 8; i++)
    {
        sf::Sprite *pawnSprite = new sf::Sprite(*GameEngine::textures["wp"]);
        allSprites[i] = new SpriteData(pawnSprite, Piece::PAWN | Piece::WHITE);
    }
    allSprites[8] = new SpriteData(new sf::Sprite(*GameEngine::textures["wr"]), Piece::ROOK | Piece::WHITE);
    allSprites[9] = new SpriteData(new sf::Sprite(*GameEngine::textures["wr"]), Piece::ROOK | Piece::WHITE);
    allSprites[10] = new SpriteData(new sf::Sprite(*GameEngine::textures["wn"]), Piece::KNIGHT | Piece::WHITE);
    allSprites[11] = new SpriteData(new sf::Sprite(*GameEngine::textures["wn"]), Piece::KNIGHT | Piece::WHITE);
    allSprites[12] = new SpriteData(new sf::Sprite(*GameEngine::textures["wb"]), Piece::BISHOP | Piece::WHITE);
    allSprites[13] = new SpriteData(new sf::Sprite(*GameEngine::textures["wb"]), Piece::BISHOP | Piece::WHITE);
    allSprites[14] = new SpriteData(new sf::Sprite(*GameEngine::textures["wk"]), Piece::KING | Piece::WHITE);
    allSprites[15] = new SpriteData(new sf::Sprite(*GameEngine::textures["wq"]), Piece::QUEEN | Piece::WHITE);

    // black piece sprites
    for (int i = 16; i < 24; i++)
    {
        sf::Sprite *pawnSprite = new sf::Sprite(*GameEngine::textures["bp"]);
        allSprites[i] = new SpriteData(pawnSprite, Piece::PAWN | Piece::BLACK);
    }
    allSprites[24] = new SpriteData(new sf::Sprite(*GameEngine::textures["br"]), Piece::ROOK | Piece::BLACK);
    allSprites[25] = new SpriteData(new sf::Sprite(*GameEngine::textures["br"]), Piece::ROOK | Piece::BLACK);
    allSprites[26] = new SpriteData(new sf::Sprite(*GameEngine::textures["bn"]), Piece::KNIGHT | Piece::BLACK);
    allSprites[27] = new SpriteData(new sf::Sprite(*GameEngine::textures["bn"]), Piece::KNIGHT | Piece::BLACK);
    allSprites[28] = new SpriteData(new sf::Sprite(*GameEngine::textures["bb"]), Piece::BISHOP | Piece::BLACK);
    allSprites[29] = new SpriteData(new sf::Sprite(*GameEngine::textures["bb"]), Piece::BISHOP | Piece::BLACK);
    allSprites[30] = new SpriteData(new sf::Sprite(*GameEngine::textures["bk"]), Piece::KING | Piece::BLACK);
    allSprites[31] = new SpriteData(new sf::Sprite(*GameEngine::textures["bq"]), Piece::QUEEN | Piece::BLACK);
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
