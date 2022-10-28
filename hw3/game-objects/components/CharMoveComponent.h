#pragma once
#include "MovementComponent.h"

/**
 * CharMoveComponent is a MovementComponent for a player controlled character
 * it updates position according to keyboard inputs.
 * 
 */
class CharMoveComponent : public MovementComponent {
    public:
        CharMoveComponent(float speed);
        sf::Vector2f getVelocity(unsigned int delta);
        // void setFalling(bool isFalling);
    private:
        // bool isFalling;
        // sf::Vector2f velocity;
        float speed;
};