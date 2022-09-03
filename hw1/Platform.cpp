#include "Platform.h"

void Platform::update()
{
    //does nothing, mabybe will check for collisions later?
    //mostly here to be overwritten by moving platform later
    //or perhaps other platforms with things like destructability
}

Platform::Platform(sf::Vector2f size, sf::Vector2f position) : sf::RectangleShape::RectangleShape(size)
{
    setPosition(position);
}