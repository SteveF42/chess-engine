#include "AI.hpp"
#include "PieceSquareTable.hpp"

Move AI::bestMove;
int AI::positions;
Board *AI::board;

long AI::moveGenerationTest(int depth, Board &position)
{
    if (depth == 0)
        return 1;

    position.moveGeneration.generateMovesInCurrentPosition();
    auto moves = position.moveGeneration.getMoves();
    long numPositions = 0;

    for (auto &[key, val] : moves)
    {
        for (auto &move : val)
        {
            position.makeMove(move);
            numPositions += moveGenerationTest(depth - 1, position);
            position.unmakeMove();
        }
    }
    return numPositions;
}

int AI::minimax(Board &position, int depth /*= MAXDEPTH*/, int alpha /*=-INFINITY*/, int beta /*=INFINITY*/)
{
    if (depth == 0)
        return searchCaptures(position, alpha, beta);

    position.moveGeneration.generateMovesInCurrentPosition();
    auto moveTable = position.moveGeneration.getMoves();
    auto moves = orderMoves(moveTable, position);

    if (moves.empty())
    {
        // being in check is bad and if theres no moves then a stalemate has occurred
        if (position.moveGeneration.getCheck())
            return -INFINITY;
        else
            return 0;
    }
    int bestEval = -INFINITY;

    for (const auto &move : moves)
    {
        position.makeMove(move);
        positions++;
        int eval = -minimax(position, depth - 1, -beta, -alpha);
        if (eval > bestEval)
        {
            bestEval = eval;
            if (depth == MAXDEPTH)
                bestMove = move;
        }
        (void)position.unmakeMove();
        // opponent had a better move so don't use it
        if (eval >= beta)
        {
            return beta;
        }
        alpha = std::max(alpha, eval);
    }

    return alpha;
}

int AI::searchCaptures(Board &position, int alpha, int beta)
{
    int eval = evaluate(position);

    if (eval >= beta)
        return beta;
    alpha = std::max(alpha, eval);

    auto captureMoves = generateCaptures(position);
    orderMoves(captureMoves, position);

    for (auto &capture : captureMoves)
    {
        position.makeMove(capture);
        eval = -searchCaptures(position, -beta, -alpha);
        (void)position.unmakeMove();

        if (eval >= beta)
            return beta;
        alpha = std::max(alpha, eval);
    }
    return alpha;
}

int AI::evaluate(Board &position)
{
    board = &position;
    int whiteEval = 0;
    int blackEval = 0;

    int whiteMaterial = countMaterial(PieceList::whiteIndex);
    int blackMaterial = countMaterial(PieceList::blackIndex);

    int whiteMaterialWithNoPawns = whiteMaterial - position.moveGeneration.pieceList.getPieces(PieceList::whiteIndex)[PieceList::pawnIndex].size() * pawnValue;
    int blackMaterialWithNoPawns = whiteMaterial - position.moveGeneration.pieceList.getPieces(PieceList::blackIndex)[PieceList::pawnIndex].size() * pawnValue;

    float whiteEndgamePhaseWeight = endgamePhaseWeight(whiteMaterialWithNoPawns);
    float blackEndgamePhaseWeight = endgamePhaseWeight(blackMaterialWithNoPawns);

    whiteEval += whiteMaterial;
    blackEval += blackMaterial;

    whiteEval += evaluatePieceSquareTables(PieceList::whiteIndex, blackEndgamePhaseWeight);
    blackEval += evaluatePieceSquareTables(PieceList::blackIndex, whiteEndgamePhaseWeight);

    // mess with this some other day
    //  whiteEval += mopUpEval(PieceList::whiteIndex, PieceList::blackIndex, whiteMaterial, blackMaterial, blackEndgamePhaseWeight);

    int eval = whiteEval - blackEval;
    int perspective = Board::whiteToMove ? 1 : -1;

    return eval * perspective;
}

// int AI::mopUpEval(int friendlyIndex, int opponentIndex, int myMaterial, int opponentMaterial, float endgameWeight)
// {
//     int mopUpScore = 0;
//     if (myMaterial > opponentMaterial + pawnValue * 2 && endgameWeight > 0)
//     {

//         int friendlyKingSquare = board.KingSquare[friendlyIndex];
//         int opponentKingSquare = board.KingSquare[opponentIndex];
//         mopUpScore += PrecomputedMoveData.centreManhattanDistance[opponentKingSquare] * 10;
//         // use ortho dst to promote direct opposition
//         mopUpScore += (14 - PrecomputedMoveData.NumRookMovesToReachSquare(friendlyKingSquare, opponentKingSquare)) * 4;

//         return (int)(mopUpScore * endgameWeight);
//     }
//     return 0;
// }

int AI::evaluatePieceSquareTables(int colorIndex, float endgamePhaseWeight)
{

    int value = 0;
    bool isWhite = colorIndex == PieceList::whiteIndex;
    auto pieces = board->moveGeneration.pieceList.getPieces(colorIndex);
    value += evaluatePieceSquareTable(PieceSquareTable::pawns, pieces[PieceList::pawnIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::rooks, pieces[PieceList::rookIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::knights, pieces[PieceList::knightIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::bishops, pieces[PieceList::bishopIndex], isWhite);
    value += evaluatePieceSquareTable(PieceSquareTable::queens, pieces[PieceList::queenIndex], isWhite);
    int kingEarlyPhase = PieceSquareTable::read(PieceSquareTable::kingMiddle, colorIndex, isWhite);
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
    auto pieces = board->moveGeneration.pieceList.getPieces(pieceIndex);
    int material = 0;
    material += pieces[PieceList::pawnIndex].size();
    material += pieces[PieceList::knightIndex].size();
    material += pieces[PieceList::rookIndex].size();
    material += pieces[PieceList::bishopIndex].size();
    material += pieces[PieceList::queenIndex].size();
    return material;
}

std::vector<Move> AI::orderMoves(std::map<int, std::vector<Move>> &moveTable, Board &position)
{
    std::vector<Move> moves;
    std::vector<int> scores;
    for (const auto &[key, moveList] : moveTable)
    {
        if (moveList.empty())
            continue;

        for (const auto &move : moveList)
        {
            int moveScoreGuess = 0;
            int movePieceType = position.getBoard()[move.start]->getPiece()->getPieceType();
            if (!position.getBoard()[move.target]->hasNullPiece())
            {
                int targetPiece = position.getBoard()[move.start]->getPiece()->getPieceType();
                moveScoreGuess += 10 * getPieceValue(targetPiece) - getPieceValue(movePieceType);
            }
            else if (move.pawnPromotion)
            {
                moveScoreGuess += 10;
            }

            moves.push_back(move);
            scores.push_back(moveScoreGuess);
        }
    }
    sortMoves(moves, scores);
    return moves;
}

std::vector<Move> AI::orderMoves(std::vector<Move> &moveList, Board &position)
{
    std::vector<Move> moves;
    std::vector<int> scores;

    for (const auto &move : moveList)
    {
        int moveScoreGuess = 0;
        int movePieceType = position.getBoard()[move.start]->getPiece()->getPieceType();
        if (!position.getBoard()[move.target]->hasNullPiece())
        {
            int targetPiece = position.getBoard()[move.target]->getPiece()->getPieceType();
            moveScoreGuess += 10 * getPieceValue(targetPiece) - getPieceValue(movePieceType);
        }

        if (move.pawnPromotion)
        {
            moveScoreGuess += 10;
        }

        moves.push_back(move);
        scores.push_back(moveScoreGuess);
    }
    sortMoves(moves, scores);
    return moves;
}

// this is terribly inefficient since its generating all moves then generating removing the non captures
std::vector<Move> AI::generateCaptures(Board &position)
{
    position.moveGeneration.generateMovesInCurrentPosition();
    auto moves = position.moveGeneration.getMoves();
    std::vector<Move> validCaptures;
    Square **board = position.getBoard();

    for (auto const &[key, moves] : moves)
    {
        for (auto move : moves)
        {
            if (!board[move.target]->hasNullPiece())
            {
                validCaptures.push_back(move);
            }
        }
    }
    return validCaptures;
}

void AI::sortMoves(std::vector<Move> &moves, std::vector<int> &weights)
{
    for (int i = 1; i < weights.size(); i++)
    {
        int j = i - 1;

        while (j >= 0 && weights[i] > weights[i])
        {
            // move the move
            moves[j + 1] = moves[j];
            // move the weight
            weights[j + 1] = weights[j];
            j--;
        }
        moves[j + 1] = moves[i];
        weights[j + 1] = weights[i];
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