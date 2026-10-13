
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class Field
{
    VOID,
    FLOOR,
    WALL,
    BOX,
    PARK,
    PLAYER
};

class Sokoban : public sf::Drawable
{
public:
    void LoadMapFromFile(std::string fileName);
    void SetDrawParameters(sf::Vector2u size);

    void Move_Player_Left();
    void Move_Player_Right();
    void Move_Player_Up();
    void Move_Player_Down();

    bool Is_Victory() const;

private:
    std::vector<std::vector<Field>> map;
    std::vector<sf::Vector2i> park_positions;

    sf::Vector2i player_position;
    sf::Vector2f shift;
    sf::Vector2f tile_size;

    void move_player(int dx, int dy);

    virtual void draw(
        sf::RenderTarget& target,
        sf::RenderStates states
    ) const override;
};
