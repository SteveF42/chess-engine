#ifndef TranspositionTable_H
#define TranspositionTable_H
#include "Board.hpp"

struct TTentry
{
    TTentry()
    {
        this->hashKey = 0;
        this->depth = 0;
        this->flag = 0;
        this->eval = 0;
        this->bestMove = Move();
    }
    TTentry(uint64_t hashKey, int depth, int flag, int eval, Move move)
    {
        this->hashKey = hashKey;
        this->depth = depth;
        this->flag = flag;
        this->eval = eval;
        this->bestMove = move;
    }

    uint64_t hashKey;
    int depth;
    int flag;
    int eval;
    Move bestMove;
};

class TranspositionTable
{
private:
    const static uint64_t TABLESIZE = 0x400000;

    TTentry entries[TABLESIZE];
    Board *board;

public:
    static int visits;
    TranspositionTable(Board *board)
    {
        this->board = board;
        clearTable();
    }
    const int EXACT = 0;
    const int UPPER = 1;
    const int LOWER = 2;
    const int FAILED = -1;
    void clearTable();
    int readTable(int alpha, int beta, int depth);
    void writeTable(int depth, int eval, int flag, Move bestMove);
    Move getMove();
};

#endif