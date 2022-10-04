#include <SFML/Graphics.hpp>
#include <thread>
#include <string>
#include <nlohmann/json.hpp>
#include <nlohmann/adl_serializer.hpp>
#include "game-objects/Platform.h"
#include "game-objects/MovingPlatform.h"
#include "game-objects/Character.h"
#include "timeline/Timeline.h"
#include "GameState.h"
#include <iostream>
#include <unistd.h>
#include <zmq.hpp>

//MAIN_TIC is the number of miliseconds expected for a frame;
#define MAIN_TIC 10

using json = nlohmann::json;

/**
 * method to be used by threads to parallelize updating game objects
 * calls updateGameObject on each GameObject in objects
 * @param objects pointer to a vector of GameObject pointers
 */
void updateObjects(std::vector<GameObject *> *objects, unsigned int delta) {
	for(GameObject* &object : *objects) {
		object->updateGameObject(delta);
	}
}
/**
 * method to be used by threads to parallelize drawing game objects
 * draws each GameObject in objects to the given window.
 * @param objects pointer to a vector of GameObject pointers
 * @param window pointer to an sf::RenderWindow
 */
void drawObjects(std::vector<GameObject *> *objects, sf::RenderWindow *window) {
	for(GameObject* &object : *objects) {
		window->draw(*object);
	}
}

int main()
{
	//create the window
	sf::RenderWindow window(sf::VideoMode(800, 600), "window"); 
 
	//create state
    json data = "[[[50.0,50.0,350.0,300.0]],[[200.0,100.0,0.0,400.0]]]"_json;
	GameState *statePtr = new GameState();
	GameState state = *statePtr;

	//populate state
	state.platforms.push_back(new MovingPlatform(sf::Vector2f(200.f, 100.f), sf::Vector2f(300.f, 400.f), sf::Vector2f(.6f,0.6f)));
	state.platforms.push_back(new Platform(sf::Vector2f(200.f, 100.f), sf::Vector2f(0.f, 400.f)));
	state.characters.push_back(new Character(sf::Vector2f(50.f, 50.f), sf::Vector2f(350.f, 300.f)));
	std::cout << data.dump() << state.count() << std::endl;

	//add textures
	sf::Texture cheese;
	cheese.loadFromFile("textures/cheese.jpg");

	for(GameObject* &p : state.platforms){
		p->setTexture(&cheese);
	} 

	sf::Texture mouse;
	mouse.loadFromFile("textures/mouse.jpeg");

	for(GameObject* &c : state.characters){
		c->setTexture(&mouse);
	} 
	
	//create and bind socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, zmq::socket_type::rep);
	const char * protocol = "tcp://127.0.0.1:5555";
	socket.bind(protocol);
	if(!socket.connected()) {
		std::cout << "socket wasn't bound" << std::endl;
		return 1;
	}
	int clients = 0;

	//resize mode
	bool constant_mode = false;
	
	Timeline *mainTimeline = new Timeline(NULL, MAIN_TIC);

	unsigned int current_time;
	unsigned int last_time = mainTimeline->getTime();
	unsigned int delta;
	//keep looping while window is open
	while(window.isOpen()) {
		current_time = mainTimeline->getTime();
		delta = current_time - last_time;
		last_time = current_time;

		//print for debugging
		// std::cout << delta << std::endl;

		//check each event that happens to the window
		sf::Event event;
		while(window.pollEvent(event)) {
			//if the event is a close request, close the window
			if(event.type == sf::Event::Closed) {
				window.close();
			} else if(event.type == sf::Event::Resized && constant_mode) {
				window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
			} else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
				constant_mode = !constant_mode;
				if(constant_mode) {
					window.setView(sf::View(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y)));
				} else {
					window.setView(window.getDefaultView());
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

		std::this_thread::sleep_for(std::chrono::milliseconds(25));

		//clear with black
		window.clear(sf::Color::Black);

		// //update objects using the fork and join pattern
		std::thread platformThread(updateObjects, &state.platforms, delta);
		std::thread characterThread(updateObjects, &state.characters, delta);

		// //wait for threads to finish updating all objects
		platformThread.join();
		characterThread.join();
		
		//start drawing the platforms while the main thread does collision logic
		std::thread platformDrawer(drawObjects, &state.platforms, &window);
		
		/* collision detection! basically just moves the character upwards while its intersecting with any of the platforms
		 * this allows the character to land on the platforms, but makes interactions hitting the platforms from the side a little weird
		 * collisions could definitely use an update, this currently doesn't just work if more characters or platforms are added.
		 */
	    // while(character.getGlobalBounds().intersects(platform.getGlobalBounds()) || character.getGlobalBounds().intersects(moving_platform.getGlobalBounds())) {
		// 	character.move(sf::Vector2f(0.0f, -0.1f));
		// }
		
		// character.updateGameObject(delta);

		/* collision detection! basically just moves the character upwards while its intersecting with any of the platforms
		 * this allows the character to land on the platforms, but makes interactions hitting the platforms from the side a little weird
		 * collisions could definitely use an update, this currently doesn't just work if more characters or platforms are added.
		 */
	    while(state.characters[0]->getGlobalBounds().intersects(state.platforms[0]->getGlobalBounds()) || state.characters[0]->getGlobalBounds().intersects(state.platforms[1]->getGlobalBounds())) {
			state.characters[0]->move(sf::Vector2f(0.0f, -0.1f));
		}

		//networking!!
		for(int i = 0; i <= clients; i++) {
			zmq::message_t request;
			if(i == clients) {
				if(socket.recv(request, zmq::recv_flags::dontwait)) {
					std::string data = state.serialize().dump();
					zmq::message_t initRep((void *) data.c_str(), data.length());
					socket.send(initRep, zmq::send_flags::none);
				}
				
			}
		}


		//wait for thread to finish drawing platforms
		platformDrawer.join();
		
		//draw characters
		drawObjects(&state.characters, &window);

		//end current frame
		window.display();
	}

	return 0;
}
