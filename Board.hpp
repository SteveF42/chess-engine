#ifndef Board_H
#define Board_H
#include "PieceList.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "NewMoveGenerator.hpp"
#include "Zobrist.hpp"
#include <string>
#include <stack>

struct CastlingRights
{
    bool blackCastleKingSide = false;
    bool whiteCastleKingSide = false;
    bool blackCastleQueenSide = false;
    bool whiteCastleQueenSide = false;
};

class Board
{
private:
    // Bits 0-3 store white and black kingside/queenside castling legality
    // Bits 4-7 store file of ep square (starting at 1, so 0 = no ep square)
    // Bits 8-13 captured piece
    // Bits 14-... fifty mover counter
    std::stack<unsigned int> gameStateHistory;
    unsigned int currentGameState;
    const unsigned int whiteCastleKingsideMask = 0b1111111111111110;
    const unsigned int whiteCastleQueensideMask = 0b1111111111111101;
    const unsigned int blackCastleKingsideMask = 0b1111111111111011;
    const unsigned int blackCastleQueensideMask = 0b1111111111110111;

    const unsigned int whiteCastleMask = whiteCastleKingsideMask & whiteCastleQueensideMask;
    const unsigned int blackCastleMask = blackCastleKingsideMask & blackCastleQueensideMask;

    std::stack<int> enPessentHistory;
    std::stack<Move> moveHistory;
    Square *board[64];

    std::stack<CastlingRights> castlingHistory;
    Piece *blackKing;
    Piece *whiteKing;

    int numSquaresToEdge[64][8];

    uint64_t zobristKey;
    std::stack<uint64_t> zobristKeyHistory;

    // class methods
    void updateCastlingRights(const Move &move);
    void movePiece(Piece *piece, Square *start, Square *target);

public:
    static bool whiteToMove;
    Move unmakeMove();
    void makeMove(Move move);
    bool validateMove(int startIdx, int target);
    void initializeBitBoards();

    // list of bitboards for the white and black index
    uint64_t bitboards[2][5];
    uint64_t colorBitboard[2];

    NewMoveGenerator moveGeneration;
    Zobrist zobrist;
    PieceList pieceList;

    Board()
    {
        for (int i = 0; i < 64; i++)
        {
            board[i] = new Square(i);
        }
    }

    //@colorIndex PieceList index type
    Piece *getKing(int colorIndex)
    {
        if (colorIndex == PieceList::whiteIndex)
        {
            return whiteKing;
        }
        else
        {
            return blackKing;
        }
    }

    uint64_t getZobristKey() { return zobristKey; }

    Square **getBoard() { return board; }
    void setSquarePiece(int idx, Piece *other);
    void setZobristKey(uint64_t zobrist) { zobristKey = zobrist; }

    void setWhiteToMove(bool t) { whiteToMove = t; }
    void setBlackCastleKingSide(bool t);
    void setWhiteCastleKingSide(bool t);
    void setBlackCastleQueenSide(bool t);
    void setWhiteCastleQueenSide(bool t);

    bool getWhiteToMove() { return whiteToMove; }
};

#endif