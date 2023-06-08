#include "Board.hpp"
#include <vector>
#include <iostream>

bool searchVector(std::vector<int> &v, int i)
{
    for (int k = 0; k < v.size(); k++)
    {
        if (v[k] == i)
        {
            std::cout << "move valid: " << i << '\n';
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
            this->generateMovesInCurrentPosition();
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

    if (checkFlag)
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
        else if (pieceType == Piece::KING)
        {
            pieceMoves = getKingMoves(piece);
        }
        positionMoves[i] = pieceMoves;
    }

    return positionMoves;
}

std::vector<Move> Board::getPawnMoves(Piece *piece)
{
    int currentPosition = piece->getPiecePosition();
    int moveOffset = piece->getPieceColor() == Piece::WHITE ? -1 : 1;
    int pawnStart = piece->getPieceColor() == Piece::WHITE ? 6 : 1;
    int pawnPromotion = piece->getPieceColor() == Piece::WHITE ? 0 : 7;

    int pieceRank = currentPosition / 8;
    std::vector<Move> validMoves;
    int attackLeft = 9 * moveOffset;
    int attackRight = 7 * moveOffset;
    int forward1 = currentPosition + 8 * moveOffset;

    bool piecePinned = false;
    int pinDirection = 999;
    for (auto i : pins)
    {
        if (i.position == piece->getPiecePosition())
        {
            piecePinned = true;
            pinDirection = i.direction;
            break;
        }
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
            Move move1(currentPosition, forward1);
            validMoves.push_back(move1);

            // move forward 2
            if (pieceRank == pawnStart && board[currentPosition + 16 * moveOffset]->hasNullPiece())
            {
                Move move2(currentPosition, currentPosition + 16 * moveOffset);
                validMoves.push_back(move2);
            }
        }
    }
    // left capture
    if (!board[currentPosition + attackLeft]->hasNullPiece() && board[currentPosition + attackLeft]->getPiece()->getPieceColor() != piece->getPieceColor())
    {
        if (!piecePinned || pinDirection == 9 * moveOffset)
        {

            Move takeLeft(currentPosition, currentPosition + attackLeft);
            validMoves.push_back(takeLeft);
        }
    }
    // right capture
    if (!board[currentPosition + attackRight]->hasNullPiece() && board[currentPosition + attackRight]->getPiece()->getPieceColor() != piece->getPieceColor())
    {
        if (!piecePinned || pinDirection == 7 * moveOffset)
        {

            Move takeRight(currentPosition, currentPosition + attackRight);
            validMoves.push_back(takeRight);
        }
    }
    // enPassant moves
    if (currentPosition + attackRight == possibleEnPassant)
    {
        Move takeRightPassant(currentPosition, currentPosition + attackRight, true);
        validMoves.push_back(takeRightPassant);
    }
    if (currentPosition + attackLeft == possibleEnPassant)
    {
        Move takeLeftPassant(currentPosition, currentPosition + attackLeft, true);
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
    std::vector<Move> validMoves;
    for (int i = 0; i < 8; i++)
    {
        bool inCheck = false;
        int target = currentLocation + kingMovesOffsets[i];
        if (target >= 64 || target < 0)
            continue;

        Move move(currentLocation, target);
        if (board[target]->hasNullPiece())
        {
            int originalKingPos = whiteToMove ? whiteKing->getPiecePosition() : blackKing->getPiecePosition();
            if (whiteToMove)
            {
                whiteKing->setPiecePosition(target);
            }
            else
            {
                blackKing->setPiecePosition(target);
            }

            checkForPinsAndChecks(pins, checks, inCheck);

            if (!inCheck)
            {
                validMoves.push_back(move);
            }

            if (whiteToMove)
            {
                whiteKing->setPiecePosition(originalKingPos);
            }
            else
            {
                blackKing->setPiecePosition(originalKingPos);
            }
            continue;
        }

        Piece *otherPiece = board[target]->getPiece();
        int otherPieceColor = otherPiece->getPieceColor();
        int currentPieceColor = pieceColor;
        if (otherPieceColor == currentPieceColor)
            continue;

        int originalKingPos = whiteToMove ? whiteKing->getPiecePosition() : blackKing->getPiecePosition();
        if (whiteToMove)
        {
            whiteKing->setPiecePosition(target);
        }
        else
        {
            blackKing->setPiecePosition(target);
        }

        checkForPinsAndChecks(pins, checks, inCheck);

        if (!inCheck)
        {
            validMoves.push_back(move);
        }

        if (whiteToMove)
        {
            whiteKing->setPiecePosition(originalKingPos);
        }
        else
        {
            blackKing->setPiecePosition(originalKingPos);
        }
    }
    return validMoves;
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
    for (auto i : pins)
    {
        if (i.position == piece->getPiecePosition())
        {
            piecePinned = true;
            break;
        }
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

        Move move(currentLocation, target);
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
    for (auto i : pins)
    {
        if (i.position == piece->getPiecePosition())
        {
            piecePinned = true;
            pinDirection = i.direction;
            break;
        }
    }

    for (int currentOffset = startIdx; currentOffset < endIdx; currentOffset++)
    {

        for (int n = 0; n < numSquaresToEdge[currentPosition][currentOffset]; n++)
        {
            if (piecePinned && pinDirection != slidingMovesOffsets[currentOffset])
                continue;

            int target = currentPosition + slidingMovesOffsets[currentOffset] * (n + 1);
            Move newMove(currentPosition, target);

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
    }

    if (!endSquare->hasNullPiece())
    {
        Piece *capturedPiece = endSquare->getPiece();
        capturedPieces.push(capturedPiece);
        move.capture = true;
    }
    else if (move.isEnPassant)
    {
        int offset = move.start - move.target == 9 ? -1 : 1;
        Piece *pawn = board[move.start + offset]->getPiece();
        capturedPieces.push(pawn);
        board[move.start + offset]->setPiece(nullptr);
    }

    piece->setPiecePosition(move.target);
    endSquare->setPiece(piece);
    startSquare->setPiece(nullptr);
    moveHistory.push(move);
    whiteToMove = !whiteToMove;
}

void Board::unmakeMove()
{
    Move pastMove = moveHistory.top();
    moveHistory.pop();

    int start = pastMove.start;
    int target = pastMove.target;
    // undo everything lmao
    Square *startSquare = board[start];
    Square *endSquare = board[target];
    Piece *piece = board[target]->getPiece();
    int startRank = start / 8;
    int endRank = target / 8;

    // puts piece back
    startSquare->setPiece(piece);
    piece->setPiecePosition(start);

    // resets the enPassantFlag if necessary
    possibleEnPassant = pastMove.possibleEnPassant;

    // places captured piece back
    if (pastMove.isEnPassant)
    {
        int offset = start - target == 9 ? -1 : 1;
        Piece *pawn = capturedPieces.top();
        capturedPieces.pop();
        board[start + offset]->setPiece(pawn);
        pawn->setPiecePosition(start + offset);
    }
    else if (pastMove.capture)
    {
        Piece *returnedPiece = capturedPieces.top();
        capturedPieces.pop();
        endSquare->setPiece(returnedPiece);
        returnedPiece->setPiecePosition(target);
    }

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
    int enemyColor = !whiteToMove ? Piece::WHITE : Piece::BLACK;
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
                if ((pieceType == Piece::ROOK && 0 <= i <= 3) || (4 <= j <= 7 && pieceType == Piece::BISHOP) || (j == 1 && pieceType == Piece::PAWN && ((enemyColor == Piece::WHITE and (i == 4 || i == 6)) || (enemyColor == Piece::BLACK and (i == 5 || i == 7)))) || pieceType == Piece::QUEEN || (j == 1 && pieceType == Piece::KING))
                {

                    if (!pinExists) // piece is in check
                    {
                        inCheck = true;
                        checks.push_back(CheckOrPin(target, slidingMovesOffsets[i]));
                    }
                    else // piece is pinned
                    {
                        pins.push_back(possiblePin);
                    }
                    break;
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