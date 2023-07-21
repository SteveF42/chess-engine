#include "AI.hpp"

Move AI::bestMove;

int AI::minimax(Board position, int depth /*= 4*/, int alpha /*=0*/, int beta /*=0*/)
{
    if (depth == 0)
        return generateEval(position);

    position.generateMovesInCurrentPosition();
    auto moveTable = position.getMoves();
    auto moves = orderMoves(moveTable, position);

    if (moveTable.empty())
    {
        // being in check is bad and if theres no moves then a stalemate has occurred
        if (position.getCheck())
            return -INFINITY;
        else
            return 0;
    }

    for (const auto &[key, piece] : moveTable)
    {
        for (const auto &move : piece)
        {
            position.makeMove(move);
            int eval = -minimax(position, depth - 1, -beta, -alpha);
            position.unmakeMove();
            if (eval >= beta)
            {
                bestMove = move;
                return beta;
            }
            alpha = std::max(alpha, eval);
        }
    }

    return alpha;
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
    for (const auto &[key, piece] : moveTable)
    {
        for (const auto &move : piece)
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