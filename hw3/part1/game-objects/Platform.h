#pragma once
#include <SFML/Graphics.hpp>
#include "GameObject.h"
/* 
 * Platform extends the SFML RectangleShape class, providing additional fields for collision information
 */
class Platform: public GameObject
{
	//for now platform is basically just a rectangle shape
	public:
		Platform(sf::Vector2f size, sf::Vector2f position);
		void updateGameObject(unsigned int delta) override;

	//TODO more functionality likely to be added here for collisions or something
};
