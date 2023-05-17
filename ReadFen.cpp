#include "ReadFen.hpp"
#include "Piece.hpp"
#include "Board.hpp"
#include <string>
#include <iostream>
#include "GameEngine.hpp"
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

Board *ReadFen::readFenString(std::string fen)
{
    std::map<char, int> lookUpTable = {{'k', Piece::KING}, {'q', Piece::QUEEN}, {'b', Piece::BISHOP}, {'r', Piece::ROOK}, {'n', Piece::KNIGHT}, {'p', Piece::PAWN}};
    std::vector<std::string> sections = splitString(fen);
    Board *newBoard = new Board();

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
            sf::Vector2f spritePosition(file * GameEngine::SQUARESIZE, rank * GameEngine::SQUARESIZE);
            Piece *newPiece = new Piece(pieceType | pieceColor, spritePosition);

            newBoard->setSquarePiece(position, newPiece);

            file += 1;
        }
    }

    // read second section player move
    if (sections[1] == "w")
    {
        newBoard->setWhiteToMove(true);
    }
    newBoard->setWhiteToMove(false);

    std::string s = "";
    // read third section castling rights
    newBoard->setBlackCastleKingSide(sections[2].find("K") ? true : false);
    newBoard->setBlackCastleQueenSide(sections[2].find("Q") ? true : false);
    newBoard->setWhiteCastleKingSide(sections[2].find("k") ? true : false);
    newBoard->setWhiteCastleQueenSide(sections[2].find("q") ? true : false);

    return newBoard;
}