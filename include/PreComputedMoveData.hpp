#ifndef PreComputedMoveData_H
#define PreComputedMoveData_H

#include <vector>
#include <stdint.h>

class PrecomputedMoveData
{
public:
    uint64_t alignMask[64][64];

    // First 4 are orthogonal, last 4 are diagonals (E, W, S, N, SW, NE, SE, NW)
    const int directionOffsets[8] = {1, -1, 8, -8, 7, -7, 9, -9};

    int numSquaresToEdge[64][8];

    std::vector<uint8_t> knightMoves[64];
    std::vector<uint8_t> kingMoves[64];

    // Pawn attack directions
    uint8_t pawnAttackDirections[2][2] = {
        {7, 5},
        {4, 6}};

    int directionLookup[127];

    uint64_t kingAttackBitboards[64];
    uint64_t knightAttackBitboards[64];
    uint64_t pawnAttackBitboards[64][2];

    uint64_t rookMoves[64];
    uint64_t bishopMoves[64];
    uint64_t queenMoves[64];

    uint64_t whitePassedPawnMask[64];
    uint64_t blackPassedPawnMask[64];

    uint64_t adjacentFileMask[8];

    // Aka manhattan distance (answers how many moves for a rook to get from square a to square b)
    int orthogonalDistance[64][64];
    // Aka chebyshev distance (answers how many moves for a king to get from square a to square b)
    int kingDistance[64][64];
    int centreManhattanDistance[64];

    int numRookMovesToReachSquare(int startSquare, int targetSquare)
    {
        return orthogonalDistance[startSquare][targetSquare];
    }

    int numKingMovesToReachSquare(int startSquare, int targetSquare)
    {
        return kingDistance[startSquare][targetSquare];
    }

    // Initialize lookup data
    PrecomputedMoveData();
};

#endif