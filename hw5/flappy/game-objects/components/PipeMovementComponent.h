#pragma once
#include "MovementComponent.h"

/**
 * PipeMovementComponent defines the behavior of pipes for a flappy bird clone.
 * An object with this movement component will repeatedly scroll from right to left,
 * with a random vertical position each time. 
 * 
 * Additionally, the static score defined in Scene is incremented once at a specif x position each loop.
 */
class PipeMovementComponent : public MovementComponent {
    public:
        PipeMovementComponent(float speed, sf::Vector2f spawnpoint, float minX, float scoreX);
        sf::Vector2f getVelocity(unsigned int delta);

    private:
        float speed;
        sf::Vector2f spawnpoint;
        float minX;
        float scoreX;
        bool scored;
};