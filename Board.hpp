#ifndef Board_H
#define Board_H
#include <string>
#include "ReadFen.hpp"
#include "Square.hpp"
#include <stack>

struct Move
{
    int start;
    int target;
    bool isEnPassant;
    bool capture = false;
    int possibleEnPassant = 999;
    Move(int s, int t, bool possiblePassant = false)
    {
        start = s;
        target = t;
        isEnPassant = possiblePassant;
    }
    Move() {}
};

struct CheckOrPin
{
    int position;
    int direction;

    CheckOrPin(int pos, int dir)
    {
        position = pos;
        direction = dir;
    }
    CheckOrPin()
    {
        position = -1;
        direction = -1;
    }
};

class Board
{
private:
    // variables
    const int slidingMovesOffsets[8] = {1, -1, 8, -8, 7, -7, 9, -9};
    const int kingMovesOffsets[8] = {1, 7, 8, 9, -1, -7, -8, -9};
    const int knightOffset[8] = {6, 10, 15, 17, -6, -10, -15, -17};

    bool checkFlag;
    std::vector<CheckOrPin> pins;
    std::vector<CheckOrPin> checks;
    int possibleEnPassant;

    std::vector<std::vector<Move>> moveset;
    Square *board[64];
    bool whiteToMove = true;
    bool blackCastleKingSide = false;
    bool whiteCastleKingSide = false;
    bool blackCastleQueenSide = false;
    bool whiteCastleQueenSide = false;

    std::stack<Piece *> capturedPieces;
    std::stack<Move> moveHistory;
    Piece *blackKing;
    Piece *whiteKing;

    int numSquaresToEdge[64][8];

    // class methods
    std::vector<Move> getSlidingTypeMoves(Piece *other);
    std::vector<Move> getPawnMoves(Piece *other);
    std::vector<Move> getKnightMoves(Piece *other);
    std::vector<Move> getKingMoves(Piece *other);
    std::vector<std::vector<Move>> pieceAvailableMoves();
    bool inCheck(int kingPos);
    void boardEdgeData();
    void makeMove(Move move);
    void unmakeMove();
    void checkForPinsAndChecks(std::vector<CheckOrPin> &pins,std::vector<CheckOrPin> &checks, bool &inCheck);

public:
    // first four offsets are rook type moves and the second are bishop like moves, all can be used for the queen
    void generateMovesInCurrentPosition();
    std::vector<Move> getPieceMoves(int idx);
    bool validateMove(int startIdx, int target);

    Board()
    {
        for (int i = 0; i < 64; i++)
        {
            board[i] = new Square(i);
        }
        boardEdgeData();
    }

    Square **getBoard() { return board; }
    void setSquarePiece(int idx, Piece *other);
    bool getCheck() { return checkFlag; }

    void setWhiteToMove(bool t) { whiteToMove = t; }
    void setBlackCastleKingSide(bool t) { blackCastleKingSide = t; }
    void setWhiteCastleKingSide(bool t) { whiteCastleKingSide = t; }
    void setBlackCastleQueenSide(bool t) { blackCastleQueenSide = t; }
    void setWhiteCastleQueenSide(bool t) { whiteCastleQueenSide = t; }

    bool getWhiteToMove() { return whiteToMove; }
    bool getBlackCastleKingSide() { return blackCastleKingSide; }
    bool getWhiteCastleKingSide() { return whiteCastleKingSide; }
    bool getBlackCastleQueenSide() { return blackCastleQueenSide; }
    bool getWhiteCastleQueenSide() { return whiteCastleQueenSide; }
};

#endif