#ifndef BitBoardUtil_H
#define BitBoardUtil_H
#include <cinttypes>

class BitBoardUtil
{
public:
    static void setBit(uint64_t &board, int square)
    {
        board |= (uint64_t)1 << square;
    }

    static void flipBit(uint64_t &board, int square)
    {
        board ^= (uint64_t)1 << square;
    }
    static bool containsBit(uint64_t bitboard, int square)
    {
        return ((bitboard >> square) & 1) != 0;
    }
    static void flipBits(uint64_t &board, int squareA, int squareB)
    {
        board ^= ((uint64_t)1 << squareA | (uint64_t)1 << squareB);
    }
};

#endif