#pragma once
#include "MovementComponent.h"
#include "events/EventHandler.h"
#include <SFML/Graphics.hpp>
#include <queue>

// static sf::Keyboard::Key LEFT = sf::Keyboard::A;
// static sf::Keyboard::Key RIGHT = sf::Keyboard::D;
// static sf::Keyboard::Key JUMP = sf::Keyboard::W;
/**
 * CharMoveComponent is a MovementComponent for a player controlled character
 * it updates position according to keyboard inputs.
 * 
 */
class CharMoveComponent : public MovementComponent  {
    public:
        CharMoveComponent(float speed);
        sf::Vector2f getVelocity(unsigned int delta);
        void enqueueInput(sf::Keyboard::Key key);

    private:
        std::queue<sf::Keyboard::Key> inputQueue;
        float speed;
};