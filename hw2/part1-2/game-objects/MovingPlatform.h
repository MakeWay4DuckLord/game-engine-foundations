#pragma once
#include "Platform.h"

/**
 * MovingPlatform extends Platform, with an additional field for velocity
 * in this current version, a MovingPlatform just moves back and forth along the axis of that velocity,
 * staying within 100 pixes of the starting position. 
 */
class MovingPlatform: public Platform::Platform
{
    public:
        sf::Vector2f vel;
        sf::Vector2f init_position;
        void updateGameObject( unsigned int delta ) override;
        MovingPlatform(sf::Vector2f size, sf::Vector2f position, sf::Vector2f vel);
};