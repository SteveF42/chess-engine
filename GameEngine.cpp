#include "GameEngine.hpp"

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
    int *arr = gameBoard->getBoard();
    for (int rank = 0; rank < BOARDSIZE; rank++)
    {
        for (int file = 0; file < BOARDSIZE; file++)
        {
            int position = (rank * 8) + file;
            if (arr[position] == 0)
                continue;

            sf::Sprite sprite = sprites[position];
            sprite.setScale(sf::Vector2f(0.19, 0.19));
            sf::Vector2f pos(file * SQUARESIZE, rank * SQUARESIZE);
            sprite.setPosition(pos);
            window->draw(sprite);
        }
    }
}

void GameEngine::updatePosition()
{
    int *arr = gameBoard->getBoard();
    for (int i = 0; i < 64; i++)
    {
        int currentPiece = arr[i];
        if (currentPiece == 0)
            continue;

        int pieceType = Piece::getPieceType(currentPiece);
        if (Piece::getPieceColor(currentPiece) == Piece::BLACK)
        {
            switch (pieceType)
            {
            case Piece::PAWN:
                sprites[i] = sf::Sprite(*textures["bp"]);
                break;
            case Piece::BISHOP:
                sprites[i] = sf::Sprite(*textures["bb"]);
                break;
            case Piece::KNIGHT:
                sprites[i] = sf::Sprite(*textures["bn"]);
                break;
            case Piece::KING:
                sprites[i] = sf::Sprite(*textures["bk"]);
                break;
            case Piece::QUEEN:
                sprites[i] = sf::Sprite(*textures["bq"]);
                break;
            case Piece::ROOK:
                sprites[i] = sf::Sprite(*textures["br"]);
                break;
            default:
                break;
            }
        }
        else
        {
            switch (pieceType)
            {
            case Piece::PAWN:
                sprites[i] = sf::Sprite(*textures["wp"]);
                break;
            case Piece::BISHOP:
                sprites[i] = sf::Sprite(*textures["wb"]);
                break;
            case Piece::KNIGHT:
                sprites[i] = sf::Sprite(*textures["wn"]);
                break;
            case Piece::KING:
                sprites[i] = sf::Sprite(*textures["wk"]);
                break;
            case Piece::QUEEN:
                sprites[i] = sf::Sprite(*textures["wq"]);
                break;
            case Piece::ROOK:
                sprites[i] = sf::Sprite(*textures["wr"]);
                break;
            default:
                break;
            }
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

    textures["bk"] = b_king;
    textures["bq"] = b_queen;
    textures["bn"] = b_knight;
    textures["bb"] = b_bishop;
    textures["br"] = b_rook;
    textures["bp"] = b_pawn;

    textures["wk"] = w_king;
    textures["wq"] = w_queen;
    textures["wn"] = w_knight;
    textures["wb"] = w_bishop;
    textures["wr"] = w_rook;
    textures["wp"] = w_pawn;
}
