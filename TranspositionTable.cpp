#include "TranspositionTable.hpp"

int TranspositionTable::visits;

void TranspositionTable::clearTable()
{
    for (int i = 0; i < TABLESIZE; i++)
    {
        entries[i].hashKey = 0;
        entries[i].depth = 0;
        entries[i].flag = 0;
        entries[i].eval = 0;
    }
}

int TranspositionTable::readTable(int alpha, int beta, int depth)
{
    TTentry &entry = entries[board->getZobristKey() % TABLESIZE];

    // ensure that the entry is the correct one were looking for
    if (entry.hashKey == board->getZobristKey())
    {
        if (entry.depth >= depth)
        {
            if (entry.flag == EXACT)
            {
                return entry.eval;
            }
            if (entry.flag == UPPER && entry.eval <= alpha)
            {
                return alpha;
            }
            if (entry.flag == LOWER && entry.eval >= beta)
            {
                return beta;
            }
        }
    }

    return FAILED;
}

void TranspositionTable::writeTable(int depth, int eval, int flag, Move bestMove)
{
    TTentry &entry = entries[board->getZobristKey() % TABLESIZE];
    entry.hashKey = board->getZobristKey();
    entry.bestMove = bestMove;
    entry.depth = depth;
    entry.eval = eval;
    entry.flag = flag;
    visits++;
}

Move TranspositionTable::getMove()
{
    return entries[board->getZobristKey() % TABLESIZE].bestMove;
}