#include <SFML/Graphics.hpp>
#include <thread>
#include <string>
#include <nlohmann/json.hpp>
#include "game-objects/components/CollisionComponent.h"
#include "game-objects/components/CharMoveComponent.h"
#include "game-objects/components/PatternMoveComponent.h"
#include "game-objects/components/PipeMovementComponent.h"
#include "game-objects/components/TrackingMoveComponent.h"
#include "game-objects/components/RenderComponent.h"
#include "timeline/Timeline.h"
#include "game-objects/GameObject.h"
#include "game-objects/Character.h"
// #include "events/Event.h"
#include "events/EventManager.h"
#include "Scene.h"
#include <iostream>
#include <unistd.h>
#include <zmq.hpp>

//scripting stuff
#include <v8.h>
#include <libplatform/libplatform.h>
#include <scripting/v8helpers.h>

//MAIN_TIC is the number of miliseconds expected for a frame;
#define MAIN_TIC 10

using json = nlohmann::json;

int score = 0;



int main()
{
	//create the window
	sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(800, 600), "window"); 
	
	//create scene
	Scene *scene = new Scene(window);

	std::string *solidCollisionScript = new std::string("solid_collision");
	std::string *deathScript = new std::string("death");
	std::string *scoreScript = new std::string("score");

	sf::Vector2f pillarSpawn(800, 350);
 
	//create objects which make up 1 pillar
	GameObject *bottom1 = scene->createObject("bottom1", TYPE_PLATFORM, sf::Vector2f(150,500), pillarSpawn, sf::Color::Green, new PipeMovementComponent(.75, pillarSpawn, -150, 200), new CollisionComponent(true, deathScript), new RenderComponent(window));
	GameObject *top1 = scene->createObject("top1", TYPE_PLATFORM, sf::Vector2f(150,500), pillarSpawn, sf::Color::Green, new TrackingMoveComponent(bottom1, sf::Vector2f(0, -750)), new CollisionComponent(true, deathScript), new RenderComponent(window));
	
	//create objects which make up a 2nd pillar
	GameObject *bottom2 = scene->createObject("bottom2", TYPE_PLATFORM, sf::Vector2f(150,500), pillarSpawn + sf::Vector2f(475, -50), sf::Color::Green, new PipeMovementComponent(.75, pillarSpawn, -150, 200), new CollisionComponent(true, deathScript), new RenderComponent(window));
	GameObject *top2 = scene->createObject("top2", TYPE_PLATFORM, sf::Vector2f(150,500), sf::Vector2f(550,400), sf::Color::Green, new TrackingMoveComponent(bottom2, sf::Vector2f(0, -750)), new CollisionComponent(true, deathScript), new RenderComponent(window));

	scene->createObject("spawn_point1", TYPE_SPAWN_POINT, sf::Vector2f(50,50), sf::Vector2f(200,200), sf::Color::White, NULL, NULL, NULL);
	GameObject *character0 = scene->createObject("character", TYPE_CHARACTER, sf::Vector2f(50,50), sf::Vector2f(0,0), sf::Color::Yellow, NULL, NULL, NULL);
	// scene->createObject("view_boundary0", TYPE_VIEW_BOUNDARY, sf::Vector2f(500, 500), sf::Vector2f(100,0), sf::Color::Black, NULL, new CollisionComponent(false), NULL);
	scene->createObject("ground", TYPE_DEATH_ZONE, sf::Vector2f(7000, 100), sf::Vector2f(0, 560), sf::Color::Green, NULL, new CollisionComponent(true, deathScript), new RenderComponent(window));

	//resize mode
	bool constant_mode = false;
	
	Timeline *mainTimeline = new Timeline(NULL, MAIN_TIC);

	unsigned int current_time;
	unsigned int last_time = mainTimeline->getTime();
	unsigned int delta;

	EventManager::getInstance()->setTimeline(new Timeline(mainTimeline, 1));


	sf::Text gameOverMessage;
	sf::Font font;
	font.loadFromFile("fonts/font.ttf");
	gameOverMessage.setFont(font);
	gameOverMessage.setCharacterSize(100);
	gameOverMessage.setString("Game Over!");
	gameOverMessage.setPosition(sf::Vector2f(125, 200));

	//init v8!
	std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.release());
    v8::V8::InitializeICU();
    v8::V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate *isolate = v8::Isolate::New(create_params);

    { // anonymous scope for managing handle scope
        v8::Isolate::Scope isolate_scope(isolate); // must enter the virtual machine to do stuff
        v8::HandleScope handle_scope(isolate);

		// Best practice to isntall all global functions in the context ahead of time.
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        // Bind the global 'print' function to the C++ Print callback.
        global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, v8helpers::Print));
		// Bind the global static factory function for creating new GameObject instances
		// global->Set(isolate, "gameobjectfactory", v8::FunctionTemplate::New(isolate, GameObject::ScriptedGameObjectFactory));
		// Bind the global static function for retrieving object handles
		global->Set(isolate, "gethandle", v8::FunctionTemplate::New(isolate, ScriptManager::getHandleFromScript));
		global->Set(isolate, "lose", v8::FunctionTemplate::New(isolate, Scene::lose));
		global->Set(isolate, "raise", v8::FunctionTemplate::New(isolate, EventManager::raiseEventCallback));

        v8::Local<v8::Context> default_context =  v8::Context::New(isolate, NULL, global);
		v8::Context::Scope default_context_scope(default_context); // enter the context

        ScriptManager *sm = new ScriptManager(isolate, default_context); 

		//create context for collisions
		v8::Local<v8::Context> collision_context = v8::Context::New(isolate, NULL, global);
		sm->addContext(isolate, collision_context, "collision_context");

		character0->exposeToV8(isolate, collision_context);

		sm->addScript("solid_collision", "scripting/scripts/solid_collision.js", "collision_context");
		sm->addScript("death", "scripting/scripts/gameover.js", "collision_context");
		
		bool gameOver = false;

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

			if(gameOver) {
				scene->draw();
				window->draw(gameOverMessage);
			} else {
				gameOver = scene->update(delta);
				scene->draw();
			}
			window->display();

			//end current frame
		}
	} //handle scope
	// }
	isolate->Dispose();
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
	return 0;
}