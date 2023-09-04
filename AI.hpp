#ifndef AI_H
#define AI_H
#include <stdint.h>
#include "Board.hpp"

class AI
{
private:
    const static int pawnValue = 100;
    const static int knightValue = 300;
    const static int bishopValue = 350;
    const static int rookValue = 500;
    const static int queenValue = 900;
    const static int MAXDEPTH = 4;
    const static int INFINITE = 99999999;
    const static int mateScore = 100000;

    static Board *position;

    static int evaluate();
    static float endgamePhaseWeight(int materialCountWithoutPawns);
    static int evaluatePieceSquareTables(int colourIndex, float endgamePhaseWeight);
    static int evaluatePieceSquareTable(const int table[], std::vector<Piece *> &pieceList, bool isWhite);
    static int mopUpEval(int friendlyIndex, int opponentIndex, int myMaterial, int opponentMaterial, float endgameWeight);
    static int countMaterial(int pieceIndex);
    static void orderMoves(std::vector<Move> &moveTable);
    static int getPieceValue(int piece);
    static int searchCaptures(int alpha, int beta);
    static void sortMoves(std::vector<Move> &moves, int* weights);
    static int minimax(int depth = MAXDEPTH, int depthFromRoot = 0, int alpha = -INFINITE, int beta = INFINITE);

public:
    static Move bestMove;
    static int positions;
    static void generateBestMove(Board *ref);
    static long moveGenerationTest(int depth, Board *position);
};

#endif