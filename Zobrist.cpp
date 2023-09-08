#include "Zobrist.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include "PieceList.hpp"
#include "Board.hpp"

uint64_t rand_uint64_slow()
{
    uint64_t r = 0;
    for (int i = 0; i < 64; i++)
    {
        r = r * 2 + rand() % 2;
    }
    return r;
}

void Zobrist::writeRandomNumbers()
{
    // rules for generating random numbers
    //  One number for each piece at each square
    //  One number to indicate the side to move is black
    //  Four numbers to indicate the castling rights, though usually 16 (2^4) are used for speed
    //  Eight numbers to indicate the file of a valid En passant square, if any
    int randomNumbers = 12 * 64 + 1 + 4 + 8;
    std::ofstream randomNumberFile(fileName);

    for (int i = 0; i < randomNumbers; i++)
    {
        uint64_t num = rand_uint64_slow();
        randomNumberFile << num;

        if (i != randomNumbers - 1)
        {
            randomNumberFile << ',';
        }
    }
    randomNumberFile.close();
}

std::stack<int> Zobrist::readRandomNumbers()
{
    if (!std::filesystem::exists(fileName))
    {
        writeRandomNumbers();
    }

    std::ifstream randomNumbers(fileName);
    std::string numberString;
    std::getline(randomNumbers, numberString);
    std::istringstream ss(numberString);
    std::stack<int> numbersQueue;

    std::string str;
    while (std::getline(ss, str, ','))
    {
        uint64_t num = (uint64_t)std::stoull(str);
        numbersQueue.push(num);
    }

    randomNumbers.close();

    return numbersQueue;
}

uint64_t Zobrist::generateZobristKey(Board *board)
{
    uint64_t zobristKey = 0;
    auto whitePieces = board->pieceList.getPieces(PieceList::whiteIndex);
    auto blackPieces = board->pieceList.getPieces(PieceList::blackIndex);

    auto whiteKing = board->getKing(PieceList::whiteIndex);
    auto blackKing = board->getKing(PieceList::blackIndex);
    zobristKey ^= pieceArray[whiteKing->getPieceType()-1][PieceList::whiteIndex][whiteKing->getPiecePosition()];
    zobristKey ^= pieceArray[blackKing->getPieceType()-1][PieceList::blackIndex][blackKing->getPiecePosition()];

    // hashing white pieces
    for (int i = 0; i < PieceList::arrSize; i++)
    {
        for (int j = 0; j < whitePieces[i].size(); j++)
        {
            Piece *piece = whitePieces[i][j];
            zobristKey ^= pieceArray[piece->getPieceType() - 1][PieceList::whiteIndex][piece->getPiecePosition()];
        }
    }
    // hashing black pieces
    for (int i = 0; i < PieceList::arrSize; i++)
    {
        for (int j = 0; j < blackPieces[i].size(); j++)
        {
            Piece *piece = blackPieces[i][j];
            zobristKey ^= pieceArray[piece->getPieceType() - 1][PieceList::blackIndex][piece->getPiecePosition()];
        }
    }
    // hash en pessent
    if (board->moveGeneration.possibleEnPassant != board->moveGeneration.noEnPessant)
        zobristKey ^= enPessentFile[board->moveGeneration.possibleEnPassant % 8];

    // hashing castle rights
    if (board->moveGeneration.blackCastleKingSide)
        zobristKey ^= castleRights[blackKingSide];
        
    if (board->moveGeneration.blackCastleQueenSide)
        zobristKey ^= castleRights[blackQueenSide];

    if (board->moveGeneration.whiteCastleKingSide)
        zobristKey ^= castleRights[whiteKingSide];

    if (board->moveGeneration.whiteCastleQueenSide)
        zobristKey ^= castleRights[whiteQueenSide];

    // hashing side to move
    if (!board->whiteToMove)
        zobristKey ^= sideToMove;

    std::cout << "Generated key " << zobristKey << '\n';
    return zobristKey;
}

Zobrist::Zobrist()
{
    std::stack<int> numbers = readRandomNumbers();

    // fills numbers for each piece type on each square
    for (int i = 0; i < 64; i++)
    {
        for (int piece = 0; piece < 6; piece++)
        {
            pieceArray[piece][PieceList::whiteIndex][i] = numbers.top();
            numbers.pop();
            pieceArray[piece][PieceList::blackIndex][i] = numbers.top();
            numbers.pop();
        }
    }
    // castle numbers
    for (int i = 0; i < 4; i++)
    {
        castleRights[i] = numbers.top();
        numbers.pop();
    }

    // enPessent numbers
    for (int i = 0; i < 8; i++)
    {
        enPessentFile[i] = numbers.top();
        numbers.pop();
    }

    sideToMove = numbers.top();
    numbers.pop();
}
