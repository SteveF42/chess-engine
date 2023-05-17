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
    return 0;
}