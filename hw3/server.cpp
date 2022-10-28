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

void updateCharacterWrapper(Scene *scene, unsigned int delta) {
	scene->updateCharacter(delta);
}

void updateObjectsWrapper(Scene *scene, unsigned int delta) {
	scene->updateObjects(delta);
}

void collisionWrapper(Scene *scene) {
	scene->calculateCollisions();
}

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
	scene->createObject("character0", TYPE_CHARACTER, sf::Vector2f(50,50), sf::Vector2f(0,0), sf::Color::Red, NULL, NULL, NULL);
	scene->createObject("view boundary 1", TYPE_VIEW_BOUNDARY, sf::Vector2f(500, 500), sf::Vector2f(100,0), sf::Color::Black, NULL, new CollisionComponent(false), NULL);
	scene->createObject("death floor", TYPE_DEATH_ZONE, sf::Vector2f(3000, 100), sf::Vector2f(0, 900), sf::Color::White, NULL, new CollisionComponent(false), NULL);
	// scene->createObject("death ceiling", TYPE_DEATH_ZONE, sf::Vector2f(3000, 100), sf::Vector2f(0, -100), sf::Color::White, NULL, new CollisionComponent(false), new RenderComponent(window));
	// scene->createObject("death wall right", TYPE_DEATH_ZONE, sf::Vector2f(100, 3000), sf::Vector2f(0, 900), sf::Color::White, NULL, new CollisionComponent(false), new RenderComponent(window));
	// scene->createObject("death wall left", TYPE_DEATH_ZONE, sf::Vector2f(100, 3000), sf::Vector2f(0, -100), sf::Color::White, NULL, new CollisionComponent(false), new RenderComponent(window));

	//create and bind sockets:
	zmq::context_t context(1);
	zmq::socket_t reqSocket (context, zmq::socket_type::req);
	zmq::socket_t pubSocket (context, zmq::socket_type::pub);
	// const char * protocol = "ipc:///tmp/test";
	const char * reqProtocol = "tcp://127.0.0.1:5555";
	const char * pubProtocol = "tcp://127.0.0.1:5556";

	reqSocket.bind(reqProtocol);
	pubSocket.bind(pubProtocol);

	long int iteration = 0;

	//resize mode
	bool constant_mode = false;
	
	Timeline *mainTimeline = new Timeline(NULL, MAIN_TIC);

	unsigned int current_time;
	unsigned int last_time = mainTimeline->getTime();
	unsigned int delta;
	int clients = 0;
	int nextId = 1;

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

		//fork and join to update objects in parallel
		std::thread characterThread(updateCharacterWrapper, scene, delta);
		std::thread objectsThread(updateObjectsWrapper, scene, delta);

		characterThread.join();
		objectsThread.join();
		// scene->calculateCollisions();

		//fork to calculate collisions while doing networking in main thread
		std::thread collisionThread(collisionWrapper, scene);

		//networking:

		zmq::send_flags sendFlag = zmq::send_flags::dontwait;
		zmq::recv_flags recvFlag = zmq::recv_flags::dontwait;
		int i = 0;
		json scene_j = scene->getObjectUpdates();
		scene_j["iteration"] = iteration++;
		do {

			
			scene_j["nextId"] = nextId;
			std::string objectsDump = scene_j.dump();
			//send objects update 
			zmq::message_t update(objectsDump.c_str(), objectsDump.size());
			// std::cout << update.to_string() << std::endl;
			zmq::send_result_t status = reqSocket.send(update, sendFlag);
			// std::cout << objectsDump << std::endl;

			
			//once someone replies to the socket, we're in business
			if(status.has_value() && status.value() > 0) {
				// sendFlag = zmq::send_flags::none;
				// recvFlag = zmq::recv_flags::none;
				//receive character update
					//make a message_t
					zmq::message_t characterUpdateMsg;
					//recv
					reqSocket.recv(characterUpdateMsg, zmq::recv_flags::none);
					//publish
					
					std::cout << characterUpdateMsg.to_string() << std::endl;

					json characterUpdate_j = json::parse(characterUpdateMsg.to_string());
					pubSocket.send(characterUpdateMsg, zmq::send_flags::none);
					// check for new
					if(characterUpdate_j["iteration"] == 0) {
						//new character
						clients++;
						nextId++;
						//TODO remake msg cause
						scene->createObject(characterUpdate_j["id"], TYPE_CHARACTER, sf::Vector2f(50,50), sf::Vector2f(0,0), sf::Color::Red, NULL, NULL, NULL);
					//check for close
					} else if(characterUpdate_j["iteration"] == -1) {
						//delete character
						clients--;
						scene->removeCharacter(characterUpdate_j["id"]);
					} else {
						//update character
						scene->updateCharacter(characterUpdate_j);
					}
			}

		} while (++i < clients);

		collisionThread.join();

		//publish footer message (aka server character update)
		if(clients > 0) {
			json serverUpdate_j = scene->getCharacterUpdate();
			serverUpdate_j["iteration"] = iteration;
			zmq::message_t serverUpdateMsg(serverUpdate_j.dump().c_str(), serverUpdate_j.dump().length());
			pubSocket.send(serverUpdateMsg, zmq::send_flags::none);
		}


		//clear with black
		window->clear(sf::Color::Black);


		scene->draw();

		//end current frame
		window->display();
	}

	return 0;
}

