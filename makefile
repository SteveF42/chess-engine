all: compile link run

compile: 
	g++ -std=c++17 -c main.cpp ./src/*.cpp -I"/opt/homebrew/Cellar/sfml/2.6.1/include" -I"./include" -DSFML_STATIC -g -O2

link:
	g++ *.o -o main -L"/opt/homebrew/Cellar/sfml/2.6.1/lib" -lsfml-window -lsfml-system -lsfml-graphics

clean:
	rm -f main *.o

run: 
	./main

# -mwindows no debug in link