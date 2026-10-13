
#include <SFML/Graphics.hpp>
#include "Sokoban.h"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(800, 600),
        "Sokoban"
    );

    Sokoban game;

    game.LoadMapFromFile("plansza.txt");
    game.SetDrawParameters(window.getSize());

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized)
            {
                window.setView(sf::View(
                    sf::FloatRect(
                        0, 0,
                        event.size.width,
                        event.size.height
                    )
                ));

                game.SetDrawParameters(window.getSize());
            }

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Left:
                    game.Move_Player_Left();
                    break;

                case sf::Keyboard::Right:
                    game.Move_Player_Right();
                    break;

                case sf::Keyboard::Up:
                    game.Move_Player_Up();
                    break;

                case sf::Keyboard::Down:
                    game.Move_Player_Down();
                    break;

                case sf::Keyboard::Escape:
                    window.close();
                    break;

                default:
                    break;
                }

                if (game.Is_Victory())
                    window.setTitle("Sokoban - Wygrana!");
            }
        }

        window.clear(sf::Color::Black);
        window.draw(game);
        window.display();
    }

    return 0;
}
