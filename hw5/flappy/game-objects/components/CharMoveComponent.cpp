#include "CharMoveComponent.h"
#include "RenderComponent.h"
#include "game-objects/GameObject.h"
#include "events/CharacterHandler.h"

CharMoveComponent::CharMoveComponent(float speed) {
    this->speed = speed;
    this->inputQueue = std::queue<sf::Keyboard::Key>();
    this->y_velocity = -1 * speed;
    this->flapDelta = 0;
}

sf::Vector2f CharMoveComponent::getVelocity(unsigned int delta) {
    flapDelta += delta;
    //do gravity
    if(y_velocity < 5) {
        y_velocity += .2 * delta;
    }

    if(flapDelta >= 20) {
        if(!inputQueue.empty() && inputQueue.front() == JUMP) {
        flapDelta = 0;
            // if(y_velocity * -1 < 3 * TERMINAL_VELOCITY) {
                y_velocity -= speed;
            // }
        }
    }

    while(!inputQueue.empty()) {
        // sf::Keyboard::Key input = inputQueue.front();        
        inputQueue.pop();
    }

    return sf::Vector2f(0, y_velocity * delta);
}

void CharMoveComponent::enqueueInput(sf::Keyboard::Key key) {
    inputQueue.push(key);
}