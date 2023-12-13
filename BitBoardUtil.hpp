#ifndef BitBoardUtil_H
#define BitBoardUtil_H
#include <cinttypes>

class BitBoardUtil
{

private:
    const static uint64_t FileA = 0x101010101010101;
    const static uint64_t notAFile = ~FileA;
    const static uint64_t notHFile = ~(FileA << 7);

    const static uint64_t deBruijn64 = 0x37E84A99DAE458F;
    inline const static int deBruijnTable[] = {
        0, 1, 17, 2, 18, 50, 3, 57,
        47, 19, 22, 51, 29, 4, 33, 58,
        15, 48, 20, 27, 25, 23, 52, 41,
        54, 30, 38, 5, 43, 34, 59, 8,
        63, 16, 49, 56, 46, 21, 28, 32,
        14, 26, 24, 40, 53, 37, 42, 7,
        62, 55, 45, 31, 13, 39, 36, 6,
        61, 44, 12, 35, 60, 11, 10, 9};

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

    // yeah idk some dud said this works on the internet
    static int PopLSB(uint64_t &b)
    {
        int i = deBruijnTable[((uint64_t)((int64_t)b & -(int64_t)b) * deBruijn64) >> 58];
        b = b & (b - 1);
        return i;
    }

    static uint64_t pawnAttacks(uint64_t pawnBitBoard, bool isWhite)
    {
        if(isWhite){
            return ((pawnBitBoard >> 9) & notHFile) | ((pawnBitBoard >> 7) & notAFile); 
        }
        return ((pawnBitBoard << 7) & notHFile) | ((pawnBitBoard << 9) & notAFile);
    }
};

#endif