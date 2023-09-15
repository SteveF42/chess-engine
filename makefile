all: compile link run

compile: 
	g++ -c *.cpp -I"C:\msys64\mingw64\SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit\SFML-2.5.1\include" -DSFML_STATIC -g -O2

link:
	g++ *.o -o main -L"C:\msys64\mingw64\SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit\SFML-2.5.1\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -lsfml-main

clean:
	rm -f main *.o

run: 
	./main
# -mwindows no debug in link