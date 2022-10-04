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
 * calls updateGameObject on each platform in the given GameState
 * @param objects pointer to a vector of GameObject pointers
 */
void updatePlatforms(GameState *state, unsigned int delta) {
	state->updatePlatforms(delta);
}

/**
 * method to be used by threads to parallelize drawing game objects
 * draws each platform in the GameState to the given window.
 * @param GameState pointer to the game state object
 * @param window pointer to an sf::RenderWindow
 */
void drawPlatforms(GameState *state, sf::RenderWindow *window) {
	state->drawPlatforms(window);
}

/**
 * method to be used by threads to parallelize drawing game objects
 * draws each character in GameState to the given window.
 * @param GameState pointer to the game state object
 * @param window pointer to an sf::RenderWindow
 */
void drawCharacters(GameState *state, sf::RenderWindow *window) {
	state->drawCharacters(window);
}

int main()
{
	//create the window
	sf::RenderWindow window(sf::VideoMode(800, 600), "window"); 
 
	//create state
	GameState state = *new GameState();

	//populate state
	state.addPlatform(0, new MovingPlatform(sf::Vector2f(200.f, 100.f), sf::Vector2f(300.f, 400.f), sf::Vector2f(.6f,0.6f)));
	state.addPlatform(1, new Platform(sf::Vector2f(200.f, 100.f), sf::Vector2f(0.f, 400.f)));
	state.addCharacter(0, new Character(sf::Vector2f(50.f, 50.f), sf::Vector2f(350.f, 300.f)));

	//add textures
	sf::Texture cheese;
	cheese.loadFromFile("textures/cheese.jpg");

	for(const auto &p : state.platforms){
		p.second->setTexture(&cheese);
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

		//update objects using the fork and join pattern
		
		//first clean up the log
		state.clearUpdateLog();

		//update object server is responsible for	
		state.updateCharacter(0, delta);
		state.updatePlatforms(delta);
		
		//start drawing the platforms while the main thread does collision logic
		std::thread platformDrawer(drawPlatforms, &state, &window);
		
		//while platform are drawing do collision detection to update local character position, then do networking

		/* collision detection! basically just moves the character upwards while its intersecting with any of the platforms
		 * this allows the character to land on the platforms, but makes interactions hitting the platforms from the side a little weird
		 * each client manages their own collision detection
		 */
	    while(state.characters[0]->getGlobalBounds().intersects(state.platforms[0]->getGlobalBounds()) || state.characters[0]->getGlobalBounds().intersects(state.platforms[1]->getGlobalBounds())) {
			state.characters[0]->move(sf::Vector2f(0.0f, -0.1f));
		}

		//network
		if(clients == 0) {
			zmq::message_t request;
			if(socket.recv(request, zmq::recv_flags::dontwait)) {
					clients++;
					state.addCharacter(clients, new Character(sf::Vector2f(50.f, 50.f), sf::Vector2f(350.f, 300.f)));
					json data_j = state.serialize();
					data_j["clientID"] = clients;
					std::string data = data_j.dump();
					zmq::message_t initRep((void *) data.c_str(), data.length());
					socket.send(initRep, zmq::send_flags::none);
				}
		} else {	
			//handle first request unt
			for(int i = 1; i <= clients; i++) {
				zmq::message_t request;
				socket.recv(request, zmq::recv_flags::none);
				json req_j = json::parse(request.to_string());

				if(req_j["clientID"] == 0) { //new client
					//client is incremented so this wont count against stuff
					state.addCharacter(++clients, new Character(sf::Vector2f(50.f, 50.f), sf::Vector2f(350.f, 300.f)));
					json data_j = state.serialize();
					data_j["clientID"] = clients;
					std::string data = data_j.dump();
					zmq::message_t initRep(data.c_str(), data.length());
					socket.send(initRep, zmq::send_flags::none);
				} else { // returning client
					if(req_j["first"]) {
						state.updateCharacter(req_j);
					} else {
						//decrement i to not count a clients second request
						i--;
					}

					//tell client which part of the networking section the server is in
					json reply_j = {
						{"first", true},
						{"second", false},
					}; 
					zmq::message_t msg(reply_j.dump().c_str(), reply_j.dump().length());
					socket.send(msg, zmq::send_flags::none);
				}
			}
		}

		platformDrawer.join();
		std::thread characterDrawer(drawCharacters, &state, &window);

		if(clients != 0) {
			//first section done, every thing is updated, now do second requests
			for(int i = 1; i <= clients; i++) {
				zmq::message_t request;
				socket.recv(request, zmq::recv_flags::none);
				json req_j = json::parse(request.to_string());

				//if its not a new string
				if(req_j["clientID"] != 0) {
					//client knows to disregard the update if its in the wrong section
					json updateJson = state.getUpdateLog();
					updateJson["first"] = false;
					updateJson["second"] = true;
					std::string updateStr = updateJson.dump();
					zmq::message_t serverUpdate(updateStr.c_str(), updateStr.length());
					socket.send(serverUpdate, zmq::send_flags::none);

				} else { //if client is new, tell it to hold its horse for a bit
					//client knows to keep checking in until it is given
					socket.send(request, zmq::send_flags::none);

					//decrement i so we dont skip any clients
					i--;
				}

			}
		}

		characterDrawer.join();

		//end current frame
		window.display();
	}

	return 0;
}
