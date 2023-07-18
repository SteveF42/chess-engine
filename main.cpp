#include <string>
#include <iostream>
#include "GameEngine.hpp"
#include "ReadFen.hpp"

int main(int argc, char const *argv[])
{

    std::string startingFenString = ReadFen::startingString;
    if (argc > 1)
    {
        std::string s = argv[1];
        if (s == "-f" || s == "-fen")
        {
            int index = 2;
            std::string input = "";
            input += argv[index++];
            while (index < argc)
            {
                input += " " + std::string(argv[index++]);
            }
            std::cout << "loading board position " << input << '\n';
            startingFenString = input;
        }
    }

    GameEngine engine(startingFenString);
    while (engine.isActive())
    {
        engine.update();
    }
    return 0;
}