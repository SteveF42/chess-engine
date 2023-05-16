#include "ReadFen.hpp"
#include "Piece.hpp"
#include "Board.hpp"
#include <string>
#include <iostream>
#include <vector>

const std::string ReadFen::startingString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

std::vector<std::string> splitString(std::string s)
{
    std::vector<std::string> arr;

    std::string temp = "";
    for (auto i : s)
    {
        if (i == ' ')
        {
            arr.push_back(temp);
            temp = "";
        }
        else
        {
            temp += i;
        }
    }

    return arr;
}

Position ReadFen::readFenString(std::string fen)
{
    std::map<char, int> lookUpTable = {{'k', Piece::KING}, {'q', Piece::QUEEN}, {'b', Piece::BISHOP}, {'r', Piece::ROOK}, {'n', Piece::KNIGHT}, {'p', Piece::PAWN}};
    Position currentPosition;
    int* board = currentPosition.board;
    std::vector<std::string> sections = splitString(fen);

    int rank = 0;
    int file = 0;
    // read the first section board position
    for (char i : sections[0])
    {
        if (isdigit(i))
        {
            file += i - '0';
        }
        else
        {
            if (i == '/')
            {
                rank += 1;
                file = 0;
                continue;
            }

            int position = (rank * 8) + file;
            bool isWhite = isupper(i);
            char c = tolower(i);
            int pieceType = lookUpTable[c];
            int pieceColor = isWhite ? Piece::WHITE : Piece::BLACK;
            board[position] = pieceColor | pieceType;

            file += 1;
        }
    }

    // read second section player move
    if (sections[1] == "w")
    {
        currentPosition.whiteToMove = true;
    }
    currentPosition.whiteToMove == false;

    std::string s = "";
    // read third section castling rights
    currentPosition.blackCastleKingSide = sections[2].find("K") ? true : false;
    currentPosition.blackCastleQueenSide = sections[2].find("Q") ? true : false;
    currentPosition.whiteCastleKingSide = sections[2].find("k") ? true : false;
    currentPosition.whiteCastleQueenSide = sections[2].find("q") ? true : false;

    return currentPosition;
}