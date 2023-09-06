#include "AI.hpp"
#include "PieceSquareTable.hpp"
#include <chrono>
#include <bits/stdc++.h>
#include <thread>

Move AI::bestMove;
int AI::positions;
Board *AI::position;
bool AI::timeout;
Move AI::bestMoveThisIteration;
clock_t AI::timeoutStart;

int partition(std::vector<Move> &moves, int *weights, int low, int high)
{
    int pivotScore = weights[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++)
    {
        if (weights[j] > pivotScore)
        {
            i++;
            Move temp = moves[i];
            moves[i] = moves[j];
            moves[j] = temp;

            int temp2 = weights[i];
            weights[i] = weights[j];
            weights[j] = temp2;
        }
    }
    Move temp = moves[i + 1];
    moves[i + 1] = moves[high];
    moves[high] = temp;

    int temp2 = weights[i + 1];
    weights[i + 1] = weights[high];
    weights[high] = temp2;

    return i + 1;
}
void quickSort(std::vector<Move> &moves, int *scores, int low, int high)
{
    if (low < high)
    {
        int pivot = partition(moves, scores, low, high);
        quickSort(moves, scores, low, pivot - 1);
        quickSort(moves, scores, pivot + 1, high);
    }
}

void AI::generateBestMove(Board *ref)
{

    position = ref;
    bestMove = Move();
    positions = 0;
    // std::thread searchThread(minimax);

    clock_t start, stop;
    // minimax();
    iterativeDeepening();
    std::cout << "Nodes searched: " << positions;
}
void AI::iterativeDeepening()
{
    timeout = false;
    timeoutStart = clock();
    for (int currentDepth = 0; currentDepth < 128; currentDepth++)
    {
        if (currentDepth > 0)
        {
            bestMoveThisIteration = bestMove;
            std::cout << "Completed search with depth: " << currentDepth << '\n';
        }
        minimax(currentDepth);
        if (timeout)
        {
            bestMove = bestMoveThisIteration;
            return;
        }
    }
}
long AI::moveGenerationTest(int depth, Board *position)
{
    if (depth == 0)
        return 1;

    auto moves = position->moveGeneration.generateMoves(position);
    long numPositions = 0;

    for (auto move : moves)
    {
        position->makeMove(move);
        numPositions += moveGenerationTest(depth - 1, position);
        bestMove = move;
        position->unmakeMove();
    }
    return numPositions;
}

int AI::minimax(int depth /*= MAXDEPTH*/, int depthFromRoot, int alpha /*=-INFINITY*/, int beta /*=INFINITY*/)
{
    if (clock() - timeoutStart > TIMEOUT_MILISECONDS)
    {
        timeout = true;
        return alpha;
    }
    if (depthFromRoot > 0)
    {
        alpha = std::max(alpha, (-mateScore) + depthFromRoot);
        beta = std::min(beta, mateScore - depthFromRoot);
        if (alpha >= beta)
        {
            return alpha;
        }
    }

    if (depth == 0)
        // return evaluate();
        return searchCaptures(alpha, beta);

    auto moves = position->moveGeneration.generateMoves(position);
    orderMoves(moves);

    if (moves.size() == 0)
    {
        // being in check is bad and if theres no moves then a stalemate has occurred
        if (position->moveGeneration.isCheck())
            return -(mateScore - (depthFromRoot));
        else
            return 0;
    }

    for (const auto &move : moves)
    {
        position->makeMove(move);
        int eval = -minimax(depth - 1, depthFromRoot + 1, -beta, -alpha);
        position->unmakeMove();

        positions++;

        // opponent had a better move so don't use it
        if (eval >= beta)
        {
            return beta;
        }
        if (eval > alpha)
        {
            alpha = eval;
            if (depthFromRoot == 0)
            {
                bestMove = move;
            }
        }
    }

    return alpha;
}

int AI::searchCaptures(int alpha, int beta)
{
    int eval = evaluate();

    if (eval >= beta)
        return beta;
    if (eval > alpha)
    {
        alpha = eval;
    }

    auto captures = position->moveGeneration.generateMoves(position, false);
    orderMoves(captures);

    for (auto &capture : captures)
    {
        position->makeMove(capture);
        eval = -searchCaptures(-beta, -alpha);
        position->unmakeMove();
        AI::positions++;

        if (eval >= beta)
            return beta;
        if (eval > alpha)
        {
            alpha = eval;
        }
    }
    return alpha;
}

int AI::evaluate()
{
    int whiteEval = 0;
    int blackEval = 0;

    int whiteMaterial = countMaterial(PieceList::whiteIndex);
    int blackMaterial = countMaterial(PieceList::blackIndex);

    int whiteMaterialWithNoPawns = whiteMaterial - position->pieceList.getPieces(PieceList::whiteIndex)[PieceList::pawnIndex].size() * pawnValue;
    int blackMaterialWithNoPawns = whiteMaterial - position->pieceList.getPieces(PieceList::blackIndex)[PieceList::pawnIndex].size() * pawnValue;

    float whiteEndgamePhaseWeight = getMaterialInfo(PieceList::whiteIndex);
    float blackEndgamePhaseWeight = getMaterialInfo(PieceList::blackIndex);

    whiteEval += whiteMaterial;
    blackEval += blackMaterial;

    whiteEval += evaluatePieceSquareTables(PieceList::whiteIndex, blackEndgamePhaseWeight);
    blackEval += evaluatePieceSquareTables(PieceList::blackIndex, whiteEndgamePhaseWeight);

    // mess with this some other day
    whiteEval += mopUpEval(PieceList::whiteIndex, PieceList::blackIndex, whiteMaterial, blackMaterial, blackEndgamePhaseWeight);
    blackEval += mopUpEval(PieceList::blackIndex, PieceList::whiteIndex, blackMaterial, whiteMaterial, whiteEndgamePhaseWeight);

    int eval = whiteEval - blackEval;
    int perspective = Board::whiteToMove ? 1 : -1;

    return eval * perspective;
}

int AI::mopUpEval(int friendlyIndex, int opponentIndex, int myMaterial, int opponentMaterial, float endgameWeight)
{
    int mopUpScore = 0;
    if (myMaterial > opponentMaterial + pawnValue * 2 && endgameWeight > 0)
    {

        int friendlyKingSquare = position->getKing(friendlyIndex)->getPiecePosition();
        int opponentKingSquare = position->getKing(opponentIndex)->getPiecePosition();
        mopUpScore += position->moveGeneration.preComputedMoveData.centreManhattanDistance[opponentKingSquare] * 10;
        // use ortho dst to promote direct opposition
        mopUpScore += (14 - position->moveGeneration.preComputedMoveData.numRookMovesToReachSquare(friendlyKingSquare, opponentKingSquare)) * 4;

        return (int)(mopUpScore * endgameWeight);
    }
    return 0;
}

int AI::evaluatePieceSquareTables(int colorIndex, float endgamePhaseWeight)
{

    int value = 0;
    bool isWhite = colorIndex == PieceList::whiteIndex;
    auto pieces = position->pieceList.getPieces(colorIndex);
    Piece *king = position->getKing(colorIndex);
    value += evaluatePieceSquareTable(PieceSquareTable::rooks, pieces[PieceList::rookIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::knights, pieces[PieceList::knightIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::bishops, pieces[PieceList::bishopIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::queens, pieces[PieceList::queenIndex], isWhite);

    int pawnEarly = evaluatePieceSquareTable(PieceSquareTable::pawns, pieces[PieceList::pawnIndex], isWhite);
    int pawnEnd = evaluatePieceSquareTable(PieceSquareTable::pawnsEnd, pieces[PieceList::pawnIndex], isWhite);

    value += (int)(pawnEarly * (1 - endgamePhaseWeight));
    value += (int)(pawnEnd * endgamePhaseWeight);

    int kingEarlyPhase = PieceSquareTable::read(PieceSquareTable::kingMiddle, king->getPiecePosition(), isWhite);
    int kingLatePhase = PieceSquareTable::read(PieceSquareTable::kingEnd, king->getPiecePosition(), isWhite);

    value += (int)(kingEarlyPhase * (1 - endgamePhaseWeight));
    value += (int)(kingLatePhase * endgamePhaseWeight);

    return value;
}

int AI::evaluatePieceSquareTable(const int table[], std::vector<Piece *> &pieceList, bool isWhite)
{
    int value = 0;
    for (int i = 0; i < pieceList.size(); i++)
    {
        value += PieceSquareTable::read(table, pieceList[i]->getPiecePosition(), isWhite);
    }
    return value;
}

float AI::endgamePhaseWeight(int materialWithNoPawns)
{
    const float endgameMaterialStart = rookValue * 2 + bishopValue + knightValue;
    const float multiplier = 1 / endgameMaterialStart;
    return 1 - std::min(1.0f, (materialWithNoPawns * multiplier));
}

int AI::countMaterial(int pieceIndex)
{
    auto pieces = position->pieceList.getPieces(pieceIndex);
    int material = 0;
    material += pieces[PieceList::pawnIndex].size() * pawnValue;
    material += pieces[PieceList::knightIndex].size() * knightValue;
    material += pieces[PieceList::rookIndex].size() * rookValue;
    material += pieces[PieceList::bishopIndex].size() * bishopValue;
    material += pieces[PieceList::queenIndex].size() * queenValue;
    return material;
}

void AI::orderMoves(std::vector<Move> &moveTable)
{
    int scores[moveTable.size()];
    int i = 0;

    for (const auto &move : moveTable)
    {
        int moveScoreGuess = 0;
        int movePieceType = position->getBoard()[move.start]->getPiece()->getPieceType();
        if (!position->getBoard()[move.target]->hasNullPiece())
        {
            int targetPiece = position->getBoard()[move.target]->getPiece()->getPieceType();
            moveScoreGuess += 10 * (getPieceValue(targetPiece) - getPieceValue(movePieceType));
        }
        if (move.pawnPromotion)
        {
            moveScoreGuess += 500;
        }
        if (move.isCastle)
        {
            moveScoreGuess += 1300;
        }
        if (position->moveGeneration.containsSquareInPawnAttackMap(move.target))
        {
            moveScoreGuess -= 350;
        }
        scores[i++] = moveScoreGuess;
    }
    quickSort(moveTable, scores, 0, moveTable.size() - 1);
    // sortMoves(moveTable,scores);
}

void AI::sortMoves(std::vector<Move> &moves, int *weights)
{
    if (moves.size() == 0)
    {
        return;
    }

    for (int i = 0; i < moves.size() - 1; i++)
    {
        for (int j = i + 1; j > 0; j--)
        {
            int swampIndex = j - 1;
            if (weights[swampIndex] < weights[j])
            {
                Move &temp = moves[j];
                moves[j] = moves[swampIndex];
                moves[swampIndex] = temp;

                int temp2 = weights[j];
                weights[j] = weights[swampIndex];
                weights[swampIndex] = temp2;
            }
        }
    }
}

int AI::getMaterialInfo(int colorIndex)
{
    auto pieces = position->pieceList.getPieces(colorIndex);
    int numPawns = pieces[PieceList::pawnIndex].size();
    int numKnights = pieces[PieceList::knightIndex].size();
    int numBishops = pieces[PieceList::bishopIndex].size();
    int numRooks = pieces[PieceList::rookIndex].size();
    int numQueens = pieces[PieceList::queenIndex].size();

    int numMajors = numRooks + numQueens;
    int numMinors = numBishops + numKnights;

    int materialScore = 0;
    materialScore += numPawns * pawnValue;
    materialScore += numKnights * knightValue;
    materialScore += numBishops * bishopValue;
    materialScore += numRooks * rookValue;
    materialScore += numQueens * queenValue;

    // Endgame Transition (0->1)
    const int queenEndgameWeight = 45;
    const int rookEndgameWeight = 20;
    const int bishopEndgameWeight = 10;
    const int knightEndgameWeight = 10;

    const int endgameStartWeight = (2 * rookEndgameWeight) + (2 * bishopEndgameWeight) + (2 * knightEndgameWeight) + (queenEndgameWeight);
    int endgameWeightSum = (numQueens * queenEndgameWeight) + (numRooks * rookEndgameWeight) + (numBishops * bishopEndgameWeight) + (numKnights * knightEndgameWeight);
    float endgameT = 1 - std::min(1.0f, (endgameWeightSum / (float)endgameStartWeight));

    return endgameT;
}

int AI::getPieceValue(int piece)
{
    switch (piece)
    {
    case Piece::PAWN:
        return pawnValue;
        break;
    case Piece::KNIGHT:
        return knightValue;
        break;
    case Piece::BISHOP:
        return bishopValue;
        break;
    case Piece::ROOK:
        return rookValue;
        break;
    case Piece::QUEEN:
        return queenValue;
        break;
    default:
        return 0;
    }
}
