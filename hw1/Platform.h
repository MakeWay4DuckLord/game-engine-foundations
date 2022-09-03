#pragma once
#include <SFML/Graphics.hpp>
/** 
 * Platform extends the SFML RectangleShape class, providing additional fields for collision information
 * @author Declan Smith
 */
class Platform: public sf::RectangleShape
{
	//for now platform is basically just a rectangle shape
	public:
		Platform(sf::Vector2f size, sf::Vector2f position);
		void update();

	//TODO more functionality likely to be added here for collisions
};
