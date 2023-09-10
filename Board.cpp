#include "Board.hpp"
#include <vector>
#include <iostream>
#include "BitBoardUtil.hpp"

bool Board::whiteToMove;

void Board::setBlackCastleKingSide(bool t) { moveGeneration.blackCastleKingSide = t; }
void Board::setWhiteCastleKingSide(bool t) { moveGeneration.whiteCastleKingSide = t; }
void Board::setBlackCastleQueenSide(bool t) { moveGeneration.blackCastleQueenSide = t; }
void Board::setWhiteCastleQueenSide(bool t) { moveGeneration.whiteCastleQueenSide = t; }

std::vector<Move> Board::getPieceMoves(int idx)
{
    std::vector<Move> pieceMoves;

    for (auto &move : moveGeneration.getMoves())
    {
        if (move.start == idx)
        {
            pieceMoves.push_back(move);
        }
    }
    return pieceMoves;
}

bool Board::validateMove(int startIdx, int target)
{
    for (auto &move : moveGeneration.getMoves())
    {
        if (move.target == target && move.start == startIdx)
        {
            this->makeMove(move);
            return true;
        }
    }
    return false;
}

void Board::movePiece(Piece *piece, Square *start, Square *target)
{
    int colorIndex = piece->getPieceColor() == Piece::WHITE ? PieceList::whiteIndex : PieceList::blackIndex;
    piece->setPiecePosition(target->getSquarePosition());
    target->setPiece(piece);
    start->setPiece(nullptr);

    BitBoardUtil::flipBits(bitboards[colorIndex][piece->getPieceType() - 1], start->getSquarePosition(), target->getSquarePosition());
    BitBoardUtil::flipBits(colorBitboard[colorIndex], start->getSquarePosition(), target->getSquarePosition());
}

void Board::makeMove(Move move)
{
    Square *startSquare = board[move.start];
    Square *endSquare = board[move.target];
    Piece *piece = startSquare->getPiece();
    int colorIndex = piece->getPieceColor() == Piece::WHITE ? PieceList::whiteIndex : PieceList::blackIndex;
    int opponentIndex = 1 - colorIndex;
    int movedPieceType = piece->getPieceType();

    int oldEnPessentSquare = moveGeneration.possibleEnPassant;
    int originalType = piece->getPieceType();
    int originalPosition = piece->getPiecePosition();
    int perspective = whiteToMove ? 1 : -1;
    int startRank = move.start / 8;
    int endRank = move.target / 8;
    enPessentHistory.push(oldEnPessentSquare);

    Piece *capturedPiece = endSquare->getPiece();
    int capturePieceType = -1;
    // moves the piece and flips its bits
    movePiece(piece, startSquare, endSquare);

    zobristKeyHistory.push(zobristKey);

    CastlingRights oldRights;
    oldRights.blackCastleKingSide = moveGeneration.blackCastleKingSide;
    oldRights.blackCastleQueenSide = moveGeneration.blackCastleQueenSide;
    oldRights.whiteCastleKingSide = moveGeneration.whiteCastleKingSide;
    oldRights.whiteCastleQueenSide = moveGeneration.whiteCastleQueenSide;

    // capture
    if (capturedPiece != nullptr && !move.isEnPassant)
    {
        move.capturedPiece = capturedPiece;
        capturePieceType = capturedPiece->getPieceTypeRaw();
        pieceList.removePiece(capturedPiece);
        move.capture = true;

        // remove opponent piece from bitboard
        BitBoardUtil::flipBit(bitboards[opponentIndex][capturedPiece->getPieceType() - 1], move.target);
        BitBoardUtil::flipBit(colorBitboard[opponentIndex], move.target);
        // remove captured piece from zobrist key
        zobristKey ^= zobrist.getPieceHash(capturedPiece->getPieceType(), opponentIndex, move.target);

        if (capturedPiece->getPieceType() == Piece::ROOK)
        {
            int rank = capturedPiece->getPiecePosition() / 8;
            int file = capturedPiece->getPiecePosition() % 8;
            if (capturedPiece->getPieceColor() == Piece::WHITE)
            {
                // queen side  rook
                if (file == 0 && rank == 7)
                {
                    moveGeneration.whiteCastleQueenSide = false;
                }
                // king side  rook
                else if (file == 7 && rank == 7)
                {
                    moveGeneration.whiteCastleKingSide = false;
                }
            }
            else
            {
                if (file == 0 && rank == 0)
                {
                    moveGeneration.blackCastleQueenSide = false;
                }
                else if (file == 7 && rank == 0)
                {
                    moveGeneration.blackCastleKingSide = false;
                }
            }
        }
    }
    // en passant move
    else if (move.isEnPassant)
    {
        int offset = std::abs(move.start - move.target) == 9 ? -1 : 1;
        offset = offset * perspective;
        int pawnPos = move.start + offset;
        Piece *pawn = board[pawnPos]->getPiece();
        move.capturedPiece = pawn;
        pieceList.removePiece(pawn);
        board[pawnPos]->setPiece(nullptr);

        // remove pawn from bitboard
        BitBoardUtil::flipBit(bitboards[opponentIndex][pawn->getPieceType() - 1], pawnPos);
        BitBoardUtil::flipBit(colorBitboard[opponentIndex], pawnPos);
        // remove pawn from key
        zobristKey ^= zobrist.getPieceHash(pawn->getPieceType(), opponentIndex, pawnPos);
    }
    // pawn promotion
    if (piece->getPieceType() == Piece::PAWN && move.pawnPromotion)
    {
        // remove pawn from board
        pieceList.removePiece(piece);
        piece->promoteType(Piece::QUEEN);
        // removes it from the pawn list and adds it to it to its corresponding pieceList
        pieceList.addPiece(piece);

        // remove pawn from the pawn bitboard and add it to correct bitboard rook, queen, etc...
        BitBoardUtil::flipBit(bitboards[colorIndex][movedPieceType - 1], move.target);
        BitBoardUtil::flipBit(bitboards[colorIndex][piece->getPieceType() - 1], move.target);
    }
    // castle move
    if (move.isCastle)
    {
        int startFile = move.start % 8;
        int endFile = move.target % 8;
        if (endFile - startFile == 2) // kingside castle
        {
            int from = move.target + 1;
            int to = move.target - 1;
            Piece *rook = board[from]->getPiece();

            rook->setPiecePosition(to);
            board[to]->setPiece(rook);
            board[from]->setPiece(nullptr);
            // remove rook original position
            zobristKey ^= zobrist.getPieceHash(rook->getPieceType(), colorIndex, from);
            // add new rook position
            zobristKey ^= zobrist.getPieceHash(rook->getPieceType(), colorIndex, to);

            // remove rook from board and add it to new position
            BitBoardUtil::flipBits(bitboards[colorIndex][rook->getPieceType() - 1], from, to);
            BitBoardUtil::flipBits(colorBitboard[colorIndex], from, to);
        }
        else // queen side castle
        {
            int from = move.target - 2;
            int to = move.target + 1;
            Piece *rook = board[from]->getPiece();
            rook->setPiecePosition(to);
            board[from]->setPiece(nullptr);
            board[to]->setPiece(rook);

            // remove original rook position
            zobristKey ^= zobrist.getPieceHash(rook->getPieceType(), colorIndex, from);
            // add new rook position
            zobristKey ^= zobrist.getPieceHash(rook->getPieceType(), colorIndex, to);

            // remove rook from board and add it to new position
            BitBoardUtil::flipBits(bitboards[colorIndex][rook->getPieceType() - 1], from, to);
            BitBoardUtil::flipBits(colorBitboard[colorIndex], from, to);
        }
    }

    // enable en pessant
    if (piece->getPieceType() == Piece::PAWN && std::abs(startRank - endRank) == 2)
    {
        moveGeneration.possibleEnPassant = (move.start + move.target) / 2;
        move.possibleEnPassant = moveGeneration.possibleEnPassant;
        zobristKey ^= zobrist.getEnPessentFile(move.possibleEnPassant);
    }
    else
    {
        moveGeneration.possibleEnPassant = moveGeneration.noEnPessant;
        move.possibleEnPassant = moveGeneration.noEnPessant;
    }

    // remove original piece from zobrist key
    zobristKey ^= zobrist.getPieceHash(movedPieceType, colorIndex, move.start);
    // add target square to zobrist key
    zobristKey ^= zobrist.getPieceHash(piece->getPieceType(), colorIndex, move.target);
    // update side to move
    zobristKey ^= zobrist.sideToMove;

    if (oldEnPessentSquare != moveGeneration.noEnPessant)
    {
        // remove the old enPessent file
        zobristKey ^= zobrist.getEnPessentFile(oldEnPessentSquare);
    }

    // remove old castling rights
    updateCastlingRights(move);
    // black castle
    if (oldRights.blackCastleKingSide != moveGeneration.blackCastleKingSide)
        zobristKey ^= zobrist.castleRights[zobrist.blackKingSide];
    if (oldRights.blackCastleQueenSide != moveGeneration.blackCastleQueenSide)
        zobristKey ^= zobrist.castleRights[zobrist.blackQueenSide];

    // white castle
    if (oldRights.whiteCastleKingSide != moveGeneration.whiteCastleKingSide)
        zobristKey ^= zobrist.castleRights[zobrist.whiteKingSide];
    if (oldRights.whiteCastleQueenSide != moveGeneration.whiteCastleQueenSide)
        zobristKey ^= zobrist.castleRights[zobrist.whiteQueenSide];

    castlingHistory.push(oldRights);
    moveHistory.push(move);

    movesetHistory.push(moveGeneration.getMoves());
    whiteToMove = !whiteToMove;

    // std::cout << "Zobrist Key:  " << zobristKey << '\n';
    // zobrist.generateZobristKey(this);

    // std::cout << "Piece: " << move.pieceType << " " << move.start << " to " << move.target << " captured piece: " << capturePieceType << '\n';
}

Move Board::unmakeMove()
{
    if (moveHistory.empty())
        return {};
    // call a move swap function or something
    Move move = moveHistory.top();
    moveHistory.pop();
    Square *target = board[move.target];
    Square *start = board[move.start];
    Piece *movedPiece = target->getPiece();
    int friendlyIndex = movedPiece->getPieceColor() == Piece::WHITE ? PieceList::whiteIndex : PieceList::blackIndex;
    int opponentIndex = 1 - friendlyIndex;
    int movedPieceType = movedPiece->getPieceType();

    moveGeneration.possibleEnPassant = enPessentHistory.top();
    enPessentHistory.pop();
    zobristKey = zobristKeyHistory.top();
    zobristKeyHistory.pop();

    whiteToMove = !whiteToMove;
    int perspective = whiteToMove ? 1 : -1;

    // moves piece back to original square
    movePiece(movedPiece, target, start);

    // capture
    if (move.capture && !move.isEnPassant)
    {
        Piece *capturedPiece = move.capturedPiece;
        target->setPiece(capturedPiece);
        pieceList.addPiece(capturedPiece);

        // puts piece back on bitboard
        BitBoardUtil::flipBit(bitboards[opponentIndex][capturedPiece->getPieceType() - 1], move.target);
        BitBoardUtil::flipBit(colorBitboard[opponentIndex], move.target);
    }
    // en passant move
    else if (move.isEnPassant)
    {
        int offset = std::abs(move.start - move.target) == 9 ? -1 : 1;
        offset = offset * perspective;
        int pawnPos = offset + move.start;
        board[pawnPos]->setPiece(move.capturedPiece);
        pieceList.addPiece(move.capturedPiece);
        moveGeneration.possibleEnPassant = move.possibleEnPassant;

        // puts pawn back on bitBoard
        BitBoardUtil::flipBit(bitboards[opponentIndex][move.capturedPiece->getPieceType() - 1], pawnPos);
        BitBoardUtil::flipBit(colorBitboard[opponentIndex], pawnPos);
    }
    // pawn promotion
    if (move.pawnPromotion)
    {
        pieceList.removePiece(movedPiece);
        movedPiece->promoteType(Piece::PAWN);
        pieceList.addPiece(movedPiece);

        // piece already moved to correct position on colorboard, remove piece from promoted type and put it back into pawn bitboard
        BitBoardUtil::flipBit(bitboards[friendlyIndex][movedPieceType - 1], move.target);
        BitBoardUtil::flipBit(bitboards[friendlyIndex][movedPiece->getPieceType() - 1], move.target);
    }
    // castle move
    if (move.isCastle)
    {
        int startFile = move.start % 8;
        int endFile = move.target % 8;
        if (endFile - startFile == 2) // kingside castle
        {
            int from = move.target - 1;
            int to = move.target + 1;
            Piece *rook = board[from]->getPiece();
            rook->setPiecePosition(to);
            board[to]->setPiece(rook);
            board[from]->setPiece(nullptr);

            BitBoardUtil::flipBits(bitboards[friendlyIndex][rook->getPieceType() - 1], from, to);
            BitBoardUtil::flipBits(colorBitboard[friendlyIndex], from, to);
        }
        else // queen side castle
        {
            int from = move.target + 1;
            int to = move.target - 2;
            Piece *rook = board[from]->getPiece();
            rook->setPiecePosition(to);
            board[to]->setPiece(rook);
            board[from]->setPiece(nullptr);

            BitBoardUtil::flipBits(bitboards[friendlyIndex][rook->getPieceType() - 1], from, to);
            BitBoardUtil::flipBits(colorBitboard[friendlyIndex], from, to);
        }
    }
    CastlingRights castleHistory = castlingHistory.top();
    moveGeneration.blackCastleKingSide = castleHistory.blackCastleKingSide;
    moveGeneration.blackCastleQueenSide = castleHistory.blackCastleQueenSide;
    moveGeneration.whiteCastleKingSide = castleHistory.whiteCastleKingSide;
    moveGeneration.whiteCastleQueenSide = castleHistory.whiteCastleQueenSide;
    castlingHistory.pop();

    this->moveGeneration.setMoves(movesetHistory.top());
    movesetHistory.pop();

    return move;
}

void Board::updateCastlingRights(const Move &move)
{
    if (Piece::getPieceType(move.pieceType) == Piece::KING && Piece::getPieceColor(move.pieceType) == Piece::BLACK)
    {
        moveGeneration.blackCastleKingSide = false;
        moveGeneration.blackCastleQueenSide = false;
    }
    else if (Piece::getPieceType(move.pieceType) == Piece::KING && Piece::getPieceColor(move.pieceType) == Piece::WHITE)
    {
        moveGeneration.whiteCastleKingSide = false;
        moveGeneration.whiteCastleQueenSide = false;
    }
    else if (Piece::getPieceType(move.pieceType) == Piece::ROOK && Piece::getPieceColor(move.pieceType) == Piece::WHITE)
    {
        int startRow = move.start / 8;
        int startCol = move.start % 8;
        if (startCol == 0 && startRow == 7)
        {
            moveGeneration.whiteCastleQueenSide = false;
        }
        else if (startCol == 7 && startRow == 7)
        {
            moveGeneration.whiteCastleKingSide = false;
        }
    }
    else if (Piece::getPieceType(move.pieceType) == Piece::ROOK && Piece::getPieceColor(move.pieceType) == Piece::BLACK)
    {
        int startRow = move.start / 8;
        int startCol = move.start % 8;
        if (startCol == 0 && startRow == 0)
        {
            moveGeneration.blackCastleQueenSide = false;
        }
        else if (startCol == 7 && startRow == 0)
        {
            moveGeneration.blackCastleKingSide = false;
        }
    }
}

void Board::setSquarePiece(int idx, Piece *other)
{
    bool isWhite = other->getPieceColor() == Piece::WHITE;
    board[idx]->setPiece(other);
    pieceList.addPiece(other);

    if (other->getPieceType() == Piece::KING)
    {
        if (isWhite)
        {
            whiteKing = other;
        }
        else
        {
            blackKing = other;
        }
    }
}

void Board::initializeBitBoards()
{
    auto whitePieces = pieceList.getPieces(PieceList::whiteIndex);
    auto blackPieces = pieceList.getPieces(PieceList::blackIndex);

    for (int i = 0; i < PieceList::arrSize; i++)
    {
        // sets white bitboards
        for (int j = 0; j < whitePieces[i].size(); j++)
        {
            Piece *piece = whitePieces[i][j];
            int pieceType = piece->getPieceType();
            int position = piece->getPiecePosition();

            uint64_t &bitboard = bitboards[PieceList::whiteIndex][pieceType - 1];
            BitBoardUtil::setBit(bitboard, position);
        }

        // sets black bitboards
        for (int j = 0; j < blackPieces[i].size(); j++)
        {
            Piece *piece = whitePieces[i][j];
            int pieceType = piece->getPieceType();
            int position = piece->getPiecePosition();

            uint64_t &bitboard = bitboards[PieceList::blackIndex][pieceType - 1];
            BitBoardUtil::setBit(bitboard, position);
        }
    }

    uint64_t whiteBoard = bitboards[PieceList::whiteIndex][0] | bitboards[PieceList::whiteIndex][1] |
                          bitboards[PieceList::whiteIndex][2] | bitboards[PieceList::whiteIndex][3] |
                          bitboards[PieceList::whiteIndex][4] | bitboards[PieceList::whiteIndex][5];

    uint64_t blackBoard = bitboards[PieceList::blackIndex][0] | bitboards[PieceList::blackIndex][1] |
                          bitboards[PieceList::blackIndex][2] | bitboards[PieceList::blackIndex][3] |
                          bitboards[PieceList::blackIndex][4] | bitboards[PieceList::blackIndex][5];

    colorBitboard[PieceList::whiteIndex] = whiteBoard;
    colorBitboard[PieceList::blackIndex] = blackBoard;
}