#ifndef PieceList_H
#define PieceList_H

#include <vector>
#include "Piece.hpp"

class PieceList
{
private:
    std::vector<Piece *> pieces[2][5] = {{{}, {}, {}, {}, {}}, {{}, {}, {}, {}, {}}};
    int getPieceIndex(int pieceType);

public:
    const static int whiteIndex = 0;
    const static int blackIndex = 1;
    const static int pawnIndex = 0;
    const static int knightIndex = 1;
    const static int rookIndex = 2;
    const static int bishopIndex = 3;
    const static int queenIndex = 4;
    const static int arrSize = 5;

    void addPiece(Piece *piece);
    void removePiece(Piece *piece);
    std::vector<Piece *> *getPieces(int pieceIndex);
};

#endif