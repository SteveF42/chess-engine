all: compile link run

objects = ../Board.o ../PieceList.o ../Square.o ../NewMoveGenerator.o \
		../Zobrist.o ../Magic.o ../MagicHelper.o ../Piece.o ../PreComputedMoveData.o \
		../ReadFen.o ../Square.o ../TranspositionTable.o ../AI.o

compile:
	g++ -std=c++20 -Wall -c test.cpp
link:
	g++ catch_amalgamated.o test.o $(objects) -o test 
run: 
	./test --success

clean:
	rm *.o