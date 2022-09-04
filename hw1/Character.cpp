#include "Character.h"

Character::Character(sf::Vector2f size, sf::Vector2f position): RectangleShape(size)
{
    setPosition(position);
};

void Character::update()
{
    //move .1 in appropriate direction if key is pressed
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        move(sf::Vector2f(0.0f, -0.1f));
    } else {
        move(sf::Vector2f(0.0f, 0.1f));
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        move(sf::Vector2f(-0.1f, 0.0f));
    }

    // if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
    //     move(sf::Vector2f(0.0f, 0.1f));
    // }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        move(sf::Vector2f(0.1f, 0.0f));
    }
}