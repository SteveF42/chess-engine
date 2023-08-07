#include "PieceList.hpp"


void PieceList::addPiece(Piece *piece)
{
    int colorIndex = piece->getPieceColor() == Piece::WHITE ? whiteIndex : blackIndex;
    int pieceIndex = getPieceIndex(piece->getPieceType());

    //piece is not a king since king is being tracked individually
    if(pieceIndex != -1)
        pieces[colorIndex][pieceIndex].push_back(piece);
}

void PieceList::removePiece(Piece *piece)
{
    int colorIndex = piece->getPieceColor() == Piece::WHITE ? whiteIndex : blackIndex;
    int pieceIndex = getPieceIndex(piece->getPieceType());
    if(pieceIndex == -1)
        return;

    auto &pieceList = pieces[colorIndex][pieceIndex];

    // still looping through the piece array and deleting which isn't the best but it's only going through a maximum of like 2 for anything thats not a pawn
    for (int i = 0; i < pieceList.size(); i++)
    {
        Piece *other = pieceList[i];
        if (other->getPiecePosition() == piece->getPiecePosition())
        {
            pieceList.erase(pieceList.begin() + i);
            return;
        }
    }
}

std::vector<Piece *>* PieceList::getPieces(int sideIndex)
{
    return pieces[sideIndex];
}

int PieceList::getPieceIndex(int pieceType)
{
    switch (pieceType)
    {
    case Piece::PAWN:
        return pawnIndex;
    case Piece::ROOK:
        return rookIndex;
    case Piece::BISHOP:
        return bishopIndex;
    case Piece::KNIGHT:
        return knightIndex;
    case Piece::QUEEN:
        return queenIndex;
    default:
        return -1;
    }
}