#include "Board.hpp"
#include <vector>
#include <iostream>
#include "GameEngine.hpp"

bool searchVector(std::vector<int> &v, int i)
{
    for (int k = 0; k < v.size(); k++)
    {
        if (v[k] == i)
        {
            return true;
        }
    }
    return false;
}

std::vector<Move> Board::getPieceMoves(int idx)
{
    return moveset[idx];
}

bool Board::validateMove(int startIdx, int target)
{
    for (auto move : moveset[startIdx])
    {
        if (move.target == target)
        {
            this->makeMove(move);
            return true;
        }
    }
    return false;
}

void Board::generateMovesInCurrentPosition()
{
    std::vector<CheckOrPin> pins;
    std::vector<CheckOrPin> checks;
    bool inCheck = false;

    this->checkForPinsAndChecks(pins, checks, inCheck);
    this->pins = pins;
    this->checks = checks;
    this->checkFlag = inCheck;

    std::vector<std::vector<Move>> moves;
    Piece *kingPiece = whiteToMove ? whiteKing : blackKing;

    if (inCheck)
    {

        if (this->checks.size() == 1) // if there is exactly one check location
        {
            moves = pieceAvailableMoves();
            // to block check a piece has to move between the attacking piece and king
            auto check = checks[0];
            int checkPosition = check.position;
            int checkDirection = check.direction;

            Piece *checkingPiece = board[checkPosition]->getPiece();
            std::vector<int> validSquares;

            if (checkingPiece->getPieceType() == Piece::KNIGHT)
            {
                validSquares.push_back(checkPosition);
            }
            else
            {
                for (int i = 0; i < 8; i++)
                {
                    int validSquare = kingPiece->getPiecePosition() + checkDirection * i;
                    validSquares.push_back(validSquare);
                    if (validSquare == checkPosition)
                        break;
                }
            }

            for (int i = 0; i < moves.size(); i++)
            {
                if (moves[i].empty())
                    continue;

                int pieceLocation = moves[i][0].start;
                Piece *piece = board[pieceLocation]->getPiece();

                if (piece->getPieceType() != Piece::KING) // the king isn't moving so another piece has to block or capture
                {
                    std::vector<Move> allowedMoves;
                    for (int j = 0; j < moves[i].size(); j++)
                    {
                        Move move = moves[i][j];
                        if (searchVector(validSquares, move.target)) // if the target square is not in the valid moves square
                        {
                            allowedMoves.push_back(moves[i][j]);
                        }
                    }
                    moves[i] = allowedMoves;
                }
            }
        }
        else // double check king has to move
        {

            moves = std::vector<std::vector<Move>>(kingPiece->getPiecePosition());
            auto kingMoves = getKingMoves(kingPiece);
            moves[kingPiece->getPiecePosition()] = kingMoves;
        }
    }
    else
    {
        // not in check
        moves = pieceAvailableMoves();
    }

    this->moveset = moves;
}

std::vector<std::vector<Move>> Board::pieceAvailableMoves()
{

    std::vector<std::vector<Move>> positionMoves(64);

    for (int i = 0; i < 64; i++)
    {
        if (board[i]->hasNullPiece())
            continue;

        Piece *piece = board[i]->getPiece();
        std::vector<Move> pieceMoves;

        int pieceType = piece->getPieceType();
        if (pieceType == Piece::QUEEN || pieceType == Piece::BISHOP || pieceType == Piece::ROOK)
        {
            pieceMoves = getSlidingTypeMoves(piece);
        }
        else if (pieceType == Piece::KNIGHT)
        {
            pieceMoves = getKnightMoves(piece);
        }
        else if (pieceType == Piece::PAWN)
        {
            pieceMoves = getPawnMoves(piece);
        }
        positionMoves[i] = pieceMoves;
    }
    this->moveset = positionMoves;
    auto whiteKingMoves = getKingMoves(whiteKing);
    auto blackKingMoves = getKingMoves(blackKing);
    moveset[whiteKing->getPiecePosition()] = whiteKingMoves;
    moveset[blackKing->getPiecePosition()] = blackKingMoves;

    return moveset;
}

std::vector<Move> Board::getPawnMoves(Piece *piece)
{
    int currentPosition = piece->getPiecePosition();
    int moveOffset = piece->getPieceColor() == Piece::WHITE ? -1 : 1;
    int pawnStart = piece->getPieceColor() == Piece::WHITE ? 6 : 1;
    int pawnPromotion = piece->getPieceColor() == Piece::WHITE ? 0 : 7;

    int pieceRank = currentPosition / 8;
    int pawnFile = (currentPosition % 8);

    std::vector<Move> validMoves;
    int attackLeft = 9 * moveOffset;
    int attackRight = 7 * moveOffset;
    int forward1 = currentPosition + 8 * moveOffset;

    bool piecePinned = false;
    int pinDirection = 999;
    int c = 0;
    for (auto i : pins)
    {
        if (i.position == piece->getPiecePosition())
        {
            piecePinned = true;
            pinDirection = i.direction;
            pins[c] = {};
            break;
        }
        c++;
    }

    if (forward1 >= 64 || forward1 < 0)
    {
        return {};
    }

    if (board[forward1]->hasNullPiece())
    {
        if (!piecePinned || pinDirection == 8 * moveOffset) // move is up or down the board
        {
            // move forward 1
            Move move1(currentPosition, forward1, piece->getPieceTypeRaw());
            validMoves.push_back(move1);

            // move forward 2
            if (pieceRank == pawnStart && board[currentPosition + 16 * moveOffset]->hasNullPiece())
            {
                Move move2(currentPosition, currentPosition + 16 * moveOffset, piece->getPieceTypeRaw());
                validMoves.push_back(move2);
            }
        }
    }

    // left capture
    if ((currentPosition + attackLeft) > 0 && (currentPosition + attackLeft) < 64 && pawnFile != 0 && pawnFile != 7)
    {

        if (!board[currentPosition + attackLeft]->hasNullPiece() && board[currentPosition + attackLeft]->getPiece()->getPieceColor() != piece->getPieceColor())
        {
            if (!piecePinned || pinDirection == 9 * moveOffset)
            {

                Move takeLeft(currentPosition, currentPosition + attackLeft, piece->getPieceTypeRaw());
                validMoves.push_back(takeLeft);
            }
        }
    }
    // right capture
    if ((currentPosition + attackRight) > 0 && (currentPosition + attackRight) < 64)
    {

        if (!board[currentPosition + attackRight]->hasNullPiece() && board[currentPosition + attackRight]->getPiece()->getPieceColor() != piece->getPieceColor())
        {
            if (!piecePinned || pinDirection == 7 * moveOffset)
            {

                Move takeRight(currentPosition, currentPosition + attackRight, piece->getPieceTypeRaw());
                validMoves.push_back(takeRight);
            }
        }
    }
    // enPassant moves
    if (currentPosition + attackRight == possibleEnPassant)
    {
        Move takeRightPassant(currentPosition, currentPosition + attackRight, piece->getPieceTypeRaw(), true);
        validMoves.push_back(takeRightPassant);
    }
    if (currentPosition + attackLeft == possibleEnPassant && pawnFile != 0 && pawnFile != 7)
    {
        Move takeLeftPassant(currentPosition, currentPosition + attackLeft, piece->getPieceTypeRaw(), true);
        validMoves.push_back(takeLeftPassant);
    }

    return validMoves;
}

std::vector<Move> Board::getKingMoves(Piece *piece)
{
    // looping through this array offset  kingMovesOffsets[8] = {1, 7, 8, 9, -1, -7, -8, -9};
    std::vector<CheckOrPin> pins;
    std::vector<CheckOrPin> checks;

    int currentLocation = piece->getPiecePosition();
    int pieceColor = piece->getPieceColor();
    int file = currentLocation / 8;
    int rank = currentLocation % 8;
    std::vector<Move> validMoves;
    for (int i = 0; i < 8; i++)
    {
        bool inCheck = false;
        int target = currentLocation + kingMovesOffsets[i];
        if (target >= 64 || target < 0)
            continue;

        int kingFile = target / 8;
        int kingRank = target % 8;
        int maxMove = std::max(std::abs(file - kingFile), std::abs(rank - kingRank));
        if (maxMove != 1)
            continue;

        Move move(currentLocation, target, piece->getPieceTypeRaw());

        if (!board[target]->hasNullPiece())
        {
            Piece *otherPiece = board[target]->getPiece();
            int otherPieceColor = otherPiece->getPieceColor();
            int currentPieceColor = pieceColor;
            if (otherPieceColor == currentPieceColor)
                continue;
        }

        int originalKingPos = piece->getPiecePosition();
        piece->setPiecePosition(target);
        checkForPinsAndChecks(pins, checks, inCheck);
        if (!inCheck)
        {
            validMoves.push_back(move);
        }
        piece->setPiecePosition(originalKingPos);
    }

    getCastleMoves(validMoves, piece);
    return validMoves;
}

void Board::getCastleMoves(std::vector<Move> &validMoves, Piece *kingPiece)
{
    if (checkFlag)
        return;

    int currLocation = kingPiece->getPiecePosition();
    if ((whiteToMove && whiteCastleKingSide && kingPiece->getPieceColor() == Piece::WHITE) || (!whiteToMove && blackCastleKingSide && kingPiece->getPieceColor() == Piece::BLACK)) // kingside castle
    {
        if (board[currLocation + 1]->hasNullPiece() && board[currLocation + 2]->hasNullPiece())
        {
            if (!squareUnderAttack(currLocation + 1, kingPiece->getPieceColor()) && !squareUnderAttack(currLocation + 2, kingPiece->getPieceColor()))
            {
                Move move(currLocation, currLocation + 2, kingPiece->getPieceTypeRaw(), false, true);
                move.pieceType = kingPiece->getPieceTypeRaw();
                validMoves.push_back(move);
            }
        }
    }
    if ((whiteToMove && whiteCastleQueenSide) || (!whiteToMove && blackCastleQueenSide)) // queenside castle
    {
        if (board[currLocation - 1]->hasNullPiece() && board[currLocation - 2]->hasNullPiece() && board[currLocation - 3]->hasNullPiece())
        {
            if (!squareUnderAttack(currLocation - 1, kingPiece->getPieceColor()) && !squareUnderAttack(currLocation - 2, kingPiece->getPieceColor()))
            {
                Move move(currLocation, currLocation - 2, kingPiece->getPieceTypeRaw(), false, true);
                validMoves.push_back(move);
            }
        }
    }
}

std::vector<Move> Board::getKnightMoves(Piece *piece)
{
    // looping through this array offset  knightOffset[8] = {6, 10, 15, 17, -6, -10, -15, -17};

    int currentLocation = piece->getPiecePosition();
    int pieceColor = piece->getPieceColor();
    std::vector<Move> validMoves;
    int rank = currentLocation / 8;
    int file = currentLocation % 8;

    int piecePinned = false;

    int c = 0;
    for (auto i : pins)
    {
        if (i.position == piece->getPiecePosition())
        {
            piecePinned = true;
            pins[c] = {};
            break;
        }
        c++;
    }

    for (int i = 0; i < 8; i++)
    {
        if (piecePinned)
            continue;

        int target = currentLocation + knightOffset[i];
        if (target >= 64 || target < 0)
            continue;

        int knightRank = target / 8;
        int knightFile = target % 8;

        // makes sure knight doesnt wrap around the edge of the board
        int maxJumpCoord = std::max(std::abs(file - knightFile), std::abs(rank - knightRank));
        if (maxJumpCoord != 2)
            continue;

        Move move(currentLocation, target, piece->getPieceTypeRaw());
        move.pieceType = piece->getPieceTypeRaw();
        if (board[target]->hasNullPiece())
        {
            validMoves.push_back(move);
            continue;
        }

        Piece *otherPiece = board[target]->getPiece();
        int otherPieceColor = otherPiece->getPieceColor();
        int currentPieceColor = pieceColor;
        if (otherPieceColor == currentPieceColor)
            continue;
        validMoves.push_back(move);
    }
    return validMoves;
}

std::vector<Move> Board::getSlidingTypeMoves(Piece *piece)
{
    int currentPosition = piece->getPiecePosition();
    int pieceType = piece->getPieceType();
    int pieceColor = piece->getPieceColor();
    std::vector<Move> possibleMoves;
    // these offsets correspond to the index of these values
    // const int slidingMovesOffsets[8] = {1, -1, 8, -8, 7, -7, 9, -9};
    int startIdx = pieceType == Piece::BISHOP ? 4 : 0;
    int endIdx = pieceType == Piece::ROOK ? 4 : 8;

    int piecePinned = false;
    int pinDirection = 9999;

    int c = 0;
    for (auto i : pins)
    {
        if (i.position == piece->getPiecePosition())
        {
            piecePinned = true;
            pinDirection = i.direction;
            pins[c] = {};
            break;
        }
        c++;
    }

    for (int currentOffset = startIdx; currentOffset < endIdx; currentOffset++)
    {

        for (int n = 0; n < numSquaresToEdge[currentPosition][currentOffset]; n++)
        {
            if (piecePinned && pinDirection != slidingMovesOffsets[currentOffset])
                continue;

            int target = currentPosition + slidingMovesOffsets[currentOffset] * (n + 1);
            Move newMove(currentPosition, target, piece->getPieceTypeRaw());

            if (board[target]->hasNullPiece())
            {
                possibleMoves.push_back(newMove);
            }
            else
            {
                Piece *otherPiece = board[target]->getPiece();
                int otherPieceColor = otherPiece->getPieceColor();
                int currentPieceColor = pieceColor;
                if (otherPieceColor == currentPieceColor)
                {
                    break;
                }
                possibleMoves.push_back(newMove);
                break;
            }
        }
    }
    return possibleMoves;
}

void Board::makeMove(Move move)
{
    // call a move swap function or something
    Square *startSquare = board[move.start];
    Square *endSquare = board[move.target];
    Piece *piece = startSquare->getPiece();
    int startRank = move.start / 8;
    int endRank = move.target / 8;

    if (piece->getPieceType() == Piece::PAWN && std::abs(startRank - endRank) == 2)
    {
        this->possibleEnPassant = (move.start + move.target) / 2;
        move.possibleEnPassant = this->possibleEnPassant;
    }
    else
    {
        this->possibleEnPassant = 999;
        move.possibleEnPassant = 999;
    }

    // capture
    if (!endSquare->hasNullPiece())
    {
        Piece *capturedPiece = endSquare->getPiece();
        move.capture = true;
        move.capturedPiece = capturedPiece;

        if (capturedPiece->getPieceType() == Piece::ROOK)
        {
            int rank = capturedPiece->getPiecePosition() / 8;
            int file = capturedPiece->getPiecePosition() % 8;
            if (capturedPiece->getPieceColor() == Piece::WHITE)
            {
                // queen side  rook
                if (file == 0 && rank == 7)
                {
                    whiteCastleQueenSide = false;
                }
                // king side  rook
                else if (file == 7 && rank == 7)
                {
                    whiteCastleKingSide = false;
                }
            }
            else
            {
                if (file == 0 && rank == 0)
                {
                    blackCastleQueenSide = false;
                }
                else if (file == 7 && rank == 0)
                {
                    blackCastleKingSide = false;
                }
            }
        }
    }
    // en passant move
    else if (move.isEnPassant)
    {
        int offset = move.start - move.target == 9 ? -1 : 1;
        Piece *pawn = board[move.start + offset]->getPiece();
        move.capturedPiece = pawn;
        board[move.start + offset]->setPiece(nullptr);
    }
    // pawn promotion
    else if (piece->getPieceType() == Piece::PAWN)
    {
        if (piece->getPieceColor() == Piece::WHITE && endSquare->getSquarePosition() == 0)
        {
            move.pawnPromotion = true;
        }
        else if (piece->getPieceColor() == Piece::BLACK && endSquare->getSquarePosition() == 7)
        {
            move.pawnPromotion = true;
        }
    }
    // castle move
    else if (move.isCastle)
    {
        int startFile = move.start % 8;
        int endFile = move.target % 8;
        if (endFile - startFile == 2) // kingside castle
        {
            Piece *rook = board[move.target + 1]->getPiece();
            rook->setPiecePosition(move.target - 1, true);
            board[move.target - 1]->setPiece(rook);
            board[move.target + 1]->setPiece(nullptr);
        }
        else // queen side castle
        {
            Piece *rook = board[move.target - 2]->getPiece();
            rook->setPiecePosition(move.target + 1, true);
            board[move.target - 2]->setPiece(nullptr);
            board[move.target + 1]->setPiece(rook);
        }
    }

    CastlingRights oldRights;
    oldRights.blackCastleKingSide = this->blackCastleKingSide;
    oldRights.blackCastleQueenSide = this->blackCastleQueenSide;
    oldRights.whiteCastleKingSide = this->whiteCastleKingSide;
    oldRights.whiteCastleQueenSide = this->whiteCastleQueenSide;
    std::cout << "WhiteCastleKing: " << whiteCastleKingSide << '\n';
    std::cout << "WhiteCastleQueen: " << whiteCastleQueenSide << '\n';
    std::cout << "BlackCastleKing: " << blackCastleKingSide << '\n';
    std::cout << "BlackCastleKing: " << blackCastleQueenSide << '\n';
    castlingHistory.push(oldRights);
    updateCastlingRights(move);

    piece->setPiecePosition(move.target);
    endSquare->setPiece(piece);
    startSquare->setPiece(nullptr);
    moveHistory.push(move);

    movesetHistory.push(moveset);
    whiteToMove = !whiteToMove;
}

void Board::updateCastlingRights(const Move &move)
{

    if (Piece::getPieceType(move.pieceType) == Piece::KING && Piece::getPieceColor(move.pieceType) == Piece::BLACK)
    {
        this->blackCastleKingSide = false;
        this->blackCastleQueenSide = false;
    }
    else if (Piece::getPieceType(move.pieceType) == Piece::KING && Piece::getPieceColor(move.pieceType) == Piece::WHITE)
    {
        this->whiteCastleKingSide = false;
        this->whiteCastleQueenSide = false;
    }
    else if (Piece::getPieceType(move.pieceType) == Piece::ROOK && Piece::getPieceColor(move.pieceType) == Piece::WHITE)
    {
        int startRow = move.start / 8;
        int startCol = move.start % 8;
        if (startCol == 0 && startRow == 7)
        {
            this->whiteCastleQueenSide = false;
        }
        else if (startCol == 7 && startRow == 7)
        {
            this->whiteCastleKingSide = false;
        }
    }
    else if (Piece::getPieceType(move.pieceType) == Piece::ROOK && Piece::getPieceColor(move.pieceType) == Piece::BLACK)
    {
        int startRow = move.start / 8;
        int startCol = move.start % 8;
        if (startCol == 0 && startRow == 0)
        {
            this->blackCastleQueenSide = false;
        }
        else if (startCol == 7 && startRow == 0)
        {
            this->blackCastleKingSide = false;
        }
    }
}

void Board::unmakeMove()
{
    if (moveHistory.empty())
        return;
    // call a move swap function or something
    Move move = moveHistory.top();
    moveHistory.pop();
    Square *target = board[move.target];
    Square *start = board[move.start];
    Piece *movedPiece = target->getPiece();

    // moves piece back to original square
    movedPiece->setPiecePosition(move.start, true);
    start->setPiece(movedPiece);
    target->setPiece(nullptr);

    // capture
    if (move.capture)
    {
        Piece *capturedPiece = move.capturedPiece;
        target->setPiece(capturedPiece);
    }
    // en passant move
    if (move.isEnPassant)
    {
        possibleEnPassant = move.possibleEnPassant;
        int offset = move.start - move.target == 9 ? -1 : 1;
        board[move.start + offset]->setPiece(move.capturedPiece);
    }
    // pawn promotion
    if (move.pawnPromotion)
    {
        movedPiece->setPieceType(Piece::PAWN | movedPiece->getPieceColor());
        movedPiece->revertSprite();
    }
    // castle move
    if (move.isCastle)
    {
        int startFile = move.start % 8;
        int endFile = move.target % 8;
        if (endFile - startFile == 2) // kingside castle
        {
            Piece *rook = board[move.target - 1]->getPiece();
            rook->setPiecePosition(move.target + 1, true);
            board[move.target + 1]->setPiece(rook);
            board[move.target - 1]->setPiece(nullptr);
        }
        else // queen side castle
        {
            Piece *rook = board[move.target + 1]->getPiece();
            rook->setPiecePosition(move.target - 2, true);
            board[move.target - 2]->setPiece(rook);
            board[move.target + 1]->setPiece(nullptr);
        }
    }
    CastlingRights castleHistory = castlingHistory.top();
    this->blackCastleKingSide = castleHistory.blackCastleKingSide;
    this->blackCastleQueenSide = castleHistory.blackCastleQueenSide;
    this->whiteCastleKingSide = castleHistory.whiteCastleKingSide;
    this->whiteCastleQueenSide = castleHistory.whiteCastleQueenSide;
    castlingHistory.pop();

    this->moveset = movesetHistory.top();
    movesetHistory.pop();

    whiteToMove = !whiteToMove;
}

void Board::setSquarePiece(int idx, Piece *other)
{
    bool isWhite = other->getPieceColor() == Piece::WHITE;
    board[idx]->setPiece(other);

    if (isWhite)
    {
        // whitePieces.push_back(other);
        if (other->getPieceType() == Piece::KING)
            whiteKing = other;
    }
    else
    {
        // blackPieces.push_back(other);
        if (other->getPieceType() == Piece::KING)
            blackKing = other;
    }
}

void Board::checkForPinsAndChecks(std::vector<CheckOrPin> &pins, std::vector<CheckOrPin> &checks, bool &inCheck)
{
    inCheck = false;
    int enemyColor = whiteToMove ? Piece::BLACK : Piece::WHITE;
    int allyColor = whiteToMove ? Piece::WHITE : Piece::BLACK;
    Piece *kingPiece = whiteToMove ? whiteKing : blackKing;
    int kingPosition = kingPiece->getPiecePosition();
    // const int slidingMovesOffsets[8] = {1, -1, 8, -8, 7, -7, 9, -9};
    // check all directions from the king
    for (int i = 0; i < 8; i++)
    {
        CheckOrPin possiblePin;
        bool pinExists = false;
        for (int j = 0; j < numSquaresToEdge[kingPosition][i]; j++)
        {
            int target = kingPosition + slidingMovesOffsets[i] * (j + 1);
            if (board[target]->hasNullPiece())
                continue;

            Piece *curr = board[target]->getPiece();
            if (curr->getPieceColor() == allyColor && curr->getPieceType() != Piece::KING)
            {
                if (!pinExists)
                {
                    possiblePin.position = target;
                    possiblePin.direction = slidingMovesOffsets[i];
                    pinExists = true;
                }
                else
                {
                    possiblePin.position = -1111;
                    possiblePin.direction = -1111;
                    pinExists = false;
                    break; // second ally piece so no pin or check is possible
                }
            }
            else if (curr->getPieceColor() == enemyColor)
            {
                // orthogonal away from king is rook
                // diagnol away from king is bishop
                // 1 square away from king is pawn
                // any square away can be a queen
                // any direction 1 square away is king

                int pieceType = curr->getPieceType();
                if ((pieceType == Piece::ROOK && i <= 3) || (pieceType == Piece::BISHOP && i >= 4) || (j == 0 && pieceType == Piece::PAWN && ((enemyColor == Piece::WHITE and (i == 4 || i == 6)) || (enemyColor == Piece::BLACK and (i == 5 || i == 7)))) || pieceType == Piece::QUEEN || (j == 0 && pieceType == Piece::KING))
                {

                    if (!pinExists) // piece is in check
                    {
                        inCheck = true;
                        checks.push_back(CheckOrPin(target, slidingMovesOffsets[i]));
                        break;
                    }
                    else // piece is pinned
                    {
                        pins.push_back(possiblePin);
                        break;
                    }
                }
                break;
            }
        }
    }

    // check knight moves on king
    // const int knightOffset[8] = {6, 10, 15, 17, -6, -10, -15, -17};
    int rank = kingPosition / 8;
    int file = kingPosition % 8;
    for (int i = 0; i < 8; i++)
    {
        int target = kingPosition + knightOffset[i];
        if (target >= 64 || target < 0)
            continue;
        if (board[target]->hasNullPiece())
            continue;

        int knightRank = target / 8;
        int knightFile = target % 8;

        // makes sure knight doesnt wrap around the edge of the board
        int maxJumpCoord = std::max(std::abs(file - knightFile), std::abs(rank - knightRank));
        if (maxJumpCoord != 2)
            continue;

        Piece *endPiece = board[target]->getPiece();

        if (endPiece->getPieceColor() == enemyColor && endPiece->getPieceType() == Piece::KNIGHT)
        {
            inCheck = true;
            checks.push_back(CheckOrPin(target, knightOffset[i]));
        }
    }
}

bool Board::squareUnderAttack(int square, int pieceColor)
{

    for (auto piece : moveset)
    {
        for (auto move : piece)
        {
            Piece *other = board[move.start]->getPiece();
            if (move.target == square && other->getPieceColor() != pieceColor)
            {
                return true;
            }
        }
    }

    return false;
}

void Board::promotePawn(int pieceLocation, int pieceType)
{
    Piece *piece = board[pieceLocation]->getPiece();
    piece->setPieceType(pieceType);
}

void Board::boardEdgeData()
{
    for (int file = 0; file < 8; file++)
    {
        for (int rank = 0; rank < 8; rank++)
        {
            int south = 7 - rank;
            int north = rank;
            int west = file;
            int east = 7 - file;

            int squareIdx = (rank * 8) + file;

            // yes I know this looks awful, I hate C++
            numSquaresToEdge[squareIdx][0] = east;
            numSquaresToEdge[squareIdx][1] = west;
            numSquaresToEdge[squareIdx][2] = south;
            numSquaresToEdge[squareIdx][3] = north;
            numSquaresToEdge[squareIdx][4] = std::min(south, west);
            numSquaresToEdge[squareIdx][5] = std::min(north, east);
            numSquaresToEdge[squareIdx][6] = std::min(east, south);
            numSquaresToEdge[squareIdx][7] = std::min(north, west);
        }
    }
}