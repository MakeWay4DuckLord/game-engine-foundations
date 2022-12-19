#include "PipeMovementComponent.h"
#include "game-objects/GameObject.h"
#include "Scene.h"

PipeMovementComponent::PipeMovementComponent(float speed, sf::Vector2f spawnpoint, float minX, float scoreX) {
    this->speed = speed;
    this->spawnpoint = spawnpoint;
    this->minX = minX;
    this->scoreX = scoreX;
    this->scored = false;
}

sf::Vector2f PipeMovementComponent::getVelocity(unsigned int delta) {
    if(object->getShape()->getPosition().x < minX) {
        object->getShape()->setPosition(sf::Vector2f(spawnpoint.x, spawnpoint.y + std::rand() % 200 ));
        this->scored = false;
        return sf::Vector2f(0,0);
    } else {
        if(!scored && object->getShape()->getPosition().x < scoreX) {
            Scene::incrementScore();
            scored = true;
        }
        return sf::Vector2f(-1 * speed * delta, 0);
    }
}