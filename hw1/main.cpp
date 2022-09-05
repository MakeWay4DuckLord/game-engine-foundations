#include <SFML/Graphics.hpp>
#include "Platform.h"
#include "MovingPlatform.h"
#include "Character.h"

int main()
{
	//create the window
	sf::RenderWindow window(sf::VideoMode(800, 600), "window");

	//create the objects
	Platform platform(sf::Vector2f(200.f, 100.f), sf::Vector2f(0.f, 400.f));
	MovingPlatform moving_platform(sf::Vector2f(200.f, 100.f), sf::Vector2f(300.f, 400.f), sf::Vector2f(0.1f,0.0f));
	Character character(sf::Vector2f(50.f, 50.f), sf::Vector2f(350.f, 300.f));

	//texture!
	sf::Texture cheese;
	cheese.loadFromFile("cheese.jpg");
	platform.setTexture(&cheese);
	moving_platform.setTexture(&cheese);

	sf::Texture mouse;
	mouse.loadFromFile("mouse.jpeg");
	character.setTexture(&mouse);

	//resize mode
	bool constant_mode = false;

	//keep looping while window is open
	while(window.isOpen()) {
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
			}
		}

		//clear with black
		window.clear(sf::Color::Black);

		//update objects
		platform.update();
		moving_platform.update();
		character.update();
		
		//collision detection! basically just moves the character upwards if its intersecting with any of the platforms
		//this allows the character to land on the platforms, but makes interactions hitting the platforms from the side a little weird
	    if(character.getGlobalBounds().intersects(platform.getGlobalBounds()) || character.getGlobalBounds().intersects(moving_platform.getGlobalBounds())) {
			character.move(sf::Vector2f(0.0f, -0.5f));
		}
		
		//draw the frame
		window.draw(platform);
		window.draw(moving_platform);
		window.draw(character);


		//end current frame
		window.display();
	}

	return 0;
}
