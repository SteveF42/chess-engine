#include "AI.hpp"

Move AI::bestMove;

int AI::minimax(Board position, int depth /*= MAXDEPTH*/, int alpha /*=0*/, int beta /*=0*/)
{
    if (depth == 0)
        return generateEval(position);

    position.generateMovesInCurrentPosition();
    auto moveTable = position.getMoves();
    auto moves = orderMoves(moveTable, position);

    if (moves.empty())
    {
        // being in check is bad and if theres no moves then a stalemate has occurred
        if (position.getCheck())
            return -INFINITY;
        else
            return 0;
    }
    int bestEval = -INFINITY;

    for (const auto &move : moves)
    {
        position.makeMove(move);
        int eval = -minimax(position, depth - 1, -beta, -alpha);
        if (eval > bestEval)
        {
            bestEval = eval;
            if (depth == MAXDEPTH)
                bestMove = move;
        }
        position.unmakeMove();
        // opponent had a better move so don't use it
        if (eval >= beta)
        {
            return beta;
        }
        alpha = std::max(alpha, eval);
    }

    return alpha;
}

int AI::searchCaptures(Board position, int alpha, int beta)
{
    int eval = generateEval(position);

    if (eval >= beta)
        return beta;

    alpha = std::max(alpha, eval);

    // generate only capture moves
    // order moves
}

int AI::generateEval(Board position)
{
    Square **board = position.getBoard();

    int blackMaterial = 0;
    int whiteMaterial = 0;
    for (int i = 0; i < 64; i++)
    {
        if (board[i]->hasNullPiece())
            continue;
        Piece *piece = board[i]->getPiece();

        if (piece->getPieceColor() == Piece::WHITE)
        {
            switch (piece->getPieceType())
            {
            case Piece::PAWN:
                whiteMaterial += pawnValue;
                break;
            case Piece::KNIGHT:
                whiteMaterial += knightValue;
                break;
            case Piece::BISHOP:
                whiteMaterial += bishopValue;
                break;
            case Piece::ROOK:
                whiteMaterial += rookValue;
                break;
            case Piece::QUEEN:
                whiteMaterial += queenValue;
                break;
            default:
                break;
            }
        }
        else
        {
            switch (piece->getPieceType())
            {
            case Piece::PAWN:
                blackMaterial += pawnValue;
                break;
            case Piece::KNIGHT:
                blackMaterial += knightValue;
                break;
            case Piece::BISHOP:
                blackMaterial += bishopValue;
                break;
            case Piece::ROOK:
                blackMaterial += rookValue;
                break;
            case Piece::QUEEN:
                blackMaterial += queenValue;
                break;
            default:
                break;
            }
        }
    }

    int eval = whiteMaterial - blackMaterial;
    int side = position.getWhiteToMove() ? 1 : -1;
    return eval * side;
}

std::vector<Move> AI::orderMoves(std::map<int, std::vector<Move>> moveTable, Board &position)
{
    std::vector<Move> moves;
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

            if (move.pawnPromotion)
            {
                moveScoreGuess += 10;
            }

            moves.push_back(move);
        }
    }
    return moves;
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