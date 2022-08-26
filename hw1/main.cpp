#include <SFML/Graphics.hpp>
int main()
{
	//create the window
	sf::RenderWindow window(sf::VideoMode(800, 600), "window");
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

		//draw the frame
		sf::RectangleShape platform(sf::Vector2f(100.f, 100.f));
	
		//set the position of the platform to the middle of the window 
		platform.setPosition((window.getSize().x - platform.getSize().x) / 2, (window.getSize().y - platform.getSize().y) / 2);
		platform.move(10.f,10.f);
		window.draw(platform);

		//end current frame
		window.display();
	}

	return 0;
}
