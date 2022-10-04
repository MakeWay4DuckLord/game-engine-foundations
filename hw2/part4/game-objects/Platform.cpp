#include "Platform.h"

void Platform::updateGameObject(unsigned int delta)
{
    //does nothing, mabybe will check for collisions later?
    //mostly here to be overwritten by moving platform later
    //or perhaps other platforms with things like destructability
}

Platform::Platform(sf::Vector2f size, sf::Vector2f position) : GameObject(size, position)
{
    // setPosition(position);
}