#include "CharMoveComponent.h"
#include "game-objects/GameObject.h"

CharMoveComponent::CharMoveComponent(float speed) {
    this->speed = speed;
}

sf::Vector2f CharMoveComponent::getVelocity(unsigned int delta) {
    sf::Vector2f velocity(0.f, speed * delta);

    //this jump is a little goofy, could do some timeline shenanigans
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        velocity.y *= -1;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x -= speed * delta;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocity.x += speed * delta;
    }

    return velocity;
}