#include "MoveGeneration.hpp"
#include "Board.hpp"

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

std::vector<Move> MoveGeneration::getSlidingTypeMoves(Piece *piece)
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

std::vector<Move> MoveGeneration::getPawnMoves(Piece *piece)
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
    int attackLeftFile = (currentPosition + attackLeft) % 8;
    int attackRightFile = (currentPosition + attackRight) % 8;

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
    if ((currentPosition + attackLeft) >= 0 && (currentPosition + attackLeft) < 64 && std::abs(pawnFile - attackLeftFile) == 1)
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
    if ((currentPosition + attackRight) >= 0 && (currentPosition + attackRight) < 64 && std::abs(pawnFile - attackRightFile) == 1)
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

std::vector<Move> MoveGeneration::getKingMoves(Piece *piece)
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

std::vector<Move> MoveGeneration::getKnightMoves(Piece *piece)
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

void MoveGeneration::getCastleMoves(std::vector<Move> &validMoves, Piece *kingPiece)
{
    if (checkFlag)
        return;

    int currLocation = kingPiece->getPiecePosition();
    if ((Board::whiteToMove && whiteCastleKingSide && kingPiece->getPieceColor() == Piece::WHITE) || (!Board::whiteToMove && blackCastleKingSide && kingPiece->getPieceColor() == Piece::BLACK)) // kingside castle
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
    if ((Board::whiteToMove && whiteCastleQueenSide) || (!Board::whiteToMove && blackCastleQueenSide)) // queenside castle
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

bool MoveGeneration::squareUnderAttack(int square, int allyColor)
{

    for (int i = 0; i < 8; i++)
    {
        CheckOrPin possiblePin;
        bool pinExists = false;
        for (int j = 0; j < numSquaresToEdge[square][i]; j++)
        {
            int target = square + slidingMovesOffsets[i] * (j + 1);
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
            else if (curr->getPieceColor() != allyColor)
            {
                // orthogonal away from king is rook
                // diagnol away from king is bishop
                // 1 square away from king is pawn
                // any square away can be a queen
                // any direction 1 square away is king

                int pieceType = curr->getPieceType();
                if ((pieceType == Piece::ROOK && i <= 3) || (pieceType == Piece::BISHOP && i >= 4) || (j == 0 && pieceType == Piece::PAWN && ((allyColor != Piece::WHITE and (i == 4 || i == 6)) || (allyColor != Piece::BLACK and (i == 5 || i == 7)))) || pieceType == Piece::QUEEN || (j == 0 && pieceType == Piece::KING))
                {

                    if (!pinExists) // piece is in check
                    {
                        return true;
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
    int rank = square / 8;
    int file = square % 8;
    for (int i = 0; i < 8; i++)
    {
        int target = square + knightOffset[i];
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

        if (endPiece->getPieceColor() != allyColor && endPiece->getPieceType() == Piece::KNIGHT)
        {
            return true;
            checks.push_back(CheckOrPin(target, knightOffset[i]));
        }
    }

    return false;
}

void MoveGeneration::generateMovesInCurrentPosition()
{
    std::vector<CheckOrPin> pins;
    std::vector<CheckOrPin> checks;
    bool inCheck = false;

    this->checkForPinsAndChecks(pins, checks, inCheck);
    this->pins = pins;
    this->checks = checks;
    this->checkFlag = inCheck;

    std::map<int, std::vector<Move>> moves;
    Piece *kingPiece = Board::whiteToMove ? whiteKing : blackKing;

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
            // redo this function to iterate through the map properly
            for (const auto &[key, val] : moves)
            {
                if (val.empty())
                    continue;

                int pieceLocation = val[0].start;
                Piece *piece = board[pieceLocation]->getPiece();

                if (piece->getPieceType() != Piece::KING) // the king isn't moving so another piece has to block or capture
                {
                    std::vector<Move> allowedMoves;
                    for (int j = 0; j < val.size(); j++)
                    {
                        Move move = val[j];
                        if (searchVector(validSquares, move.target)) // if the target square is not in the valid moves square
                        {
                            allowedMoves.push_back(val[j]);
                        }
                    }
                    moves[key] = allowedMoves;
                }
            }
        }
        else // double check king has to move
        {

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

void MoveGeneration::checkForPinsAndChecks(std::vector<CheckOrPin> &pins, std::vector<CheckOrPin> &checks, bool &inCheck)
{
    inCheck = false;
    int enemyColor = Board::whiteToMove ? Piece::BLACK : Piece::WHITE;
    int allyColor = Board::whiteToMove ? Piece::WHITE : Piece::BLACK;
    Piece *kingPiece = Board::whiteToMove ? whiteKing : blackKing;
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

void MoveGeneration::boardEdgeData()
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

std::map<int, std::vector<Move>> MoveGeneration::pieceAvailableMoves()
{

    std::map<int, std::vector<Move>> positionMoves;
    int colorToMove = Board::whiteToMove ? Piece::WHITE : Piece::BLACK;
    std::vector<Piece *> *pieces = pieceList.getPieces(Board::whiteToMove);

    // loops through only the pieces
    for (int i = 0; i < PieceList::arrSize; i++)
    {
        for (int j = 0; j < pieces[i].size(); j++)
        {
            std::vector<Move> pieceMoves;
            Piece *piece = pieces[i][j];

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
            positionMoves[piece->getPiecePosition()] = pieceMoves;
        }
    }
    if (Board::whiteToMove)
    {
        auto pieceMoves = getKingMoves(whiteKing);
        positionMoves[whiteKing->getPiecePosition()] = pieceMoves;
    }
    else
    {
        auto pieceMoves = getKingMoves(blackKing);
        positionMoves[blackKing->getPiecePosition()] = pieceMoves;
    }

    this->moveset = positionMoves;
    return moveset;
}
