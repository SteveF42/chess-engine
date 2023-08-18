#include "PreComputedMoveData.hpp"
#include <math.h>
#include <vector>
#include "PieceList.hpp"

PrecomputedMoveData::PrecomputedMoveData()
{
    // Calculate knight jumps and available squares for each square on the board.
    // See comments by variable definitions for more info.
    int allKnightJumps[] = {15, 17, -17, -15, 10, -6, 6, -10};

    for (int squareIndex = 0; squareIndex < 64; squareIndex++)
    {

        int rank = squareIndex / 8;
        int file = squareIndex % 8;

        int south = 7 - rank;
        int north = rank;
        int west = file;
        int east = 7 - file;


        // yes I know this looks awful, I hate C++
        numSquaresToEdge[squareIndex][0] = east;
        numSquaresToEdge[squareIndex][1] = west;
        numSquaresToEdge[squareIndex][2] = south;
        numSquaresToEdge[squareIndex][3] = north;
        numSquaresToEdge[squareIndex][4] = std::min(south, west);
        numSquaresToEdge[squareIndex][5] = std::min(north, east);
        numSquaresToEdge[squareIndex][6] = std::min(east, south);
        numSquaresToEdge[squareIndex][7] = std::min(north, west);

        // Calculate all squares knight can jump to from current square
        std::vector<uint8_t> legalKnightJumps;
        uint64_t knightBitboard = 0;
        for (int i = 0; i < 8; i++)
        {
            int knightJumpSquare = squareIndex + allKnightJumps[i];
            if (knightJumpSquare >= 0 && knightJumpSquare < 64)
            {
                int knightSquareY = knightJumpSquare / 8;
                int knightSquareX = knightJumpSquare - knightSquareY * 8;
                // Ensure knight has moved max of 2 squares on x/y axis (to reject indices that have wrapped around side of board)
                int maxCoordMoveDst = std::max(std::abs(file - knightSquareX), std::abs(rank - knightSquareY));
                if (maxCoordMoveDst == 2)
                {
                    legalKnightJumps.push_back((uint8_t)knightJumpSquare);
                    knightBitboard |= (uint64_t)1 << knightJumpSquare;
                }
            }
        }
        knightMoves[squareIndex] = legalKnightJumps;
        knightAttackBitboards[squareIndex] = knightBitboard;

        // Calculate all squares king can move to from current square (not including castling)
        std::vector<uint8_t> legalKingMoves;
        for (int i = 0; i < 8; i++)
        {
            int kingMoveSquare = squareIndex + directionOffsets[i];
            if (kingMoveSquare >= 0 && kingMoveSquare < 64)
            {
                int kingSquareY = kingMoveSquare / 8;
                int kingSquareX = kingMoveSquare % 8;
                // Ensure king has moved max of 1 square on x/y axis (to reject indices that have wrapped around side of board)
                int maxCoordMoveDst = std::max(std::abs(file - kingSquareX), std::abs(rank - kingSquareY));
                if (maxCoordMoveDst == 1)
                {
                    legalKingMoves.push_back((uint8_t)kingMoveSquare);
                    kingAttackBitboards[squareIndex] |= (uint64_t)1 << kingMoveSquare;
                }
            }
        }
        kingMoves[squareIndex] = legalKingMoves;

        // Calculate legal pawn captures for white and black
        if (file > 0)
        {
            if (rank < 7)
            {
                pawnAttackBitboards[squareIndex][PieceList::whiteIndex] = (uint64_t)1 << (squareIndex - 9);
            }
            if (rank > 0)
            {
                pawnAttackBitboards[squareIndex][PieceList::blackIndex] = (uint64_t)1 << (squareIndex + 7);
            }
        }
        if (file < 7)
        {
            if (rank < 7)
            {
                pawnAttackBitboards[squareIndex][PieceList::whiteIndex] = (uint64_t)1 << (squareIndex - 7);
            }
            if (rank > 0)
            {
                pawnAttackBitboards[squareIndex][PieceList::blackIndex] = (uint64_t)1 << (squareIndex + 9);
            }
        }

        // Rook moves
        for (int directionIndex = 0; directionIndex < 4; directionIndex++)
        {
            int currentDirOffset = directionOffsets[directionIndex];
            for (int n = 0; n < numSquaresToEdge[squareIndex][directionIndex]; n++)
            {
                int targetSquare = squareIndex + currentDirOffset * (n + 1);
                rookMoves[squareIndex] |= (uint64_t)1 << targetSquare;
            }
        }
        // Bishop moves
        for (int directionIndex = 4; directionIndex < 8; directionIndex++)
        {
            int currentDirOffset = directionOffsets[directionIndex];
            for (int n = 0; n < numSquaresToEdge[squareIndex][directionIndex]; n++)
            {
                int targetSquare = squareIndex + currentDirOffset * (n + 1);
                bishopMoves[squareIndex] |= (uint64_t)1 << targetSquare;
            }
        }
        queenMoves[squareIndex] = rookMoves[squareIndex] | bishopMoves[squareIndex];
    }

    for (int i = 0; i < 127; i++)
    {
        int offset = i - 63;
        int absOffset = std::abs(offset);
        int absDir = 1;
        if (absOffset % 9 == 0)
        {
            absDir = 9;
        }
        else if (absOffset % 8 == 0)
        {
            absDir = 8;
        }
        else if (absOffset % 7 == 0)
        {
            absDir = 7;
        }

        directionLookup[i] = absDir * signbit(offset);
    }

    // Distance lookup
    for (int squareA = 0; squareA < 64; squareA++)
    {
        int rankA = squareA / 8;
        int fileA = squareA % 8;
        int fileDstFromCentre = std::max(3 - fileA, fileA - 4);
        int rankDstFromCentre = std::max(3 - rankA, rankA - 4);
        centreManhattanDistance[squareA] = fileDstFromCentre + rankDstFromCentre;

        for (int squareB = 0; squareB < 64; squareB++)
        {
            int rankB = squareA / 8;
            int fileB = squareA % 8;
            int rankDistance = std::abs(rankA - rankB);
            int fileDistance = std::abs(fileA - fileB);
            orthogonalDistance[squareA][squareB] = fileDistance + rankDistance;
            kingDistance[squareA][squareB] = std::max(fileDistance, rankDistance);
        }
    }
}