#ifndef AI_H
#define AI_H

#include "Board.hpp"

class AI
{
private:
    const static int pawnValue = 100;
    const static int knightValue = 300;
    const static int bishopValue = 300;
    const static int rookValue = 500;
    const static int queenValue = 900;
    const static uint16_t INFINITY = UINT16_MAX;

    static int generateEval(Board position);
    static std::vector<Move> orderMoves(std::map<int, std::vector<Move>> moveTable, Board &position);
    static int getPieceValue(int piece);

public:
    static Move bestMove;
    static int minimax(Board position, int depth = 4, int alpha = 0, int beta = 0);
};

#endif