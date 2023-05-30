#include "Board.hpp"
#include <vector>
#include <iostream>

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
            // call a move swap function or something
            Piece *pawn = board[startIdx]->getPiece();
            int startRank = startIdx / 8;
            int endRank = target / 8;

            if (pawn->getPieceType() == Piece::PAWN && std::abs(startRank - endRank) == 2)
            {
                possibleEnPassant = (startIdx + target) / 2;
                std::cout << "POSSIBLE EN PESSANT ON " << (startIdx + target) / 2 << '\n';
            }
            else
            {
                possibleEnPassant = 999;
            }
            this->makeMove(move);
            this->generateMovesInCurrentPosition();
            return true;
        }
    }
    return false;
}

void Board::generateMovesInCurrentPosition()
{
    std::vector<std::vector<Move>> positionMoves(64);

    for (int i = 0; i < 64; i++)
    {
        if (board[i]->hasNullPiece())
            continue;

        Piece *currentPiece = board[i]->getPiece();
        std::vector<Move> pieceMoves = pieceAvailableMoves(currentPiece);

        positionMoves[i] = pieceMoves;
    }
    moveset = positionMoves;
}

std::vector<Move> Board::pieceAvailableMoves(Piece *piece)
{

    int pieceType = piece->getPieceType();
    if (pieceType == Piece::QUEEN || pieceType == Piece::BISHOP || pieceType == Piece::ROOK)
    {
        return getSlidingTypeMoves(piece);
    }
    else if (pieceType == Piece::KNIGHT)
    {
        return getKnightMoves(piece);
    }
    else if (pieceType == Piece::PAWN)
    {
        return getPawnMoves(piece);
    }
    else if (pieceType == Piece::KING)
    {
        return getKingMoves(piece);
    }

    return {};
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

    if (forward1 >= 64 || forward1 < 0)
    {
        return {};
    }

    if (board[forward1]->hasNullPiece())
    {
        Move move1(currentPosition, forward1);
        validMoves.push_back(move1);
    }
    if (pieceRank == pawnStart && board[currentPosition + 16 * moveOffset]->hasNullPiece())
    {
        // 2 moves from spawn, something with en peasant has to happen here
        Move move2(currentPosition, currentPosition + 16 * moveOffset);
        validMoves.push_back(move2);
    }
    if (!board[currentPosition + attackLeft]->hasNullPiece() && board[currentPosition + attackLeft]->getPiece()->getPieceColor() != piece->getPieceColor())
    {
        Move takeLeft(currentPosition, currentPosition + attackLeft);
        validMoves.push_back(takeLeft);
    }
    if (!board[currentPosition + attackRight]->hasNullPiece() && board[currentPosition + attackRight]->getPiece()->getPieceColor() != piece->getPieceColor())
    {
        Move takeRight(currentPosition, currentPosition + attackRight);
        validMoves.push_back(takeRight);
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

    int currentLocation = piece->getPiecePosition();
    int pieceColor = piece->getPieceColor();
    std::vector<Move> validMoves;
    for (int i = 0; i < 8; i++)
    {
        int target = currentLocation + kingMovesOffsets[i];
        if (target >= 64 || target < 0)
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

std::vector<Move> Board::getKnightMoves(Piece *piece)
{
    // looping through this array offset  knightOffset[8] = {6, 10, 15, 17, -6, -10, -15, -17};

    int currentLocation = piece->getPiecePosition();
    int pieceColor = piece->getPieceColor();
    std::vector<Move> validMoves;
    int rank = currentLocation / 8;
    int file = currentLocation % 8;
    for (int i = 0; i < 8; i++)
    {
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

    for (int currentOffset = startIdx; currentOffset < endIdx; currentOffset++)
    {

        for (int n = 0; n < numSquaresToEdge[currentPosition][currentOffset]; n++)
        {
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
    Square *startSquare = board[move.start];
    Square *endSquare = board[move.target];
    Piece *piece = startSquare->getPiece();
    

    if (!endSquare->hasNullPiece())
    {
        capturedPieces.push_back(endSquare->getPiece());
    }
    else if (move.isEnPassant)
    {
        int offset = move.start - move.target == 9 ? -1 : 1;
        Piece *pawn = board[move.start + offset]->getPiece();
        capturedPieces.push_back(pawn);
        board[move.start + offset]->setPiece(nullptr);
    }

    piece->setPiecePosition(move.target);
    endSquare->setPiece(piece);
    startSquare->setPiece(nullptr);
    whiteToMove = !whiteToMove;
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