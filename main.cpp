#include<string>
#include<iostream>
#include"GameEngine.hpp"
#include"ReadFen.hpp"

int main()
{
    GameEngine engine;
    while(engine.isActive()){
        engine.update();
    }

    Position s = ReadFen::readFenString(ReadFen::startingString);
    for(auto i : s.board){
        std::cout << i;
    }
    return 0;
}