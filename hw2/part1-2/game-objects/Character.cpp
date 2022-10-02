#include "Character.h"

Character::Character(sf::Vector2f size, sf::Vector2f position, int speed): GameObject(size, position)
{
    this->speed = speed;
};

Character::Character(sf::Vector2f size, sf::Vector2f position): GameObject(size, position)
{
    this->speed = DEFAULT_SPEED;
};

void Character::updateGameObject(unsigned int delta)
{
    //move .1 in appropriate direction if key is pressed
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        move(sf::Vector2f(0.0f, -1.0f * delta * speed));
    } else {
        move(sf::Vector2f(0.0f, 1.0f * delta * speed));
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        move(sf::Vector2f(-1.0f * delta * speed, 0.0f));
    }

    // if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
    //     move(sf::Vector2f(0.0f, 0.1f));
    // }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        move(sf::Vector2f(1.0f * delta * speed, 0.0f));
    }
}