#include "CharMoveComponent.h"
#include "RenderComponent.h"
#include "game-objects/GameObject.h"
#include "events/CharacterHandler.h"

CharMoveComponent::CharMoveComponent(float speed) {
    this->speed = speed;
    this->inputQueue = std::queue<sf::Keyboard::Key>();
}

sf::Vector2f CharMoveComponent::getVelocity(unsigned int delta) {
    float actualSpeed = speed;
    //by default you just fall
    sf::Vector2f velocity(0.f, speed * delta);

    while(!inputQueue.empty()) {

        sf::Keyboard::Key input = inputQueue.front();

        if(input == LEFT) {
            velocity.x -= speed * delta;
        } else if(input == RIGHT) {
            velocity.x += speed * delta;
        } else if(input == JUMP) {
            velocity.y -= 2 * speed * delta;
        }
    
        inputQueue.pop();
    }

    return velocity;
}

void CharMoveComponent::enqueueInput(sf::Keyboard::Key key) {
    inputQueue.push(key);
}