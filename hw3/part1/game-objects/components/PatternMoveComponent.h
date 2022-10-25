#pragma once
#include "MovementComponent.h"
#include <deque>


class PatternMoveComponent : public MovementComponent {
    public:
        PatternMoveComponent(float speed, std::deque<sf::Vector2f> pattern);
        sf::Vector2f getVelocity(unsigned int delta);
    private:
        //pattern representation
        std::deque<sf::Vector2f> pattern;
        float speed;
};