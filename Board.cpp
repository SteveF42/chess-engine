#include "Board.hpp"
#include <vector>
#include <iostream>

std::vector<Move> Board::getPieceMoves(int idx)
{
    return moveset[idx];
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
    // else if (pieceType == Piece::KNIGHT)
    // {
    // }
    // else if (pieceType == Piece::PAWN)
    // {
    // }
    // else if (pieceType == Piece::KING)
    // {
    // }

    return {};
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