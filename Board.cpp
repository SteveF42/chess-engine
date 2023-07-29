#include "Board.hpp"
#include <vector>
#include <iostream>
#include "GameEngine.hpp"

bool Board::whiteToMove;

void Board::setBlackCastleKingSide(bool t) { moveGeneration.blackCastleKingSide = t; }
void Board::setWhiteCastleKingSide(bool t) { moveGeneration.whiteCastleKingSide = t; }
void Board::setBlackCastleQueenSide(bool t) { moveGeneration.blackCastleQueenSide = t; }
void Board::setWhiteCastleQueenSide(bool t) { moveGeneration.whiteCastleQueenSide = t; }

std::vector<Move> Board::getPieceMoves(int idx)
{
    return moveGeneration.getMoves()[idx];
}

bool Board::validateMove(int startIdx, int target)
{
    for (auto move : moveGeneration.getMoves()[startIdx])
    {
        if (move.target == target)
        {
            this->makeMove(move);
            return true;
        }
    }
    return false;
}

void Board::makeMove(Move move)
{
    // call a move swap function or something
    Square *startSquare = board[move.start];
    Square *endSquare = board[move.target];
    Piece *piece = startSquare->getPiece();
    int startRank = move.start / 8;
    int endRank = move.target / 8;

    if (piece->getPieceType() == Piece::PAWN && std::abs(startRank - endRank) == 2)
    {
        this->possibleEnPassant = (move.start + move.target) / 2;
        move.possibleEnPassant = this->possibleEnPassant;
    }
    else
    {
        this->possibleEnPassant = 999;
        move.possibleEnPassant = 999;
    }

    // capture
    if (!endSquare->hasNullPiece())
    {
        Piece *capturedPiece = endSquare->getPiece();
        move.capture = true;
        move.capturedPiece = capturedPiece;
        moveGeneration.pieceList.removePiece(capturedPiece);

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
        int offset = move.start - move.target == 9 ? -1 : 1;
        Piece *pawn = board[move.start + offset]->getPiece();
        move.capturedPiece = pawn;
        moveGeneration.pieceList.removePiece(pawn);
        board[move.start + offset]->setPiece(nullptr);
    }
    // pawn promotion
    else if (piece->getPieceType() == Piece::PAWN)
    {
        if (piece->getPieceColor() == Piece::WHITE && endSquare->getSquarePosition() == 0)
        {
            move.pawnPromotion = true;
        }
        else if (piece->getPieceColor() == Piece::BLACK && endSquare->getSquarePosition() == 7)
        {
            move.pawnPromotion = true;
        }
    }
    // castle move
    else if (move.isCastle)
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

void Board::unmakeMove()
{
    if (moveHistory.empty())
        return;
    // call a move swap function or something
    Move move = moveHistory.top();
    moveHistory.pop();
    Square *target = board[move.target];
    Square *start = board[move.start];
    Piece *movedPiece = target->getPiece();

    // moves piece back to original square
    movedPiece->setPiecePosition(move.start);
    start->setPiece(movedPiece);
    target->setPiece(nullptr);

    // capture
    if (move.capture)
    {
        Piece *capturedPiece = move.capturedPiece;
        target->setPiece(capturedPiece);
        moveGeneration.pieceList.addPiece(capturedPiece);
    }
    // en passant move
    if (move.isEnPassant)
    {
        possibleEnPassant = move.possibleEnPassant;
        int offset = move.start - move.target == 9 ? -1 : 1;
        board[move.start + offset]->setPiece(move.capturedPiece);
        moveGeneration.pieceList.addPiece(move.capturedPiece);
    }
    // pawn promotion
    if (move.pawnPromotion)
    {
        movedPiece->setPieceType(Piece::PAWN | movedPiece->getPieceColor());
        movedPiece->revertSprite();
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

    this->moveGeneration.setMoves(movesetHistory.top());
    movesetHistory.pop();

    whiteToMove = !whiteToMove;
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
    moveGeneration.pieceList.addPiece(other);

    if (other->getPieceType() == Piece::KING)
    {
        if (isWhite)
        {
            moveGeneration.setWhiteKing(other);
        }
        else
        {
            moveGeneration.setblackKing(other);
        }
    }
}

void Board::promotePawn(int pieceLocation, int pieceType)
{
    Piece *piece = board[pieceLocation]->getPiece();
    piece->setPieceType(pieceType);
}