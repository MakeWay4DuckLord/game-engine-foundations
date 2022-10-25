#include <SFML/Graphics.hpp>
#include <thread>
#include <string>
#include <nlohmann/json.hpp>
#include <nlohmann/adl_serializer.hpp>
#include "game-objects/components/CollisionComponent.h"
#include "game-objects/components/CharMoveComponent.h"
#include "game-objects/components/PatternMoveComponent.h"
#include "game-objects/components/RenderComponent.h"
#include "timeline/Timeline.h"
#include "game-objects/GameObject.h"
#include "game-objects/Character.h"
#include <iostream>
#include <unistd.h>
#include <zmq.hpp>

//MAIN_TIC is the number of miliseconds expected for a frame;
#define MAIN_TIC 10

using json = nlohmann::json;

int main()
{
	//create the window
	sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(800, 600), "window"); 
 
	//create state

	//populate state
	// state.addPlatform(0, new MovingPlatform(sf::Vector2f(200.f, 100.f), sf::Vector2f(300.f, 400.f), sf::Vector2f(.6f,0.6f)));
	// state.addPlatform(1, new Platform(sf::Vector2f(200.f, 100.f), sf::Vector2f(0.f, 400.f)));
	// state.addCharacter(0, new Character(sf::Vector2f(50.f, 50.f), sf::Vector2f(350.f, 300.f)));

	sf::RectangleShape *rectangle = new sf::RectangleShape(sf::Vector2f(200.f, 100.f));
	rectangle->setFillColor(sf::Color::Green);
	rectangle->setPosition(sf::Vector2f(350.f, 400.f));

	GameObject *platform = new GameObject(rectangle, new PatternMoveComponent(1, {sf::Vector2f(0,0), sf::Vector2f(0, 100), sf::Vector2f(100, 100)}), new CollisionComponent(true), new RenderComponent(window));

	sf::RectangleShape *square = new sf::RectangleShape(sf::Vector2f(50.f, 50.f));
	square->setFillColor(sf::Color::Red);
	square->setPosition(sf::Vector2f(350.f, 300.f));

	sf::RectangleShape *point = new sf::RectangleShape(sf::Vector2f(0, 0));

	point->setPosition(sf::Vector2f(100, 10));

	GameObject *spawnPoint = new GameObject(point);

	Character *character = new Character(square, 1.f, window, spawnPoint);

	// sf::RectangleShape *barrier = new sf::RectangleShape(sf::Vector2f(800, 400));
	// GameObject *viewBoundary = new GameObject(barrier);

	// viewBoundary->collisionComponent = new CollisionComponent(false);

	sf::RectangleShape *theVoid = new sf::RectangleShape(sf::Vector2f(800, 1));
	theVoid->setPosition(0, 600);
	GameObject *deathZone = new GameObject(theVoid, NULL, new CollisionComponent(false), NULL);
	// deathZone->collisionComponent = new CollisionComponent(true);


	//create and bind socket


	//resize mode
	bool constant_mode = false;
	
	Timeline *mainTimeline = new Timeline(NULL, MAIN_TIC);

	unsigned int current_time;
	unsigned int last_time = mainTimeline->getTime();
	unsigned int delta;
	//keep looping while window is open
	while(window->isOpen()) {
		current_time = mainTimeline->getTime();
		delta = current_time - last_time;
		last_time = current_time;

		//print for debugging
		// std::cout << delta << std::endl;

		//check each event that happens to the window
		sf::Event event;
		while(window->pollEvent(event)) {
			//if the event is a close request, close the window
			if(event.type == sf::Event::Closed) {
				window->close();
			} else if(event.type == sf::Event::Resized && constant_mode) {
				window->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
			} else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
				constant_mode = !constant_mode;
				if(constant_mode) {
					window->setView(sf::View(sf::FloatRect(0, 0, window->getSize().x, window->getSize().y)));
				} else {
					window->setView(window->getDefaultView());
				}
			} else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				mainTimeline->pause();
				last_time = mainTimeline->getTime();
			} else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num2) {
				last_time *= 0.5f / mainTimeline->setScalar(0.5f);
			} else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num1) {
				last_time *= 1.0f / mainTimeline->setScalar(1.0f);
				// mainTimeline->setScalar(1.0f);
			} else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num3) {
				last_time *= 2.0f / mainTimeline->setScalar(2.0f);
			}
		}

		// std::this_thread::sleep_for(std::chrono::milliseconds(25));

		//clear with black
		window->clear(sf::Color::Black);

		//update objects using the fork and join pattern
		
		//first clean up the log

		//update object server is responsible for	
		
		// state.updatePlatforms(delta);
		
		//start drawing the platforms while the main thread does collision logic
		
		//while platform are drawing do collision detection to update local character position, then do networking

		/* collision detection! basically just moves the character upwards while its intersecting with any of the platforms
		 * this allows the character to land on the platforms, but makes interactions hitting the platforms from the side a little weird
		 * each client manages their own collision detection
		 */
	    // while(state.characters[0]->getGlobalBounds().intersects(state.platforms[0]->getGlobalBounds()) || state.characters[0]->getGlobalBounds().intersects(state.platforms[1]->getGlobalBounds())) {
		// 	state.characters[0]->move(sf::Vector2f(0.0f, -0.1f));
		// }
		// for(int i = 0; i < state.platforms.size(); i++) {
		// 	state.characters[0]->collisionComponent->collision(state.platforms[i]->collisionComponent);
		// }

		// state.platforms[0]->collisionComponent->getBounds();
		// state.platforms[1]->collisionComponent->getBounds();


		// characterDrawer.join();
		// state.drawCharacters(&window);
		// state.drawAll(&window);
		
		platform->update(delta);
		character->update(delta);

		character->collisionComponent->collision(platform->collisionComponent);

		// if(!character->collisionComponent->collision(viewBoundary->collisionComponent)) {
		// 	float x = viewBoundary->getShape()->getGlobalBounds().width / 2;
		// 	float y = viewBoundary->getShape()->getGlobalBounds().height / 2;
		// 	viewBoundary->getShape()->move(sf::Vector2f(x, y));
		// 	window->getView().move(x, y);
		// }

		if(character->collisionComponent->collision(deathZone->collisionComponent)) {
			character->respawn();
		}

		// window.draw(*platform->getShape());
		// window.draw(*character->getShape());
		platform->renderComponent->draw();
		character->renderComponent->draw();

		//end current frame
		window->display();
	}

	return 0;
}
