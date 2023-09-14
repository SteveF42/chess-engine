all: compile link run

compile: 
	g++ -c *.cpp -I"C:\msys64\mingw64\SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit\SFML-2.5.1\include" -DSFML_STATIC -g -O2

link:
	g++ *.o -o main -L"C:\msys64\mingw64\SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit\SFML-2.5.1\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -lsfml-main

clean:
	rm -f main *.o

run: 
	./main -f r1bq1rk1/ppp2ppp/3bp1n1/8/3PQ3/2N3P1/PP3P1P/R3KBNR w KQ - 3 13
# -mwindows no debug in link