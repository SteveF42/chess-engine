#ifndef PieceSquareTable_H
#define PieceSquareTable_H

class PieceSquareTable
{
public:
	static int read(const int table[], int square, bool isWhite)
	{
		if (!isWhite)
		{
			square = 63 - square;
			// int rank = 7 - (square / 8);
			// int file = square % 8;
			// square = (rank * 8) + file;
		}

		return table[square];
	}

	static int read(const int pieceType, int square)
	{
		switch (pieceType)
		{
		case Piece::PAWN:
			return pawnsEnd[square];
		case Piece::KNIGHT:
			return knights[square];
		case Piece::BISHOP:
			return bishops[square];
		case Piece::ROOK:
			return rooks[square];
		case Piece::QUEEN:
			return queens[square];
		default:
			return 0;
		}
	}

	const static int pawns[];

	const static int pawnsEnd[];

	const static int knights[];

	const static int bishops[];

	const static int rooks[];

	const static int queens[];

	const static int kingStart[];

	const static int kingEnd[];
};

const int PieceSquareTable::pawns[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	50, 50, 50, 50, 50, 50, 50, 50,
	10, 10, 20, 30, 30, 20, 10, 10,
	5, 5, 10, 25, 25, 10, 5, 5,
	0, 0, 0, 20, 20, 0, 0, 0,
	5, -5, -10, 0, 0, -10, -5, 5,
	5, 10, 10, -20, -20, 10, 10, 5,
	0, 0, 0, 0, 0, 0, 0, 0};
const int PieceSquareTable::pawnsEnd[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	80, 80, 80, 80, 80, 80, 80, 80,
	50, 50, 50, 50, 50, 50, 50, 50,
	30, 30, 30, 30, 30, 30, 30, 30,
	20, 20, 20, 20, 20, 20, 20, 20,
	10, 10, 10, 10, 10, 10, 10, 10,
	10, 10, 10, 10, 10, 10, 10, 10,
	0, 0, 0, 0, 0, 0, 0, 0};

const int PieceSquareTable::knights[] = {
	-50,
	-40,
	-30,
	-30,
	-30,
	-30,
	-40,
	-50,
	-40,
	-20,
	0,
	0,
	0,
	0,
	-20,
	-40,
	-30,
	0,
	10,
	15,
	15,
	10,
	0,
	-30,
	-30,
	5,
	15,
	20,
	20,
	15,
	5,
	-30,
	-30,
	0,
	15,
	20,
	20,
	15,
	0,
	-30,
	-30,
	5,
	10,
	15,
	15,
	10,
	5,
	-30,
	-40,
	-20,
	0,
	5,
	5,
	0,
	-20,
	-40,
	-50,
	-40,
	-30,
	-30,
	-30,
	-30,
	-40,
	-50,
};

const int PieceSquareTable::bishops[] = {
	-20,
	-10,
	-10,
	-10,
	-10,
	-10,
	-10,
	-20,
	-10,
	0,
	0,
	0,
	0,
	0,
	0,
	-10,
	-10,
	0,
	5,
	10,
	10,
	5,
	0,
	-10,
	-10,
	5,
	5,
	10,
	10,
	5,
	5,
	-10,
	-10,
	0,
	10,
	10,
	10,
	10,
	0,
	-10,
	-10,
	10,
	10,
	10,
	10,
	10,
	10,
	-10,
	-10,
	5,
	0,
	0,
	0,
	0,
	5,
	-10,
	-20,
	-10,
	-10,
	-10,
	-10,
	-10,
	-10,
	-20,
};

const int PieceSquareTable::rooks[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 10, 10, 10, 10, 10, 10, 5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	0, 0, 0, 5, 5, 0, 0, 0};

const int PieceSquareTable::queens[] = {
	-20, -10, -10, -5, -5, -10, -10, -20,
	-10, 0, 0, 0, 0, 0, 0, -10,
	-10, 0, 5, 5, 5, 5, 0, -10,
	-5, 0, 5, 5, 5, 5, 0, -5,
	0, 0, 5, 5, 5, 5, 0, -5,
	-10, 5, 5, 5, 5, 5, 0, -10,
	-10, 0, 5, 0, 0, 0, 0, -10,
	-20, -10, -10, -5, -5, -10, -10, -20};

const int PieceSquareTable::kingStart[] = {
	-80, -70, -70, -70, -70, -70, -70, -80,
	-60, -60, -60, -60, -60, -60, -60, -60,
	-40, -50, -50, -60, -60, -50, -50, -40,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-20, -30, -30, -40, -40, -30, -30, -20,
	-10, -20, -20, -20, -20, -20, -20, -10,
	20, 20, -5, -5, -5, -5, 20, 20,
	20, 30, 10, 0, 0, 10, 30, 20};

const int PieceSquareTable::kingEnd[] = {
	-20, -10, -10, -10, -10, -10, -10, -20,
	-5, 0, 5, 5, 5, 5, 0, -5,
	-10, -5, 20, 30, 30, 20, -5, -10,
	-15, -10, 35, 45, 45, 35, -10, -15,
	-20, -15, 30, 40, 40, 30, -15, -20,
	-25, -20, 20, 25, 25, 20, -20, -25,
	-30, -25, 0, 0, 0, 0, -25, -30,
	-50, -30, -30, -30, -30, -30, -30, -50};

#endif