#ifndef TranspositionTable_H
#define TranspositionTable_H
#include "Board.hpp"

struct TTentry
{
    uint64_t hashKey;
    int depth;
    int flag;
    int eval;
};

class TranspositionTable
{
private:
    const static int tableSize = 0x400000;
    const int exact = 0;
    const int upper = 1;
    const int lower = 2;
    const int failed = -1;
    TTentry *entries[tableSize];

public:
    TranspositionTable(Board *position);
    int readTable(int alpha, int beta, int depth);
    int writeTable();
};

#endif