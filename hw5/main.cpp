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

int main()
{
	//create the window
	sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(800, 600), "window"); 
	
	//create scene
	Scene *scene = new Scene(window);
 
	scene->createObject("moving_platform1", TYPE_PLATFORM, sf::Vector2f(200.f, 100.f), sf::Vector2f(750, 400), sf::Color::Yellow, new PatternMoveComponent(0.25, {sf::Vector2f(750,500), sf::Vector2f(750, 150)}), new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("moving_platform2", TYPE_PLATFORM, sf::Vector2f(200.f, 100.f), sf::Vector2f(1150, 150), sf::Color::Yellow, new PatternMoveComponent(.75, {sf::Vector2f(1350,100), sf::Vector2f(1550, 100), sf::Vector2f(1550, 0), sf::Vector2f(350, 0), sf::Vector2f(350, 150), sf::Vector2f(350, -100), sf::Vector2f(1550, -100), sf::Vector2f(1550, 100)}), new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("platform_1", TYPE_PLATFORM, sf::Vector2f(200,100), sf::Vector2f(550,400), sf::Color::Blue, NULL, new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("platform_2", TYPE_PLATFORM, sf::Vector2f(200,100), sf::Vector2f(950,500), sf::Color::Green, NULL, new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("platform_3", TYPE_PLATFORM, sf::Vector2f(200,100), sf::Vector2f(1150,100), sf::Color::Magenta, NULL, new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("platform_4", TYPE_PLATFORM, sf::Vector2f(200,100), sf::Vector2f(550,150), sf::Color::Cyan, NULL, new CollisionComponent(true), new RenderComponent(window));
	scene->createObject("spawn_point1", TYPE_SPAWN_POINT, sf::Vector2f(0,0), sf::Vector2f(675,345), sf::Color::Black, NULL, NULL, NULL);
	GameObject *character0 = scene->createObject("character", TYPE_CHARACTER, sf::Vector2f(50,50), sf::Vector2f(0,0), sf::Color::Red, NULL, NULL, NULL);
	scene->createObject("view_boundary0", TYPE_VIEW_BOUNDARY, sf::Vector2f(500, 500), sf::Vector2f(100,0), sf::Color::Black, NULL, new CollisionComponent(false), NULL);
	scene->createObject("death_floor", TYPE_DEATH_ZONE, sf::Vector2f(3000, 100), sf::Vector2f(0, 900), sf::Color::White, NULL, new CollisionComponent(false), NULL);
	// scene->createObject("death ceiling", TYPE_DEATH_ZONE, sf::Vector2f(3000, 100), sf::Vector2f(0, -100), sf::Color::White, NULL, new CollisionComponent(false), new RenderComponent(window));
	// scene->createObject("death wall right", TYPE_DEATH_ZONE, sf::Vector2f(100, 3000), sf::Vector2f(0, 900), sf::Color::White, NULL, new CollisionComponent(false), new RenderComponent(window));
	// scene->createObject("death wall left", TYPE_DEATH_ZONE, sf::Vector2f(100, 3000), sf::Vector2f(0, -100), sf::Color::White, NULL, new CollisionComponent(false), new RenderComponent(window));

	//resize mode
	bool constant_mode = false;
	
	Timeline *mainTimeline = new Timeline(NULL, MAIN_TIC);

	unsigned int current_time;
	unsigned int last_time = mainTimeline->getTime();
	unsigned int delta;

	EventManager::getInstance()->setTimeline(new Timeline(mainTimeline, 1));

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

		global->Set(isolate, "raise", v8::FunctionTemplate::New(isolate, EventManager::raiseEventCallback));

        v8::Local<v8::Context> default_context =  v8::Context::New(isolate, NULL, global);
		v8::Context::Scope default_context_scope(default_context); // enter the context

        ScriptManager *sm = new ScriptManager(isolate, default_context); 

		// Without parameters, these calls are made to the default context
        sm->addScript("hello_world", "scripting/scripts/hello_world.js");
		sm->addScript("raise_event", "scripting/scripts/raise_event.js");

		
		// Create a new context
		v8::Local<v8::Context> object_context = v8::Context::New(isolate, NULL, global);
		sm->addContext(isolate, object_context, "object_context");

		character0->exposeToV8(isolate, object_context);
		// character0->exposeToV8(isolate, object_context);

		sm->addScript("test_accessors", "scripting/scripts/test_accessors.js", "object_context");


		//create context for collisions
		v8::Local<v8::Context> collision_context = v8::Context::New(isolate, NULL, global);
		sm->addContext(isolate, collision_context, "collision_context");

		character0->exposeToV8(isolate, collision_context);

		sm->addScript("solid_collision", "scripting/scripts/solid_collision.js", "collision_context");

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
					sm->runOne("test_accessors", false, "object_context");
					// sm->runOne("raise_event", true);
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
	} //handle scope
	// }
	isolate->Dispose();
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
	return 0;
}

// void usage()
// {
// 	// Usage
// 	std::cout << "\n"
// 		<< "JavaScript <--> C++ Integration Example\n"
// 		<< "---------------------------------------\n"
// 		<< "\n"
// 		<< "Commands:\n"
// 		<< "\th: run hello_world.js\n"
// 		<< "\tp: run perform_function.js\n"
// 		<< "\tc: run create_object.js\n"
// 		<< "\tm: run modify_position.js\n"
// 		<< "\tr: reload scripts from files\n"
// 		<< "\tl: list all game objects\n"
// 		<< "\ta: create new object and assign random position\n"
// 		<< "\to: run random_object.js\n"
// 		<< "\t?: to print this message\n"
// 		<< "\tq: quit"
// 		<< std::endl;

// }

// int main(int argc, char** argv)
// {

//     /*
//      * NOTE: You must initialize v8 in main, otherwise handles will go out of scope.
//      *
//      * Contexts are generally on a per-thread basis, so if you would like to do 
//      * script execution in a different thread, you must create a new context in
//      * the other thread and give care to ensure the context doesn't go out of scope
//      * before the thread's run function is called.
// 	 *
// 	 * The below v8 function calls are basically boilerplate. 
//      */
//     std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
//     v8::V8::InitializePlatform(platform.release());
//     v8::V8::InitializeICU();
//     v8::V8::Initialize();
//     v8::Isolate::CreateParams create_params;
//     create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
//     v8::Isolate *isolate = v8::Isolate::New(create_params);

//     { // anonymous scope for managing handle scope
//         v8::Isolate::Scope isolate_scope(isolate); // must enter the virtual machine to do stuff
//         v8::HandleScope handle_scope(isolate);

// 		// Best practice to isntall all global functions in the context ahead of time.
//         v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
//         // Bind the global 'print' function to the C++ Print callback.
//         global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, v8helpers::Print));
// 		// Bind the global static factory function for creating new GameObject instances
// 		// global->Set(isolate, "gameobjectfactory", v8::FunctionTemplate::New(isolate, GameObject::ScriptedGameObjectFactory));
// 		// Bind the global static function for retrieving object handles
// 		global->Set(isolate, "gethandle", v8::FunctionTemplate::New(isolate, ScriptManager::getHandleFromScript));
//         v8::Local<v8::Context> default_context =  v8::Context::New(isolate, NULL, global);
// 		v8::Context::Scope default_context_scope(default_context); // enter the context

//         ScriptManager *sm = new ScriptManager(isolate, default_context); 

// 		// Without parameters, these calls are made to the default context
//         sm->addScript("hello_world", "scripting/scripts/hello_world.js");
//         // sm->addScript("perform_function", "scripts/perform_function.js");

// 		// Create a new context
// 		// v8::Local<v8::Context> object_context = v8::Context::New(isolate, NULL, global);
// 		// sm->addContext(isolate, object_context, "object_context");

// 		// GameObject *go = new GameObject();
// 		// go->exposeToV8(isolate, object_context);

// 		// With the "object_context" parameter, these scripts are put in a
// 		// different context than the prior three scripts
// 		// sm->addScript("create_object", "scripts/create_object.js", "object_context");
// 		// sm->addScript("random_object", "scripts/random_object.js", "object_context");
// 		// sm->addScript("random_position", "scripts/random_position.js", "object_context");
// 		// sm->addScript("modify_position", "scripts/modify_position.js", "object_context");

// 		// Use the following 4 lines in place of the above 4 lines as the
// 		// reference if you don't plan to use multiple contexts
// 		/* sm->addScript("create_object", "scripts/create_object.js"); */
// 		/* sm->addScript("random_object", "scripts/random_object.js"); */
// 		/* sm->addScript("random_position", "scripts/random_position.js"); */
// 		/* sm->addScript("modify_position", "scripts/modify_position.js"); */

// 		usage();

// 		bool reload = false;
// 		int c = '\n';
// 		do
// 		{
// 			std::cout << "v8 example: ";
// 			if(c!='\n')
// 			{
// 				if(c == '?')
// 				{
// 					usage();
// 				}
// 				else if(c == 'h')
// 				{
// 					sm->runOne("hello_world", reload);
// 					reload = false;
// 				}
// 			// 	else if(c == 'p')
// 			// 	{
// 			// 		sm->runOne("perform_function", reload);
// 			// 		reload = false;
// 			// 	}
// 			// 	else if(c =='c')
// 			// 	{
// 			// 		sm->runOne("create_object", reload, "object_context");
// 			// 		reload = false;
// 			// 	}
// 			// 	else if(c == 'm')
// 			// 	{
// 			// 		std::cout << "Native position before: (" << go->x << ", " << go->y << ")"  << std::endl;
// 			// 		sm->runOne("modify_position", reload, "object_context");
// 			// 		std::cout << "Native position after: (" << go->x << ", " << go->y << ")"  << std::endl;
// 			// 		reload = false;
// 			// 	}
// 			// 	else if(c == 'r')
// 			// 	{
// 			// 		reload = true;
// 			// 	}
// 			// 	else if(c == 'l')
// 			// 	{
// 			// 		std::cout << "List of current GameObjects" << std::endl;
// 			// 		for(GameObject *g : GameObject::game_objects)
// 			// 		{
// 			// 			std::cout << "\t" << g->guid << ": " << g->x << ", " << g->y << std::endl;
// 			// 		}
// 			// 	}
// 			// 	else if(c == 'a')
// 			// 	{
// 			// 		sm->runOne("random_position", reload, "object_context");
// 			// 	}
// 			// 	else if(c == 'o')
// 			// 	{
// 			// 		sm->runOne("random_object", reload, "object_context");
// 			// 	}
// 			}
// 			c = getchar();
// 		} while(c!='q');
//     }

//     isolate->Dispose();
//     v8::V8::Dispose();
//     v8::V8::ShutdownPlatform();

// 	return 0;
// }
