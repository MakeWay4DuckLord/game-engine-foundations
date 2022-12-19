#pragma once
#include "ObjectComponent.h" 
#include "SFML/System/Vector2.hpp"
class MovementComponent : public ObjectComponent
{
    public:
        MovementComponent();
        void updateMovement(unsigned int delta);
        virtual sf::Vector2f getVelocity(unsigned int delta) = 0;

};