#include "AI.hpp"
#include "PieceSquareTable.hpp"
#include <chrono>
#include <bits/stdc++.h>
#include <thread>

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
    transPositions = 0;
    qPositions = 0;
    numExtensions = 0;

    clock_t start, stop;
    // tt->clearTable();
    // minimax();
    iterativeDeepening();
    std::cout << "Nodes searched: " << positions << '\n';
    std::cout << "Quiet nodes searched: " << qPositions << '\n';
    std::cout << "Transposition table hits: " << transPositions << '\n';
    std::cout << "transposition table visits: " << tt->visits << '\n';
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

    for (auto &move : moves)
    {
        position->makeMove(move);
        numPositions += moveGenerationTest(depth - 1, position);
        bestMove = move;
        position->unmakeMove();
    }
    return numPositions;
}

int AI::calculateExtension(const Move &move)
{
    int movedPieceType = move.pieceType;
    int targetRank = move.target / 8;

    int extension = 0;
    if (numExtensions < 16)
    {
        if (position->moveGeneration.isCheck())
        {
            extension = 1;
        }
        else if (Piece::getPieceType(movedPieceType) == Piece::PAWN && (targetRank == 1 || targetRank == 6))
        {
            extension = 1;
        }
    }
    return extension;
}

int AI::minimax(int depth /*= MAXDEPTH*/, int depthFromRoot, int alpha /*=-INFINITY*/, int beta /*=INFINITY*/)
{
    if (clock() - timeoutStart > TIMEOUT_MILISECONDS)
    {
        timeout = true;
        return alpha;
    }

    // if a mating sequence has been found earlier it should skip this position
    if (depthFromRoot > 0)
    {
        alpha = std::max(alpha, (-mateScore) + depthFromRoot);
        beta = std::min(beta, mateScore - depthFromRoot);
        if (alpha >= beta)
        {
            return alpha;
        }
    }
    // look current position in the transposition table
    int cacheEval = tt->readTable(alpha, beta, depth);
    if (cacheEval != tt->FAILED)
    {
        transPositions++;
        if (depthFromRoot == 0)
        {
            bestMove = tt->getMove();
        }
        return cacheEval;
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

    int ttFlag = tt->UPPER;
    Move bestMoveThisPosition;

    for (int i = 0; i < moves.size(); i++)
    {
        Move &move = moves[i];
        int extension = calculateExtension(move);
        numExtensions += extension;
        bool isCapture = position->getBoard()[move.target]->hasNullPiece();

        position->makeMove(move);
        bool needsFullSearch = true;
        int eval = 0;

        if (i >= 3 && extension == 0 && depth >= 3 && !isCapture)
        {
            const int reduceDepth = 1;
            eval = -minimax(depth - 1 - reduceDepth, depthFromRoot + 1, -beta, -alpha);
            // If the evaluation turns out to be better than anything we've found so far, we'll need to redo the
            // search at the full depth to get a more accurate result. Note: this does introduce some danger that
            // we might miss a good move if the reduced search cannot see that it is good, but the idea is for
            // the increased search speed to outweigh these occasional errors.
            needsFullSearch = eval > alpha;
        }

        // Perform a full-depth search
        if (needsFullSearch)
        {
            eval = -minimax(depth - 1 + extension, depthFromRoot + 1, -beta, -alpha);
        }
        position->unmakeMove();

        if (timeout)
        {
            return alpha;
        }

        positions++;

        // opponent had a better move so don't use it
        if (eval >= beta)
        {
            tt->writeTable(depth, beta, tt->LOWER, move);
            return beta;
        }
        if (eval > alpha)
        {
            ttFlag = tt->EXACT;
            alpha = eval;
            bestMoveThisPosition = move;
            if (depthFromRoot == 0)
            {
                bestMove = move;
            }
        }
    }

    tt->writeTable(depth, alpha, ttFlag, bestMoveThisPosition);
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
    orderMoves(captures, false);

    for (auto &capture : captures)
    {
        position->makeMove(capture);
        eval = -searchCaptures(-beta, -alpha);
        position->unmakeMove();
        qPositions++;

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
    if (myMaterial > opponentMaterial + pawnValue * 2 && endgameWeight > 0)
    {
        int mopUpScore = 0;
        int friendlyKingSquare = position->getKing(friendlyIndex)->getPiecePosition();
        int opponentKingSquare = position->getKing(opponentIndex)->getPiecePosition();
        // encourage king to edge of board
        mopUpScore += position->moveGeneration.preComputedMoveData.centreManhattanDistance[opponentKingSquare] * 10;
        // encourage king to move closer to other king
        mopUpScore += (14 - position->moveGeneration.preComputedMoveData.orthogonalDistance[friendlyKingSquare][opponentKingSquare]) * 4;

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

    int kingEarlyPhase = PieceSquareTable::read(PieceSquareTable::kingStart, king->getPiecePosition(), isWhite);
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

void AI::orderMoves(std::vector<Move> &moveTable, bool useTT)
{
    int scores[moveTable.size()];
    int i = 0;

    uint64_t opponentAttacks = position->moveGeneration.getAttackMap();
    uint64_t opponentPawnAttacks = position->moveGeneration.getPawnAttackMap();

    Move hashMove;
    if (useTT)
    {
        hashMove = tt->getMove();
    }
    for (const auto &move : moveTable)
    {
        if (move.target == hashMove.target && move.start == hashMove.start)
        {
            scores[i++] += 1000000000;
            continue;
        }
        Piece *movedPiece = position->getBoard()[move.start]->getPiece();
        int pieceType = movedPiece->getPieceType();
        int moveScoreGuess = 0;
        int movePieceType = position->getBoard()[move.start]->getPiece()->getPieceType();
        bool isCapture = !position->getBoard()[move.target]->hasNullPiece();

        if (isCapture)
        {
            int targetPiece = position->getBoard()[move.target]->getPiece()->getPieceType();
            int changeInPieceScore = (getPieceValue(targetPiece) - getPieceValue(movePieceType));
            bool reCapturePossible = BitBoardUtil::containsBit(opponentAttacks | opponentPawnAttacks, move.target);
            if (reCapturePossible)
            {
                moveScoreGuess += (changeInPieceScore >= 0 ? (8 * million) : (2 * million)) + changeInPieceScore;
            }
            else
            {
                moveScoreGuess += (8 * million) + changeInPieceScore;
            }
        }
        if (move.isEnPassant)
        {
            moveScoreGuess += 25 * pawnValue;
        }
        if (move.pawnPromotion)
        {
            moveScoreGuess += 6 * million;
        }
        if (move.isCastle)
        {
            moveScoreGuess += 1000;
        }
        if (pieceType != Piece::KING)
        {
            if (BitBoardUtil::containsBit(opponentPawnAttacks, move.target))
            {
                moveScoreGuess -= 50;
            }
            else if (BitBoardUtil::containsBit(opponentAttacks, move.target))
            {
                moveScoreGuess -= 25;
            }
        }
        scores[i++] = moveScoreGuess;
        // sortMoves(moveTable,scores);
    }
    quickSort(moveTable, scores, 0, moveTable.size() - 1);
}

int AI::getMaterialInfo(int colorIndex)
{
    auto pieces = position->pieceList.getPieces(colorIndex);
    int numPawns = pieces[PieceList::pawnIndex].size();
    int numKnights = pieces[PieceList::knightIndex].size();
    int numBishops = pieces[PieceList::bishopIndex].size();
    int numRooks = pieces[PieceList::rookIndex].size();
    int numQueens = pieces[PieceList::queenIndex].size();

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
