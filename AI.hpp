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
    const static int MAXDEPTH = 4;
    const static int INFINITY = INT_MAX;

    static int generateEval(Board position);
    static std::vector<Move> orderMoves(std::map<int, std::vector<Move>> &moveTable, Board &position);
    static std::vector<Move> orderMoves(std::vector<Move> &moveTable, Board &position);
    static std::vector<Move> generateCaptures(Board &position);
    static int getPieceValue(int piece);
    static int searchCaptures(Board &position, int alpha, int beta);
    static void sortMoves(std::vector<Move> &moves, std::vector<int> &weights);

public:
    static Move bestMove;
    static int positions;
    static int minimax(Board &position, int depth = MAXDEPTH, int alpha = -INFINITY, int beta = INFINITY);
    static long moveGenerationTest(int depth, Board &position);
};

#endif