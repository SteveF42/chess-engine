#ifndef Board_H
#define Board_H
#include "ReadFen.hpp"
#include "MoveGeneration.hpp"
#include "Move.hpp"
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
    std::stack<std::map<int, std::vector<Move>>> movesetHistory;
    std::stack<Move> moveHistory;
    int possibleEnPassant;
    Square *board[64];

    std::stack<CastlingRights> castlingHistory;

    int numSquaresToEdge[64][8];

    // class methods
    void updateCastlingRights(const Move &move);

public:
    static bool whiteToMove;
    Piece* getKing(int colorIndex){
        if(colorIndex == PieceList::whiteIndex){
            return moveGeneration.getWhiteKing();
        }else{
            return moveGeneration.getBlackKing();
        }
    }
    // first four offsets are rook type moves and the second are bishop like moves, all can be used for the queen
    void generateMovesInCurrentPosition();
    void promotePawn(int pieceLocation, int pieceType);
    Move unmakeMove();
    void makeMove(Move move);
    std::vector<Move> getPieceMoves(int idx);
    bool validateMove(int startIdx, int target);
    MoveGeneration moveGeneration;

    Board()
    {
        for (int i = 0; i < 64; i++)
        {
            board[i] = new Square(i);
        }
        moveGeneration.setBoardRef(board);
    }

    Square **getBoard() { return board; }
    void setSquarePiece(int idx, Piece *other);

    void setWhiteToMove(bool t) { whiteToMove = t; }
    void setBlackCastleKingSide(bool t);
    void setWhiteCastleKingSide(bool t);
    void setBlackCastleQueenSide(bool t);
    void setWhiteCastleQueenSide(bool t);

    bool getWhiteToMove() { return whiteToMove; }
};

#endif