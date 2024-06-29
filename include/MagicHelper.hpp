#ifndef MagicHelper_H
#define MagicHelper_H
#include <cinttypes>
#include "BitBoardUtil.hpp"

struct Coord
{
    int fileIndex;
    int rankIndex;
    Coord(int fileIndex, int rankIndex)
    {
        this->fileIndex = fileIndex;
        this->rankIndex = rankIndex;
    }
    Coord(int squareIndex)
    {
        this->fileIndex = squareIndex % 8;
        this->rankIndex = squareIndex / 8;
    }
    bool isValidSquare() { return (fileIndex >= 0 && fileIndex < 8 && rankIndex >= 0 && rankIndex < 8); }
    int squareIndex() { return (rankIndex * 8) + fileIndex; }
    Coord operator*(const int &other) { return Coord(fileIndex * other, rankIndex * other); }
    Coord operator+(const Coord &other) { return Coord(fileIndex + other.fileIndex, rankIndex + other.rankIndex); }
    Coord operator-(const Coord &other) { return Coord(fileIndex - other.fileIndex, rankIndex - other.rankIndex); }
};

class MagicHelper
{
private:
    Coord rookDirections[4] = {Coord(-1, 0), Coord(1, 0), Coord(0, 1), Coord(0, -1)};
    Coord bishopDirections[4] = {Coord(-1, 1), Coord(1, 1), Coord(1, -1), Coord(-1, -1)};

public:
    uint64_t *createAllBlockerBitboards(uint64_t movementMask);
    uint64_t createMovementMask(int squareIndex, bool ortho);
    uint64_t legalMoveBitboardFromBlockers(int startSquare, uint64_t blockerBitboard, bool ortho);
};

#endif