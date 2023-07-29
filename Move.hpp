#ifndef Move_H
#define Move_H
#include"Piece.hpp"

struct Move
{
    int start;
    int target;
    int possibleEnPassant = 999;
    int pieceType;
    bool capture = false;
    bool isEnPassant;
    bool isCastle;
    bool pawnPromotion;
    int promotionPieceType;
    Piece *capturedPiece = nullptr;
    Move(int s, int t, int pieceType, bool possiblePassant = false, bool castle = false)
    {
        start = s;
        target = t;
        isEnPassant = possiblePassant;
        isCastle = castle;
        this->pieceType = pieceType;

        int targetRank = t / 8;
        if ((Piece::getPieceType(pieceType) == Piece::PAWN) && (targetRank == 0 || targetRank == 7))
            pawnPromotion = true;
        else
            pawnPromotion = false;
    }
    Move()
    {
        start = -1;
        target = -1;
    }
};

#endif