
#include "Sokoban.h"
#include <fstream>
#include <algorithm>
#include <cmath>

void Sokoban::LoadMapFromFile(std::string fileName)
{
    std::ifstream file(fileName);
    std::string line;

    map.clear();
    park_positions.clear();

    while (std::getline(file, line))
    {
        std::vector<Field> row;

        for (char c : line)
        {
            Field field = Field::VOID;

            switch (c)
            {
            case 'X':
                field = Field::WALL;
                break;

            case '*':
                field = Field::VOID;
                break;

            case ' ':
                field = Field::FLOOR;
                break;

            case 'B':
                field = Field::BOX;
                break;

            case 'P':
                field = Field::PARK;
                break;

            case 'S':
                field = Field::PLAYER;
                player_position = sf::Vector2i(
                    static_cast<int>(row.size()),
                    static_cast<int>(map.size())
                );
                break;
            }

            if (c == 'P')
            {
                park_positions.push_back(sf::Vector2i(
                    static_cast<int>(row.size()),
                    static_cast<int>(map.size())
                ));
            }

            row.push_back(field);
        }

        map.push_back(row);
    }
}

void Sokoban::SetDrawParameters(sf::Vector2u size)
{
    if (map.empty() || map[0].empty())
        return;

    float width = size.x / static_cast<float>(map[0].size());
    float height = size.y / static_cast<float>(map.size());

    float tile = std::min(width, height);

    tile_size = sf::Vector2f(tile, tile);

    shift = sf::Vector2f(
        (size.x - tile * map[0].size()) / 2.0f,
        (size.y - tile * map.size()) / 2.0f
    );
}

void Sokoban::draw(
    sf::RenderTarget& target,
    sf::RenderStates states
) const
{
    sf::RectangleShape square;
    square.setSize(tile_size);
    square.setOutlineThickness(1);
    square.setOutlineColor(sf::Color::Black);

    for (int y = 0; y < static_cast<int>(map.size()); y++)
    {
        for (int x = 0; x < static_cast<int>(map[y].size()); x++)
        {
            switch (map[y][x])
            {
            case Field::VOID:
                square.setFillColor(sf::Color(30, 30, 30));
                break;

            case Field::FLOOR:
                square.setFillColor(sf::Color(220, 210, 180));
                break;

            case Field::WALL:
                square.setFillColor(sf::Color(100, 100, 100));
                break;

            case Field::BOX:
                square.setFillColor(sf::Color(160, 90, 40));
                break;

            case Field::PARK:
                square.setFillColor(sf::Color::Green);
                break;

            case Field::PLAYER:
                square.setFillColor(sf::Color::Blue);
                break;
            }

            square.setPosition(
                shift.x + x * tile_size.x,
                shift.y + y * tile_size.y
            );

            target.draw(square, states);
        }
    }
}

void Sokoban::Move_Player_Left()
{
    move_player(-1, 0);
}

void Sokoban::Move_Player_Right()
{
    move_player(1, 0);
}

void Sokoban::Move_Player_Up()
{
    move_player(0, -1);
}

void Sokoban::Move_Player_Down()
{
    move_player(0, 1);
}

void Sokoban::move_player(int dx, int dy)
{
    int x = player_position.x + dx;
    int y = player_position.y + dy;

    // Sprawdzamy granice planszy.
    if (y < 0 || y >= static_cast<int>(map.size()))
        return;

    if (x < 0 || x >= static_cast<int>(map[y].size()))
        return;

    Field next = map[y][x];

    // Nie przechodzimy przez ściany ani puste obszary.
    if (next == Field::WALL || next == Field::VOID)
        return;

    // Przesuwamy skrzynkę.
    if (next == Field::BOX)
    {
        int bx = x + dx;
        int by = y + dy;

        if (by < 0 || by >= static_cast<int>(map.size()))
            return;

        if (bx < 0 || bx >= static_cast<int>(map[by].size()))
            return;

        if (map[by][bx] != Field::FLOOR &&
            map[by][bx] != Field::PARK)
            return;

        map[by][bx] = Field::BOX;
    }

    // Przesuwamy gracza.
    map[player_position.y][player_position.x] = Field::FLOOR;

    player_position = sf::Vector2i(x, y);
    map[y][x] = Field::PLAYER;

    // Przywracamy miejsca na skrzynki.
    for (auto park : park_positions)
    {
        if (map[park.y][park.x] == Field::FLOOR)
            map[park.y][park.x] = Field::PARK;
    }
}

bool Sokoban::Is_Victory() const
{
    for (auto park : park_positions)
    {
        if (map[park.y][park.x] != Field::BOX)
            return false;
    }

    return true;
}
