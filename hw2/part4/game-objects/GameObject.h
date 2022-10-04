#pragma once
#include <SFML/Graphics.hpp>
/* 
 * GameObject extends the SFML RectangleShape class, providing additional fields for collision information
 */
class GameObject: public sf::RectangleShape
{
	//for now platform is basically just a rectangle shape
	public:
	    GameObject(sf::Vector2f size, sf::Vector2f position);
		virtual void updateGameObject(unsigned int delta) = 0;
};