#include "MagicHelper.hpp"
#include <vector>

uint64_t *MagicHelper::createAllBlockerBitboards(uint64_t movementMask)
{
    // Create a list of the indices of the bits that are set in the movement mask
    std::vector<int> moveSquareIndices;
    for (int i = 0; i < 64; i++)
    {
        if (((movementMask >> i) & 1) == 1)
        {
            moveSquareIndices.push_back(i);
        }
    }

    // Calculate total number of different bitboards (one for each possible arrangement of pieces)
    int numPatterns = 1 << moveSquareIndices.size(); // 2^n
    uint64_t *blockerBitboards = new uint64_t[numPatterns];

    // Create all bitboards
    for (int patternIndex = 0; patternIndex < numPatterns; patternIndex++)
    {
        for (int bitIndex = 0; bitIndex < moveSquareIndices.size(); bitIndex++)
        {
            int bit = (patternIndex >> bitIndex) & 1;
            blockerBitboards[patternIndex] |= (uint64_t)bit << moveSquareIndices[bitIndex];
        }
    }

    return blockerBitboards;
}

uint64_t MagicHelper::createMovementMask(int squareIndex, bool ortho)
{
    uint64_t mask = 0;
    Coord *directions = (ortho ? rookDirections : bishopDirections);
    Coord startCoord = Coord(squareIndex);

    for (int i = 0; i < 4; i++)
    {
        Coord &dir = directions[i];
        for (int dst = 1; dst < 8; dst++)
        {
            Coord coord = startCoord + dir * dst;
            Coord nextCoord = startCoord + dir * (dst + 1);

            if (nextCoord.isValidSquare())
            {
                BitBoardUtil::setBit(mask, coord.squareIndex());
            }
            else
            {
                break;
            }
        }
    }
    return mask;
}
uint64_t MagicHelper::legalMoveBitboardFromBlockers(int startSquare, uint64_t blockerBitboard, bool ortho)
{
    uint64_t bitboard = 0;

    Coord *directions = (ortho ? rookDirections : bishopDirections);
    Coord startCoord(startSquare);

    for (int i = 0; i < 4; i++)
    {
        Coord &dir = directions[i];
        for (int dst = 1; dst < 8; dst++)
        {
            Coord coord = startCoord + dir * dst;

            if (coord.isValidSquare())
            {
                BitBoardUtil::setBit(bitboard, coord.squareIndex());
                if (BitBoardUtil::containsBit(blockerBitboard, coord.squareIndex()))
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    return bitboard;
}