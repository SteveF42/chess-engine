#ifndef Magic_H
#define Magic_H

#include "cinttypes"
#include "PrecomputedMagics.hpp"
#include "MagicHelper.hpp"
#include "array"
// Helper class for magic bitboards.
// This is a technique where bishop and rook moves are precomputed
// for any configuration of origin square and blocking pieces.
class Magic
{
    // Rook and bishop mask bitboards for each origin square.
    // A mask is simply the legal moves available to the piece from the origin square
    // (on an empty board), except that the moves stop 1 square before the edge of the board.
    MagicHelper magicHelper;

    uint64_t rookMask[64];

    uint64_t bishopMask[64];

    uint64_t rookAttacks[64][4096];

    uint64_t bishopAttacks[64][512];

    int rookReleventBits[64] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12};

    // bishop rellevant occupancy bits
    int bishopReleventBits[64] = {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6};

public:
    uint64_t getrookAttacks(int square, uint64_t blockers);

    uint64_t getBishopAttacks(int square, uint64_t blockers);

    int countBits(uint64_t bitboard);

    uint64_t setOccupancy(int index, int bits_in_mask, uint64_t attack_mask);
    uint64_t bishopAttacksOnTheFly(int square, uint64_t block);
    uint64_t rookAttacksOnTheFly(int square, uint64_t block);
    void initSliderAttacks(int isBishop);
    uint64_t maskBishopAttacks(int square);

    // mask rook attacks
    uint64_t maskRookAttacks(int square);

    Magic()
    {
        initSliderAttacks(true);
        initSliderAttacks(false);
    }
};

#endif