#include "NewMoveGenerator.hpp"
#include "Board.hpp"
#include "BitBoardUtil.hpp"
#include <span>

std::vector<Move> &NewMoveGenerator::generateMoves(Board *board, bool includeQuietMoves /*=true*/)
{
    this->board = board;
    genQuiets = includeQuietMoves;

    Init();
    generateKingMoves();

    // Only king moves are valid in a double check position, so can return early.
    if (inDoubleCheck)
    {
        moves.resize(currentMoveIndex);
        moves.shrink_to_fit();
        return moves;
    }

    generateSlidingPieceMoves();
    generateKnightMoves();
    generatePawnMoves();

    moves.resize(currentMoveIndex);
    moves.shrink_to_fit();
    return moves;
}

void NewMoveGenerator::Init()
{
    moves = std::vector<Move>(MAX_MOVES);
    currentMoveIndex = 0;
    inCheck = false;
    inDoubleCheck = false;
    pinsExistInPosition = false;
    checkRayBitmask = 0;
    pinRayBitmask = 0;

    isWhiteToMove = board->getWhiteToMove();
    friendlyColor = isWhiteToMove ? Piece::WHITE : Piece::BLACK;
    opponentColor = isWhiteToMove ? Piece::BLACK : Piece::WHITE;
    friendlyColorIndex = isWhiteToMove ? PieceList::whiteIndex : PieceList::blackIndex;
    friendlyKing = board->getKing(friendlyColorIndex);
    opponentColorIndex = 1 - friendlyColorIndex;

    // bitboard stuff
    friendlyPieces = board->colorBitboard[friendlyColorIndex];
    opponentPieces = board->colorBitboard[opponentColorIndex];
    allPieces = friendlyPieces | opponentPieces;
    emptySquares = ~allPieces;
    emptyOrEnemySquares = emptySquares | opponentPieces;
    moveTypeMask = genQuiets ? UINT64_MAX : opponentPieces;
    calculateAttackData();
}

void NewMoveGenerator::generateKingMoves()
{
    int friendlyKingPos = friendlyKing->getPiecePosition();

    uint64_t legalMask = ~(opponentAttackMap | friendlyPieces);
    uint64_t kingMoves = (preComputedMoveData.kingAttackBitboards[friendlyKingPos] & legalMask & moveTypeMask);

    while (kingMoves != 0)
    {
        int targetSquare = BitBoardUtil::PopLSB(kingMoves);

        moves[currentMoveIndex++] = Move(friendlyKingPos, targetSquare, friendlyKing->getPieceTypeRaw());
    }

    getCastleMoves();
}
void NewMoveGenerator::getCastleMoves()
{
    if (inCheck || !genQuiets)
        return;

    int currLocation = friendlyKing->getPiecePosition();
    if ((Board::whiteToMove && whiteCastleKingSide && friendlyKing->getPieceColor() == Piece::WHITE) || (!Board::whiteToMove && blackCastleKingSide && friendlyKing->getPieceColor() == Piece::BLACK)) // kingside castle
    {
        if (board->getBoard()[currLocation + 1]->hasNullPiece() && board->getBoard()[currLocation + 2]->hasNullPiece())
        {
            if (!squareIsAttacked(currLocation + 1) && !squareIsAttacked(currLocation + 2))
            {
                Move move(currLocation, currLocation + 2, friendlyKing->getPieceTypeRaw(), false, true);
                move.pieceType = friendlyKing->getPieceTypeRaw();
                moves[currentMoveIndex++] = move;
            }
        }
    }
    if ((Board::whiteToMove && whiteCastleQueenSide) || (!Board::whiteToMove && blackCastleQueenSide)) // queenside castle
    {
        if (board->getBoard()[currLocation - 1]->hasNullPiece() && board->getBoard()[currLocation - 2]->hasNullPiece() && board->getBoard()[currLocation - 3]->hasNullPiece())
        {
            if (!squareIsAttacked(currLocation - 1) && !squareIsAttacked(currLocation - 2))
            {
                Move move(currLocation, currLocation - 2, friendlyKing->getPieceTypeRaw(), false, true);
                move.pieceType = friendlyKing->getPieceTypeRaw();
                moves[currentMoveIndex++] = move;
            }
        }
    }
}
bool NewMoveGenerator::containsSquareInPawnAttackMap(int square)
{
    return ((opponentPawnAttackMap >> square) & 1) != 0;
}

bool NewMoveGenerator::squareIsAttacked(int square)
{
    return ((opponentAttackMap >> square) & 1) != 0;
}

bool NewMoveGenerator::squareIsInCheckRay(int square)
{
    return inCheck && ((checkRayBitmask >> square) & 1) != 0;
}
bool NewMoveGenerator::containsSquare(uint64_t bitboard, int square)
{
    return ((bitboard >> square) & 1) != 0;
}

void NewMoveGenerator::calculateAttackData()
{
    genSlidingAttackMap();
    // Search squares in all directions around friendly king for checks/pins by enemy sliding pieces (queen, rook, bishop)
    int startDirIndex = 0;
    int endDirIndex = 8;
    auto opponentPieces = board->pieceList.getPieces(opponentColorIndex);
    int friendlyKingSquare = friendlyKing->getPiecePosition();

    if (opponentPieces[PieceList::queenIndex].size() == 0)
    {
        startDirIndex = (opponentPieces[PieceList::rookIndex].size() > 0) ? 0 : 4;
        endDirIndex = (opponentPieces[PieceList::bishopIndex].size() > 0) ? 8 : 4;
    }

    for (int dir = startDirIndex; dir < endDirIndex; dir++)
    {
        bool isDiagonal = dir > 3;

        int n = preComputedMoveData.numSquaresToEdge[friendlyKingSquare][dir];
        int directionOffset = slidingMovesOffsets[dir];
        bool isFriendlyPieceAlongRay = false;
        uint64_t rayMask = 0;

        for (int i = 0; i < n; i++)
        {
            int squareIndex = friendlyKingSquare + directionOffset * (i + 1);
            rayMask |= (uint64_t)1 << squareIndex;
            Piece *piece = board->getBoard()[squareIndex]->getPiece();

            // This square contains a piece
            if (piece != nullptr)
            {
                if (piece->getPieceColor() == friendlyColor)
                {
                    // First friendly piece we have come across in this direction, so it might be pinned
                    if (!isFriendlyPieceAlongRay)
                    {
                        isFriendlyPieceAlongRay = true;
                    }
                    // This is the second friendly piece we've found in this direction, therefore pin is not possible
                    else
                    {
                        break;
                    }
                }
                // This square contains an enemy piece
                else
                {
                    int pieceType = piece->getPieceType();

                    // Check if piece is in bitmask of pieces able to move in current direction
                    if ((isDiagonal && (pieceType == Piece::QUEEN || pieceType == Piece::BISHOP)) || (!isDiagonal && ((pieceType == Piece::QUEEN || pieceType == Piece::ROOK))))
                    {
                        // Friendly piece blocks the check, so this is a pin
                        if (isFriendlyPieceAlongRay)
                        {
                            pinsExistInPosition = true;
                            pinRayBitmask |= rayMask;
                        }
                        // No friendly piece blocking the attack, so this is a check
                        else
                        {
                            checkRayBitmask |= rayMask;
                            inDoubleCheck = inCheck; // if already in check, then this is double check
                            inCheck = true;
                        }
                        break;
                    }
                    else
                    {
                        // This enemy piece is not able to move in the current direction, and so is blocking any checks/pins
                        break;
                    }
                }
            }
        }
        // Stop searching for pins if in double check, as the king is the only piece able to move in that case anyway
        if (inDoubleCheck)
        {
            break;
        }
    }

    // Knight attacks
    auto opponentKnights = opponentPieces[PieceList::knightIndex];
    opponentKnightAttacks = 0;
    bool isKnightCheck = false;

    for (int knightIndex = 0; knightIndex < opponentKnights.size(); knightIndex++)
    {
        int startSquare = opponentKnights[knightIndex]->getPiecePosition();
        opponentKnightAttacks |= preComputedMoveData.knightAttackBitboards[startSquare];

        if (!isKnightCheck && containsSquare(opponentKnightAttacks, friendlyKingSquare))
        {
            isKnightCheck = true;
            inDoubleCheck = inCheck; // if already in check, then this is double check
            inCheck = true;
            checkRayBitmask |= (uint64_t)1 << startSquare;
        }
    }

    // Pawn attacks
    auto opponentPawns = opponentPieces[PieceList::pawnIndex];
    opponentPawnAttackMap = 0;
    bool isPawnCheck = false;

    for (int pawnIndex = 0; pawnIndex < opponentPawns.size(); pawnIndex++)
    {
        int pawnSquare = opponentPawns[pawnIndex]->getPiecePosition();
        opponentPawnAttackMap |= preComputedMoveData.pawnAttackBitboards[pawnSquare][opponentColorIndex];

        if (!isPawnCheck && containsSquare(opponentPawnAttackMap, friendlyKingSquare))
        {
            isPawnCheck = true;
            inDoubleCheck = inCheck; // if already in check, then this is double check
            inCheck = true;
            checkRayBitmask |= (uint64_t)1 << pawnSquare;
        }
    }

    int enemyKingSquare = board->getKing(opponentColorIndex)->getPiecePosition();

    opponentAttackMapNoPawns = opponentSlidingAttackMap | opponentKnightAttacks | preComputedMoveData.kingAttackBitboards[enemyKingSquare];
    opponentAttackMap = opponentAttackMapNoPawns | opponentPawnAttackMap;
}

void NewMoveGenerator::genSlidingAttackMap()
{
    opponentSlidingAttackMap = 0;

    auto pieces = board->pieceList.getPieces(opponentColorIndex);

    for (int i = 0; i < pieces[PieceList::rookIndex].size(); i++)
    {
        updateSlidingAttackPiece(pieces[PieceList::rookIndex][i]->getPiecePosition(), 0, 4);
    }

    for (int i = 0; i < pieces[PieceList::queenIndex].size(); i++)
    {
        updateSlidingAttackPiece(pieces[PieceList::queenIndex][i]->getPiecePosition(), 0, 8);
    }

    for (int i = 0; i < pieces[PieceList::bishopIndex].size(); i++)
    {
        updateSlidingAttackPiece(pieces[PieceList::bishopIndex][i]->getPiecePosition(), 4, 8);
    }
}

void NewMoveGenerator::updateSlidingAttackPiece(int startSquare, int startDirIndex, int endDirIndex)
{

    for (int directionIndex = startDirIndex; directionIndex < endDirIndex; directionIndex++)
    {
        int currentDirOffset = slidingMovesOffsets[directionIndex];
        for (int n = 0; n < preComputedMoveData.numSquaresToEdge[startSquare][directionIndex]; n++)
        {
            int targetSquare = startSquare + currentDirOffset * (n + 1);
            Piece *targetSquarePiece = board->getBoard()[targetSquare]->getPiece();
            opponentSlidingAttackMap |= (uint64_t)1 << targetSquare;
            if (targetSquare != friendlyKing->getPiecePosition())
            {
                if (targetSquarePiece != nullptr)
                {
                    break;
                }
            }
        }
    }
}

void NewMoveGenerator::generateSlidingPieceMoves()
{
    int friendlyKingSquare = friendlyKing->getPiecePosition();

    uint64_t legalMask = emptyOrEnemySquares & moveTypeMask;
    // If this piece is pinned, and the king is in check, this piece cannot move

    uint64_t orthogonalSliders = board->bitboards[friendlyColorIndex][PieceList::rookIndex] | board->bitboards[friendlyColorIndex][PieceList::queenIndex];
    uint64_t diagonalSliders = board->bitboards[friendlyColorIndex][PieceList::bishopIndex] | board->bitboards[friendlyColorIndex][PieceList::queenIndex];

    if (inCheck)
    {
        orthogonalSliders &= ~pinRayBitmask;
        diagonalSliders &= ~pinRayBitmask;
        legalMask &= checkRayBitmask;
    }

    // orthogonal
    while (orthogonalSliders != 0)
    {
        int startSquare = BitBoardUtil::PopLSB(orthogonalSliders);
        uint64_t moveSquares = magic.getrookAttacks(startSquare, allPieces) & legalMask;
        if (isPinned(startSquare))
        {
            moveSquares &= pinRayBitmask;
        }

        while (moveSquares != 0)
        {
            int targetSquare = BitBoardUtil::PopLSB(moveSquares);
            moves[currentMoveIndex++] = Move(startSquare, targetSquare);
        }
    }
    // Diag
    while (diagonalSliders != 0)
    {
        int startSquare = BitBoardUtil::PopLSB(diagonalSliders);
        uint64_t moveSquares = magic.getBishopAttacks(startSquare, allPieces) & legalMask;
        if (isPinned(startSquare))
        {
            moveSquares &= pinRayBitmask;
        }

        while (moveSquares != 0)
        {
            int targetSquare = BitBoardUtil::PopLSB(moveSquares);
            moves[currentMoveIndex++] = Move(startSquare, targetSquare);
        }
    }
}

void NewMoveGenerator::generateKnightMoves()
{
    auto pieces = board->pieceList.getPieces(friendlyColorIndex);
    auto myKnights = pieces[PieceList::knightIndex];
    uint64_t knights = board->bitboards[friendlyColorIndex][PieceList::knightIndex] & ~pinRayBitmask;
    uint64_t mask = emptyOrEnemySquares & moveTypeMask;
    int pieceType = Piece::KNIGHT | Board::whiteToMove ? Piece::WHITE : Piece::BLACK;

    if (inCheck)
    {
        mask &= checkRayBitmask;
    }

    while (knights != 0)
    {
        int knightPosition = BitBoardUtil::PopLSB(knights);
        uint64_t moveSquares = preComputedMoveData.knightAttackBitboards[knightPosition] & mask;
        while (moveSquares != 0)
        {
            int target = BitBoardUtil::PopLSB(moveSquares);
            moves[currentMoveIndex++] = Move(knightPosition, target, pieceType);
        }
    }
}
bool NewMoveGenerator::isPinned(int square)
{
    return pinsExistInPosition && ((pinRayBitmask >> square) & 1) != 0;
}

void NewMoveGenerator::generatePawnMoves()
{
    auto myPawns = board->pieceList.getPieces(friendlyColorIndex)[PieceList::pawnIndex];
    int friendlyKingSquare = friendlyKing->getPiecePosition();
    int pawnOffset = (friendlyColor == Piece::WHITE) ? -8 : 8;
    int startRank = (Board::whiteToMove) ? 6 : 1;
    int finalRankBeforePromotion = (Board::whiteToMove) ? 1 : 6;

    for (int i = 0; i < myPawns.size(); i++)
    {
        int startSquare = myPawns[i]->getPiecePosition();
        int rank = startSquare / 8;
        bool oneStepFromPromotion = rank == finalRankBeforePromotion;
        Piece *pawnPiece = myPawns[i];

        if (genQuiets)
        {

            int squareOneForward = startSquare + pawnOffset;

            // Square ahead of pawn is empty: forward moves
            if (board->getBoard()[squareOneForward]->hasNullPiece())
            {
                // Pawn not pinned, or is moving along line of pin
                if (!isPinned(startSquare) || containsSquare(pinRayBitmask, squareOneForward))
                {
                    // Not in check, or pawn is interposing checking piece
                    if (!inCheck || squareIsInCheckRay(squareOneForward))
                    {
                        if (oneStepFromPromotion)
                        {
                            makePromotionMoves(startSquare, squareOneForward);
                        }
                        else
                        {
                            moves[currentMoveIndex++] = Move(startSquare, squareOneForward, myPawns[i]->getPieceTypeRaw());
                        }
                    }

                    // Is on starting square (so can move two forward if not blocked)
                    if (rank == startRank)
                    {
                        int squareTwoForward = squareOneForward + pawnOffset;
                        if (board->getBoard()[squareTwoForward]->hasNullPiece())
                        {
                            // Not in check, or pawn is interposing checking piece
                            if (!inCheck || squareIsInCheckRay(squareTwoForward))
                            {
                                // flag en pessent right here
                                moves[currentMoveIndex++] = Move(startSquare, squareTwoForward, pawnPiece->getPieceTypeRaw());
                            }
                        }
                    }
                }
            }
        }

        // pawn attacks
        uint64_t pawnAttack = preComputedMoveData.pawnAttackBitboards[startSquare][friendlyColorIndex];
        pawnAttack &= board->colorBitboard[opponentColorIndex] & moveTypeMask;

        if (inCheck)
        {
            pawnAttack &= checkRayBitmask;
        }
        if (isPinned(startSquare))
        {
            pawnAttack &= pinRayBitmask;
        }

        while (pawnAttack != 0)
        {
            int targetSquare = BitBoardUtil::PopLSB(pawnAttack);
            if (oneStepFromPromotion)
            {
                makePromotionMoves(startSquare, targetSquare);
            }
            else
            {
                moves[currentMoveIndex++] = Move(startSquare, targetSquare, myPawns[i]->getPieceTypeRaw());
            }
        }

        for (int j = 0; j < 2; j++)
        {
            // Check if square exists diagonal to pawn
            if (preComputedMoveData.numSquaresToEdge[startSquare][preComputedMoveData.pawnAttackDirections[friendlyColorIndex][j]] > 0)
            {
                int pawnCaptureDir = preComputedMoveData.directionOffsets[preComputedMoveData.pawnAttackDirections[friendlyColorIndex][j]];
                int targetSquare = startSquare + pawnCaptureDir;
                Piece *targetPiece = board->getBoard()[targetSquare]->getPiece();

                // If piece is pinned, and the square it wants to move to is not on same line as the pin, then skip this direction
                if (isPinned(startSquare) && !containsSquare(pinRayBitmask, targetSquare))
                {
                    continue;
                }

                // Capture en-passant
                if (targetSquare == possibleEnPassant)
                {
                    int epCapturedPawnSquare = targetSquare + ((Board::whiteToMove) ? 8 : -8);
                    if (!inCheckAfterEnPassant(startSquare, targetSquare, epCapturedPawnSquare))
                    {
                        moves[currentMoveIndex++] = Move(startSquare, targetSquare, myPawns[i]->getPieceTypeRaw(), true);
                    }
                }
            }
        }
    }
}
void NewMoveGenerator::makePromotionMoves(int start, int target)
{
    // knight rook bishop queen
    Move knight(start, target);
    knight.promotionPieceType = Piece::KNIGHT;
    knight.pawnPromotion = true;
    Move rook(start, target);
    rook.pawnPromotion = true;
    rook.promotionPieceType = Piece::ROOK;
    Move bishop(start, target);
    bishop.pawnPromotion = true;
    bishop.promotionPieceType = Piece::BISHOP;
    Move queen(start, target);
    queen.pawnPromotion = true;
    queen.promotionPieceType = Piece::QUEEN;

    moves[currentMoveIndex++] = queen;
    moves[currentMoveIndex++] = rook;
    moves[currentMoveIndex++] = bishop;
    moves[currentMoveIndex++] = knight;
}

bool NewMoveGenerator::inCheckAfterEnPassant(int startSquare, int targetSquare, int epCapturedPawnSquare)
{
    // Update board to reflect en-passant capture
    Piece *movedPawn = board->getBoard()[startSquare]->getPiece();
    Piece *capturedPawn = board->getBoard()[epCapturedPawnSquare]->getPiece();
    board->getBoard()[targetSquare]->setPiece(movedPawn);
    board->getBoard()[startSquare]->setPiece(nullptr);
    board->getBoard()[epCapturedPawnSquare]->setPiece(nullptr);

    bool inCheckAfterEpCapture = false;
    if (squareAttackedAfterEPCapture(epCapturedPawnSquare, startSquare))
    {
        inCheckAfterEpCapture = true;
    }

    // Undo change to board
    board->getBoard()[startSquare]->setPiece(movedPawn);
    board->getBoard()[targetSquare]->setPiece(nullptr);
    board->getBoard()[epCapturedPawnSquare]->setPiece(capturedPawn);
    return inCheckAfterEpCapture;
}

bool NewMoveGenerator::squareAttackedAfterEPCapture(int epCaptureSquare, int capturingPawnStartSquare)
{
    int friendlyKingSquare = friendlyKing->getPiecePosition();
    if (containsSquare(opponentAttackMapNoPawns, friendlyKingSquare))
    {
        return true;
    }

    // Loop through the horizontal direction towards ep capture to see if any enemy piece now attacks king
    int dirIndex = (epCaptureSquare < friendlyKingSquare) ? 1 : 0;
    for (int i = 0; i < preComputedMoveData.numSquaresToEdge[friendlyKingSquare][dirIndex]; i++)
    {
        int squareIndex = friendlyKingSquare + preComputedMoveData.directionOffsets[dirIndex] * (i + 1);
        Piece *piece = board->getBoard()[squareIndex]->getPiece();
        if (piece != nullptr)
        {
            // Friendly piece is blocking view of this square from the enemy.
            if (piece->getPieceColor() == friendlyColor)
            {
                break;
            }
            // This square contains an enemy piece
            else
            {
                if (piece->getPieceType() == Piece::ROOK || piece->getPieceType() == Piece::QUEEN)
                {
                    return true;
                }
                else
                {
                    // This piece is not able to move in the current direction, and is therefore blocking any checks along this line
                    break;
                }
            }
        }
    }

    // check if enemy pawn is contro lling this square (can't use pawn attack bitboard, because pawn has been captured)
    for (int i = 0; i < 2; i++)
    {
        // Check if square exists diagonal to friendly king from which enemy pawn could be attacking it
        if (preComputedMoveData.numSquaresToEdge[friendlyKingSquare][preComputedMoveData.pawnAttackDirections[friendlyColorIndex][i]] > 0)
        {
            // move in direction friendly pawns attack to get square from which enemy pawn would attack
            Piece *piece = board->getBoard()[friendlyKingSquare + preComputedMoveData.directionOffsets[preComputedMoveData.pawnAttackDirections[friendlyColorIndex][i]]]->getPiece();
            if (piece != nullptr && piece->getPieceColor() == opponentColor) // is enemy pawn
            {
                return true;
            }
        }
    }

    return false;
}
