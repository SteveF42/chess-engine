#include <SFML/Graphics.hpp>

const int BOARDSIZE = 8;
const float SQUARESIZE = 25.f;
const sf::Color LIGHTSQUARE = sf::Color(210,140,69);
const sf::Color DARKSQUARE = sf::Color(255,207,159);
const float screenOffsetMultiplyer = 1.5;

int main()
{
    sf::RenderWindow window(sf::VideoMode(200 * screenOffsetMultiplyer, 200), "chess engine!");
    sf::CircleShape shape(100.f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.setSize(sf::Vector2u(1000.f*screenOffsetMultiplyer, 1000.f));
        for (int rank = 0; rank < BOARDSIZE; rank++)
        {
            for (int file = 0; file < BOARDSIZE; file++)
            {
                sf::RectangleShape rectangle(sf::Vector2(SQUARESIZE, SQUARESIZE));
                auto pos = sf::Vector2((file * SQUARESIZE), rank * SQUARESIZE);
                rectangle.setPosition(pos);
                rectangle.setFillColor(DARKSQUARE);
                if (file % 2 == 1 && rank % 2 == 0)
                {
                    rectangle.setFillColor(LIGHTSQUARE);
                }
                else if (file % 2 == 0 && rank % 2 == 1)
                {
                    rectangle.setFillColor(LIGHTSQUARE);
                }
                window.draw(rectangle);
            }
        }

        window.display();
    }

    return 0;
}