#include "MovingPlatform.h"
#include <cmath>

MovingPlatform::MovingPlatform(sf::Vector2f size, sf::Vector2f position, sf::Vector2f vel) : Platform::Platform(size, position)
{
    MovingPlatform::vel = vel;
    MovingPlatform::init_position = position;
}

void MovingPlatform::update()
{
    //for now, the platform is just hard coded to move back and forth within a hundred pixels of its starting position
    sf::Vector2f current_position = getPosition();
    float distance = sqrt(powf((init_position.x - current_position.x), 2) + powf((init_position.y - current_position.y),2));
    if(distance >= 100) {
        vel.x *= -1;
        vel.y *= -1;
    }
    move(vel);
}