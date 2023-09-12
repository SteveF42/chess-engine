#include "Magic.hpp"

uint64_t Magic::getrookAttacks(int square, uint64_t blockers)
{
    // calculate magic index
    blockers &= rookMask[square];
    blockers *= PrecomputedMagics::rookMagics[square];
    blockers >>= 64 - rookReleventBits[square];

    // return rellevant attacks
    return rookAttacks[square][blockers];
}
uint64_t Magic::getBishopAttacks(int square, uint64_t blockers)
{
    blockers &= bishopMask[square];
    blockers *= PrecomputedMagics::bishopMagics[square];
    blockers >>= 64 - bishopReleventBits[square];
    return bishopAttacks[square][blockers];
}
int Magic::countBits(uint64_t bitboard)
{
    // bit count
    int count = 0;

    // pop bits untill bitboard is empty
    while (bitboard)
    {
        // increment count
        count++;

        // consecutively reset least significant 1st bit
        bitboard &= bitboard - 1;
    }

    // return bit count
    return count;
}

uint64_t Magic::setOccupancy(int index, int bits_in_mask, uint64_t attack_mask)
{
    // occupancy map
    uint64_t occupancy = 0ULL;

    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++)
    {
        // pop LS1B in attack map
        int square = BitBoardUtil::PopLSB(attack_mask);

        // make sure occupancy is on board
        if (index & (1 << count))
            // populate occupancy map
            occupancy |= (1ULL << square);
    }

    // return occupancy map
    return occupancy;
}
uint64_t Magic::bishopAttacksOnTheFly(int square, uint64_t block)
{
    // attack bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f)))
            break;
    }

    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f)))
            break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f)))
            break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f)))
            break;
    }

    // return attack map for bishop on a given square
    return attacks;
}

uint64_t Magic::rookAttacksOnTheFly(int square, uint64_t block)
{
    // attacks bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf)))
            break;
    }

    for (r = tr - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf)))
            break;
    }

    for (f = tf + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f)))
            break;
    }

    for (f = tf - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f)))
            break;
    }

    // return attack map for bishop on a given square
    return attacks;
}

void Magic::initSliderAttacks(int isBishop)
{
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook masks
        bishopMask[square] = maskBishopAttacks(square);
        rookMask[square] = maskRookAttacks(square);

        // init current mask
        uint64_t mask = isBishop ? bishopMask[square] : rookMask[square];

        // count attack mask bits
        int bit_count = countBits(mask);

        // occupancy variations count
        int occupancy_variations = 1 << bit_count;

        // loop over occupancy variations
        for (int count = 0; count < occupancy_variations; count++)
        {
            // bishop
            if (isBishop)
            {
                // init occupancies, magic index & attacks
                uint64_t occupancy = setOccupancy(count, bit_count, mask);
                uint64_t magic_index = occupancy * PrecomputedMagics::bishopMagics[square] >> 64 - bishopReleventBits[square];
                bishopAttacks[square][magic_index] = bishopAttacksOnTheFly(square, occupancy);
            }

            // rook
            else
            {
                // init occupancies, magic index & attacks
                uint64_t occupancy = setOccupancy(count, bit_count, mask);
                uint64_t magic_index = occupancy * PrecomputedMagics::rookMagics[square] >> 64 - rookReleventBits[square];
                rookAttacks[square][magic_index] = rookAttacksOnTheFly(square, occupancy);
            }
        }
    }
}

uint64_t Magic::maskBishopAttacks(int square)
{
    // attack bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
        attacks |= (1ULL << (r * 8 + f));

    // return attack map for bishop on a given square
    return attacks;
}

uint64_t Magic::maskRookAttacks(int square)
{
    // attacks bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1; r <= 6; r++)
        attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--)
        attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++)
        attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--)
        attacks |= (1ULL << (tr * 8 + f));

    // return attack map for bishop on a given square
    return attacks;
}