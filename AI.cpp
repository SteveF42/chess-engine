#include "AI.hpp"
#include "PieceSquareTable.hpp"
#include <chrono>
#include <bits/stdc++.h>

Move AI::bestMove;
int AI::positions;
Board *AI::position;

void AI::generateBestMove(Board *ref)
{
    position = ref;
    bestMove = Move();
    positions = 0;
    clock_t start, stop;
    start = clock();
    minimax();
    stop = clock();
    std::cout << "Positions evaluated: " << AI::positions << '\n';
    std::cout << "Time for execution: " << std::fixed << double(stop - start) / CLOCKS_PER_SEC << std::setprecision(5) << '\n';
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

int AI::minimax(int depth /*= MAXDEPTH*/, int alpha /*=-INFINITY*/, int beta /*=INFINITY*/)
{
    if (depth == 0)
        // return evaluate();
        return searchCaptures(alpha, beta);

    auto moves = position->moveGeneration.generateMoves(position);
    orderMoves(moves);

    if (moves.empty())
    {
        // being in check is bad and if theres no moves then a stalemate has occurred
        if (position->moveGeneration.isCheck())
            return -AI::INFINITE;
        else
            return 0;
    }

    for (const auto &move : moves)
    {
        position->makeMove(move);
        int eval = -minimax(depth - 1, -beta, -alpha);
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
            if (depth == MAXDEPTH)
            {
                bestMove = move;
            }
        }
    }

    return alpha;
}

int AI::searchCaptures(int alpha, int beta,int depth)
{
    int eval = evaluate();

    if (eval >= beta)
        return beta;
    alpha = std::max(alpha, eval);

    auto captures = position->moveGeneration.generateMoves(position, false);
    orderMoves(captures);

    for (auto &capture : captures)
    {
        position->makeMove(capture);
        eval = -searchCaptures(-beta, -alpha,depth-1);
        position->unmakeMove();
        AI::positions++;

        if (eval >= beta)
            return beta;
        alpha = std::max(alpha, eval);
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

    float whiteEndgamePhaseWeight = endgamePhaseWeight(whiteMaterialWithNoPawns);
    float blackEndgamePhaseWeight = endgamePhaseWeight(blackMaterialWithNoPawns);

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
    value += evaluatePieceSquareTable(PieceSquareTable::pawns, pieces[PieceList::pawnIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::rooks, pieces[PieceList::rookIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::knights, pieces[PieceList::knightIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::bishops, pieces[PieceList::bishopIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::queens, pieces[PieceList::queenIndex], isWhite);
    int kingEarlyPhase = PieceSquareTable::read(PieceSquareTable::kingMiddle, king->getPiecePosition(), isWhite);
    value += (int)(kingEarlyPhase * (1 - endgamePhaseWeight));
    // value += PieceSquareTable.Read (PieceSquareTable.kingMiddle, board.KingSquare[colourIndex], isWhite);

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
    std::vector<int> scores;

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
            moveScoreGuess += 30;
        }
        if (move.isCastle)
        {
            moveScoreGuess += 10;
        }
        if (position->moveGeneration.containsSquareInPawnAttackMap(move.target))
        {
            moveScoreGuess -= 350;
        }
        scores.push_back(moveScoreGuess);
    }
    sortMoves(moveTable, scores);
}

void AI::sortMoves(std::vector<Move> &moves, std::vector<int> &weights)
{
    if(moves.size() == 0 && weights.size() == 0){
        return;
    }

    for (int i = 0; i < weights.size() - 1; i++)
    {
        for (int j = i + 1; j > 0; j--)
        {
            int swampIndex = j - 1;
            if (weights[swampIndex] < weights[j])
            {
                Move& temp = moves[j];
                moves[j] = moves[swampIndex];
                moves[swampIndex] = temp;

                int temp2 = weights[j];
                weights[j] = weights[swampIndex];
                weights[swampIndex] = temp2;
            }
        }
    }
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
