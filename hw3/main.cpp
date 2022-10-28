#include <SFML/Graphics.hpp>
#include <thread>
#include <string>
#include <nlohmann/json.hpp>
#include "game-objects/components/CollisionComponent.h"
#include "game-objects/components/CharMoveComponent.h"
#include "game-objects/components/PatternMoveComponent.h"
#include "game-objects/components/RenderComponent.h"
#include "timeline/Timeline.h"
#include "game-objects/GameObject.h"
#include "game-objects/Character.h"
#include "Scene.h"
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
	
	//create scene
	Scene *scene = new Scene(window);
 
	scene->createObject("moving platform 1", TYPE_PLATFORM, sf::Vector2f(200.f, 100.f), sf::Vector2f(750, 400), sf::Color::Yellow, new PatternMoveComponent(0.25, {sf::Vector2f(750,500), sf::Vector2f(750, 150)}), new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("moving platform 2", TYPE_PLATFORM, sf::Vector2f(200.f, 100.f), sf::Vector2f(1150, 150), sf::Color::Yellow, new PatternMoveComponent(.75, {sf::Vector2f(1350,100), sf::Vector2f(1550, 100), sf::Vector2f(1550, 0), sf::Vector2f(350, 0), sf::Vector2f(350, 150), sf::Vector2f(350, -100), sf::Vector2f(1550, -100), sf::Vector2f(1550, 100)}), new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("platform 1", TYPE_PLATFORM, sf::Vector2f(200,100), sf::Vector2f(550,400), sf::Color::Blue, NULL, new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("platform 2", TYPE_PLATFORM, sf::Vector2f(200,100), sf::Vector2f(950,500), sf::Color::Green, NULL, new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("platform 3", TYPE_PLATFORM, sf::Vector2f(200,100), sf::Vector2f(1150,100), sf::Color::Magenta, NULL, new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("platform 4", TYPE_PLATFORM, sf::Vector2f(200,100), sf::Vector2f(550,150), sf::Color::Cyan, NULL, new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("spawn point 1", TYPE_SPAWN_POINT, sf::Vector2f(0,0), sf::Vector2f(675,345), sf::Color::Black, NULL, NULL, NULL);
	scene->createObject("character 0", TYPE_CHARACTER, sf::Vector2f(50,50), sf::Vector2f(0,0), sf::Color::Red, NULL, NULL, NULL);
	scene->createObject("view boundary 0", TYPE_VIEW_BOUNDARY, sf::Vector2f(500, 500), sf::Vector2f(100,0), sf::Color::Black, NULL, new CollisionComponent(false), NULL);
	scene->createObject("death floor", TYPE_DEATH_ZONE, sf::Vector2f(3000, 100), sf::Vector2f(0, 900), sf::Color::White, NULL, new CollisionComponent(false), NULL);
	// scene->createObject("death ceiling", TYPE_DEATH_ZONE, sf::Vector2f(3000, 100), sf::Vector2f(0, -100), sf::Color::White, NULL, new CollisionComponent(false), new RenderComponent(window));
	// scene->createObject("death wall right", TYPE_DEATH_ZONE, sf::Vector2f(100, 3000), sf::Vector2f(0, 900), sf::Color::White, NULL, new CollisionComponent(false), new RenderComponent(window));
	// scene->createObject("death wall left", TYPE_DEATH_ZONE, sf::Vector2f(100, 3000), sf::Vector2f(0, -100), sf::Color::White, NULL, new CollisionComponent(false), new RenderComponent(window));

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

		//check each event that happens to the window
		sf::Event event;
		while(window->pollEvent(event)) {
			//if the event is a close request, close the window
			if(event.type == sf::Event::Closed) {
				window->close();
			} else if(event.type == sf::Event::Resized && constant_mode) {
				// scene->resizeView(event.size.width, event.size.width);
				// window->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
				window->setView(sf::View(sf::FloatRect(0, 0, window->getSize().x, window->getSize().y)));
			} else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
				// scene->toggleView();
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
			} else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num3) {
				last_time *= 2.0f / mainTimeline->setScalar(2.0f);
			}
		}

		//clear with black
		window->clear(sf::Color::Black);

		scene->update(delta);

		scene->draw();

		//end current frame
		window->display();
	}

	return 0;
}
