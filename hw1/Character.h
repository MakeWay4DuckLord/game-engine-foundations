#pragma once
#include <SFML/Graphics.hpp>

/**
 * Character represents a RectangleShape that moves based on keyboard input.
 * The character moves horizontally with the a and d keys, moves upward with w, 
 * and moves down if w isn't pressed, to create a kind of gravity
 */
class Character: public sf::RectangleShape
{
    public:
        //might make an interface (or i suppose abstract class cause c++) for objects that has update()
        void update();
        Character(sf::Vector2f size, sf::Vector2f position);
};