#include <SFML/Graphics.hpp>
#include <thread>
#include <nlohmann/json.hpp>
#include <nlohmann/adl_serializer.hpp>
#include <zmq.hpp>
#include <string>
#include "game-objects/Platform.h"
#include "game-objects/MovingPlatform.h"
#include "game-objects/Character.h"
#include "timeline/Timeline.h"
#include "GameState.h"
#include <iostream>
#include <unistd.h>

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

	//create the objects
	// Character character(sf::Vector2f(50.f, 50.f), sf::Vector2f(350.f, 300.f));
 
    //create and join socket!
    zmq::context_t context(1);
    const char * protocol = "tcp://127.0.0.1:5555";
    zmq::socket_t socket (context, zmq::socket_type::req);
    socket.connect(protocol);

    if(!socket.connected()) {
      std::cout << "socket cannot be bound??" << std::endl;
      return 1;
    }
    //initialization req

    zmq::message_t initRequest(1);

    socket.send(initRequest, zmq::send_flags::none);

    zmq::message_t reply;
    socket.recv(reply, zmq::recv_flags::none);

    json data = json::parse(reply.to_string());

	// GameState preState(characters, platforms);

	// json data = preState.serialize();


	// GameState *statePtr = new GameState();
	GameState state(data);

	// std::cout << data.dump() << state.count() << std::endl;


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

		// character.updateGameObject(delta);


		//draw characters
        drawObjects(&state.characters, &window);
		drawObjects(&state.platforms, &window);

		//end current frame
		window.display();
	}

	return 0;
}
