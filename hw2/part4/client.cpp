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

int main()
{
	//create the window
	sf::RenderWindow window(sf::VideoMode(800, 600), "window");
 
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
	bool waiting = true;
	int clientID;
	GameState state;
	Character *character;			
	//if server is ready to add a new client it will send back a non zero clientID, 
	while(waiting) {
		json newClient {
			{"clientID", 0},
		};
   		zmq::message_t initRequest(newClient.dump().c_str(), newClient.dump().length());
		socket.send(initRequest, zmq::send_flags::none);

		zmq::message_t reply;
		socket.recv(reply, zmq::recv_flags::none);
		

		json data = json::parse(reply.to_string());
		
		if(data["clientID"] != 0) {
			state = *new GameState(data);
			clientID = data["clientID"];
			character = state.characters[clientID];
			waiting = false;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(5));	
		}
	}


	//add texture to platforms
	sf::Texture cheese;
	cheese.loadFromFile("textures/cheese.jpg");

	for(const auto &p : state.platforms){
		p.second->setTexture(&cheese);
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
			} else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num3) {
				last_time *= 2.0f / mainTimeline->setScalar(2.0f);
			}
		}

		//simulate some more work
		std::this_thread::sleep_for(std::chrono::milliseconds(25));

		//clear with black
		window.clear(sf::Color::Black);

		//update our object
		character->updateGameObject(delta);

		/* collision detection! basically just moves the character upwards while its intersecting with any of the platforms
		 * this allows the character to land on the platforms, but makes interactions hitting the platforms from the side a little weird
		 * each client handles their own collision detection
		 */
		for(const auto &p : state.platforms) {
			while(state.characters[clientID]->getGlobalBounds().intersects(p.second->getGlobalBounds())) {
				state.characters[clientID]->move(sf::Vector2f(0.0f, -0.1f));
			}
		}
		//now character is all updated, give this info to server:
		json charUpdate {
			{"first", true},
			{"second", false},
			{"clientID", clientID},
			{"position.x", character->getPosition().x},
			{"position.y", character->getPosition().y},
		};

		bool polling = true;
		while(polling){
			std::string msgStr = charUpdate.dump();
			zmq::message_t msg(msgStr.c_str(), msgStr.length());
			socket.send(msg, zmq::send_flags::none);

			zmq::message_t confirmation;
			socket.recv(confirmation, zmq::recv_flags::none);
			json reply = json::parse(confirmation.to_string());
			//keep going until the server tells us it was in the first networking section
			if(reply["first"]){
				polling = false;
			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds(5));	
			}
		}

		/* now we will poll the server asking for an update, 
		 * which it will give us once its received updates from all clients.
		 */
		json updateRequestJ {
			{"first", false},
			{"second", true},
			{"clientID", clientID},
		};
		polling = true;
		while(polling) {
			std::string reqStr = updateRequestJ.dump();
			zmq::message_t updateRequest(reqStr.c_str(), reqStr.length());
			socket.send(updateRequest, zmq::send_flags::none);

			zmq::message_t reply;
			socket.recv(reply, zmq::recv_flags::none);
			json serverUpdate = json::parse(reply.to_string());
			if(serverUpdate["second"]) {
				polling = false;
				state.update(serverUpdate);
			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds(5));	
			}

		}

		//draw characters
		state.drawAll(&window);

		//end current frame
		window.display();
	}

	return 0;
}
