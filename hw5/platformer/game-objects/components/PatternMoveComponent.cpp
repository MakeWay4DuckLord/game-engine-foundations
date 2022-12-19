#include "PatternMoveComponent.h"
#include "game-objects/GameObject.h"
#include <cmath>

PatternMoveComponent::PatternMoveComponent(float speed, std::deque<sf::Vector2f> pattern) {
    this->pattern = pattern;
    this->speed = speed;
}

sf::Vector2f PatternMoveComponent::getVelocity(unsigned int delta) {
    float x = pattern.front().x - object->getShape()->getPosition().x;
    float y = pattern.front().y - object->getShape()->getPosition().y;
    float magnitude = sqrt(x*x + y*y);
    if(magnitude < speed) {
        pattern.push_back(pattern.front());
        pattern.pop_front();
        return sf::Vector2f(x, y);
    }
    return sf::Vector2f(x * delta / magnitude, y * delta / magnitude);
}