#ifndef AI_H
#define AI_H
#include <stdint.h>
#include "Board.hpp"
#include "TranspositionTable.hpp"

class AI
{
private:
    const int million = 1000000;

    const static int pawnValue = 100;
    const static int knightValue = 300;
    const static int bishopValue = 350;
    const static int rookValue = 500;
    const static int queenValue = 900;
    const static int MAXDEPTH = 4;
    const static int INFINITE = 99999999;
    const static int mateScore = 100000;
    int pawnBonuses[7] = {0, 120, 80, 50, 30, 15, 15};
    int isolatedPawnCountPenalty[9] = {0, -10, -25, -50, -75, -75, -75, -75, -75};

    const static int TIMEOUT_MILISECONDS = 3201; // 3 seconds
    Board *position;
    bool timeout = true;
    bool playBookMove = false;
    Move bestMoveThisIteration;
    clock_t timeoutStart;
    TranspositionTable *tt;
    int numExtensions;
    std::vector<Move> killerMoves;
    int qPositions;

    int evaluate();
    float endgamePhaseWeight(int materialCountWithoutPawns);
    int evaluatePieceSquareTables(int colourIndex, float endgamePhaseWeight);
    int evaluatePieceSquareTable(const int table[], std::vector<Piece *> &pieceList, bool isWhite);
    int mopUpEval(int friendlyIndex, int opponentIndex, int myMaterial, int opponentMaterial, float endgameWeight);
    int countMaterial(int pieceIndex);
    void orderMoves(std::vector<Move> &moveTable, bool useTT = true);
    int getPieceValue(int piece);
    int searchCaptures(int alpha, int beta);
    int minimax(int depth = MAXDEPTH, int depthFromRoot = 0, int alpha = -INFINITE, int beta = INFINITE);
    float getMaterialInfo(int colorIndex);
    void iterativeDeepening();
    int calculateExtension(const Move &move);
    int evaluatePawns(int friendlyColorIndex);

public:
    AI(Board *position)
    {
        this->position = position;
        tt = new TranspositionTable(position);
    }
    Move bestMove;
    int positions;
    int transPositions;
    void generateBestMove();
    long moveGenerationTest(int depth);
    bool getTimeout() { return timeout; }
};

#endif
