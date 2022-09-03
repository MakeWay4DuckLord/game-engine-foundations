#include <SFML/Graphics.hpp>
#include "Platform.h"
#include "MovingPlatform.h"

int main()
{
	//create the window
	sf::RenderWindow window(sf::VideoMode(800, 600), "window");

	//create the objects
	Platform platform(sf::Vector2f(200.f, 100.f), sf::Vector2f(0.f, 400.f));
	MovingPlatform moving_platform(sf::Vector2f(200.f, 100.f), sf::Vector2f(300.f, 400.f), sf::Vector2f(0.1f,0.0f));


	//keep looping while window is open
	while(window.isOpen()) {
		//check each event that happens to the window
		sf::Event event;
		while(window.pollEvent(event)) {
			//if the event is a close request, close the window
			if(event.type == sf::Event::Closed) {
				window.close();
			}
		}

		//clear with black
		window.clear(sf::Color::Black);

		//update objects
		platform.update();
		moving_platform.update();
		
		//draw the frame
		window.draw(platform);
		window.draw(moving_platform);

		//end current frame
		window.display();
	}

	return 0;
}
