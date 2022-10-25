#pragma once
#include <SFML/Graphics.hpp>
// #include "components/CollisionComponent.h"
class CollisionComponent;
class MovementComponent;
class RenderComponent;
/* 
 * GameObject extends the SFML RectangleShape class, providing additional fields for collision information
 */
class GameObject
{
	public:
	    GameObject(sf::Shape *shape);
	    GameObject(sf::Shape *shape, MovementComponent *movementComponent, CollisionComponent *collisionComponent, RenderComponent *renderComponent);
		void update(unsigned int delta);
		CollisionComponent *collisionComponent;
		MovementComponent *movementComponent;
		RenderComponent *renderComponent;
		sf::Shape* getShape();
	private:
		sf::Shape *shape;
};