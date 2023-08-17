#include "Board.hpp"
#include <vector>
#include <iostream>

bool Board::whiteToMove;

void Board::setBlackCastleKingSide(bool t) { moveGeneration.blackCastleKingSide = t; }
void Board::setWhiteCastleKingSide(bool t) { moveGeneration.whiteCastleKingSide = t; }
void Board::setBlackCastleQueenSide(bool t) { moveGeneration.blackCastleQueenSide = t; }
void Board::setWhiteCastleQueenSide(bool t) { moveGeneration.whiteCastleQueenSide = t; }

std::vector<Move> Board::getPieceMoves(int idx)
{
    std::vector<Move> pieceMoves;

    for (auto &move : moveGeneration.getMoves())
    {
        if (move.start == idx)
        {
            pieceMoves.push_back(move);
        }
    }
    return pieceMoves;
}

bool Board::validateMove(int startIdx, int target)
{
    for (auto &move : moveGeneration.getMoves())
    {
        if (move.target == target && move.start == startIdx)
        {
            this->makeMove(move);
            return true;
        }
    }
    return false;
}

void Board::makeMove(Move move)
{
    Square *startSquare = board[move.start];
    Square *endSquare = board[move.target];
    Piece *piece = startSquare->getPiece();
    int perspective = whiteToMove ? 1 : -1;
    int startRank = move.start / 8;
    int endRank = move.target / 8;

    // capture
    if (!endSquare->hasNullPiece() && !move.isEnPassant)
    {
        Piece *capturedPiece = endSquare->getPiece();
        move.capturedPiece = capturedPiece;
        pieceList.removePiece(capturedPiece);
        move.capture = true;

        if (capturedPiece->getPieceType() == Piece::ROOK)
        {
            int rank = capturedPiece->getPiecePosition() / 8;
            int file = capturedPiece->getPiecePosition() % 8;
            if (capturedPiece->getPieceColor() == Piece::WHITE)
            {
                // queen side  rook
                if (file == 0 && rank == 7)
                {
                    moveGeneration.whiteCastleQueenSide = false;
                }
                // king side  rook
                else if (file == 7 && rank == 7)
                {
                    moveGeneration.whiteCastleKingSide = false;
                }
            }
            else
            {
                if (file == 0 && rank == 0)
                {
                    moveGeneration.blackCastleQueenSide = false;
                }
                else if (file == 7 && rank == 0)
                {
                    moveGeneration.blackCastleKingSide = false;
                }
            }
        }
    }
    // en passant move
    else if (move.isEnPassant)
    {
        int offset = std::abs(move.start - move.target) == 9 ? -1 : 1;
        offset = offset * perspective;
        Piece *pawn = board[move.start + offset]->getPiece();
        move.capturedPiece = pawn;
        pieceList.removePiece(pawn);
        board[move.start + offset]->setPiece(nullptr);
    }
    // pawn promotion
    if (piece->getPieceType() == Piece::PAWN && move.pawnPromotion)
    {
        pieceList.removePiece(piece);
        piece->promoteType(Piece::QUEEN);
        // removes it from the pawn list and adds it to it to its corresponding pieceList
        pieceList.addPiece(piece);
    }
    // castle move
    if (move.isCastle)
    {
        int startFile = move.start % 8;
        int endFile = move.target % 8;
        if (endFile - startFile == 2) // kingside castle
        {
            Piece *rook = board[move.target + 1]->getPiece();
            rook->setPiecePosition(move.target - 1);
            board[move.target - 1]->setPiece(rook);
            board[move.target + 1]->setPiece(nullptr);
        }
        else // queen side castle
        {
            Piece *rook = board[move.target - 2]->getPiece();
            rook->setPiecePosition(move.target + 1);
            board[move.target - 2]->setPiece(nullptr);
            board[move.target + 1]->setPiece(rook);
        }
    }

    // enable en pessant
    if (piece->getPieceType() == Piece::PAWN && std::abs(startRank - endRank) == 2)
    {
        moveGeneration.possibleEnPassant = (move.start + move.target) / 2;
        move.possibleEnPassant = moveGeneration.possibleEnPassant;
    }
    else
    {
        moveGeneration.possibleEnPassant = -1;
        move.possibleEnPassant = -1;
    }

    CastlingRights oldRights;
    oldRights.blackCastleKingSide = moveGeneration.blackCastleKingSide;
    oldRights.blackCastleQueenSide = moveGeneration.blackCastleQueenSide;
    oldRights.whiteCastleKingSide = moveGeneration.whiteCastleKingSide;
    oldRights.whiteCastleQueenSide = moveGeneration.whiteCastleQueenSide;

    castlingHistory.push(oldRights);
    updateCastlingRights(move);

    piece->setPiecePosition(move.target);
    endSquare->setPiece(piece);
    startSquare->setPiece(nullptr);
    moveHistory.push(move);

    movesetHistory.push(moveGeneration.getMoves());
    whiteToMove = !whiteToMove;
}

Move Board::unmakeMove()
{
    if (moveHistory.empty())
        return {};
    // call a move swap function or something
    Move move = moveHistory.top();
    moveHistory.pop();
    Square *target = board[move.target];
    Square *start = board[move.start];
    Piece *movedPiece = target->getPiece();

    whiteToMove = !whiteToMove;
    int perspective = whiteToMove ? 1 : -1;

    // moves piece back to original square
    movedPiece->setPiecePosition(move.start);
    start->setPiece(movedPiece);
    target->setPiece(nullptr);

    // capture
    if (move.capture && !move.isEnPassant)
    {
        Piece *capturedPiece = move.capturedPiece;
        target->setPiece(capturedPiece);
        pieceList.addPiece(capturedPiece);
    }
    // en passant move
    else if (move.isEnPassant)
    {
        int offset = std::abs(move.start - move.target) == 9 ? -1 : 1;
        offset = offset * perspective;
        board[move.start + offset]->setPiece(move.capturedPiece);
        pieceList.addPiece(move.capturedPiece);
    }
    // pawn promotion
    if (move.pawnPromotion)
    {
        pieceList.removePiece(movedPiece);
        movedPiece->promoteType(Piece::PAWN);
        pieceList.addPiece(movedPiece);
    }
    // castle move
    if (move.isCastle)
    {
        int startFile = move.start % 8;
        int endFile = move.target % 8;
        if (endFile - startFile == 2) // kingside castle
        {
            Piece *rook = board[move.target - 1]->getPiece();
            rook->setPiecePosition(move.target + 1);
            board[move.target + 1]->setPiece(rook);
            board[move.target - 1]->setPiece(nullptr);
        }
        else // queen side castle
        {
            Piece *rook = board[move.target + 1]->getPiece();
            rook->setPiecePosition(move.target - 2);
            board[move.target - 2]->setPiece(rook);
            board[move.target + 1]->setPiece(nullptr);
        }
    }
    CastlingRights castleHistory = castlingHistory.top();
    moveGeneration.blackCastleKingSide = castleHistory.blackCastleKingSide;
    moveGeneration.blackCastleQueenSide = castleHistory.blackCastleQueenSide;
    moveGeneration.whiteCastleKingSide = castleHistory.whiteCastleKingSide;
    moveGeneration.whiteCastleQueenSide = castleHistory.whiteCastleQueenSide;
    castlingHistory.pop();

    moveGeneration.possibleEnPassant = move.possibleEnPassant;
    this->moveGeneration.setMoves(movesetHistory.top());
    movesetHistory.pop();

    return move;
}

void Board::updateCastlingRights(const Move &move)
{
    if (Piece::getPieceType(move.pieceType) == Piece::KING && Piece::getPieceColor(move.pieceType) == Piece::BLACK)
    {
        moveGeneration.blackCastleKingSide = false;
        moveGeneration.blackCastleQueenSide = false;
    }
    else if (Piece::getPieceType(move.pieceType) == Piece::KING && Piece::getPieceColor(move.pieceType) == Piece::WHITE)
    {
        moveGeneration.whiteCastleKingSide = false;
        moveGeneration.whiteCastleQueenSide = false;
    }
    else if (Piece::getPieceType(move.pieceType) == Piece::ROOK && Piece::getPieceColor(move.pieceType) == Piece::WHITE)
    {
        int startRow = move.start / 8;
        int startCol = move.start % 8;
        if (startCol == 0 && startRow == 7)
        {
            moveGeneration.whiteCastleQueenSide = false;
        }
        else if (startCol == 7 && startRow == 7)
        {
            moveGeneration.whiteCastleKingSide = false;
        }
    }
    else if (Piece::getPieceType(move.pieceType) == Piece::ROOK && Piece::getPieceColor(move.pieceType) == Piece::BLACK)
    {
        int startRow = move.start / 8;
        int startCol = move.start % 8;
        if (startCol == 0 && startRow == 0)
        {
            moveGeneration.blackCastleQueenSide = false;
        }
        else if (startCol == 7 && startRow == 0)
        {
            moveGeneration.blackCastleKingSide = false;
        }
    }
}

void Board::setSquarePiece(int idx, Piece *other)
{
    bool isWhite = other->getPieceColor() == Piece::WHITE;
    board[idx]->setPiece(other);
    pieceList.addPiece(other);

    if (other->getPieceType() == Piece::KING)
    {
        if (isWhite)
        {
            whiteKing = other;
        }
        else
        {
            blackKing = other;
        }
    }
}
