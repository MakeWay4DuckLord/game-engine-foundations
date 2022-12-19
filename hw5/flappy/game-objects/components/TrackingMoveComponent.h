#pragma once
#include "MovementComponent.h"

class TrackingMoveComponent : public MovementComponent {
    public:
        TrackingMoveComponent(GameObject *target, sf::Vector2f offset);
        sf::Vector2f getVelocity(unsigned int delta);

    private:
        GameObject *target;
        sf::Vector2f offset;
};