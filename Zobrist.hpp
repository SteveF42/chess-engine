#ifndef Zobrist_H
#define Zobrist_H
#include <string>
#include <stack>

class Board;

class Zobrist
{
private:
    const std::string fileName = "randomNumbers.txt";
    // 8 pieces for both black and white on 64 squares
    uint64_t pieceArray[6][2][64];
    uint64_t enPessentFile[8];

    void writeRandomNumbers();
    std::stack<int> readRandomNumbers();

public:
    const static int blackKingSide = 0;
    const static int blackQueenSide = 1;
    const static int whiteKingSide = 2;
    const static int whiteQueenSide = 3;

    // piecetype, colorindex, position
    uint64_t sideToMove;
    uint64_t castleRights[4];

    uint64_t generateZobristKey(Board *board);
    Zobrist();

    uint64_t getPieceHash(int pieceType, int colorIndex, int square)
    {
        return pieceArray[pieceType - 1][colorIndex][square];
    }

    uint64_t getEnPessentFile(int square)
    {
        return enPessentFile[square % 8];
    }
};

#endif