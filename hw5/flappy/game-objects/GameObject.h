#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include <utility>
#include <v8.h>


#define TYPE_DEATH_ZONE "deathzone"
#define TYPE_SPAWN_POINT "spawnpoint"
#define TYPE_PLATFORM "platform"
#define TYPE_CHARACTER "character"
#define TYPE_VIEW_BOUNDARY "viewboundary"

//forward declarations!
class CollisionComponent;
class MovementComponent;
class RenderComponent;

using json = nlohmann::json;

/**
 * GameObject represents any game object using a sf::Shape, a MovementComponent, a CollisionComponent, and a RenderComponent.
 */
class GameObject
{
	public:
		//construct GameObject with all components set to NULL
	    GameObject(std::string id, std::string type, sf::Shape *shape);
		//construct GameObject with given values for all fields
	    GameObject(std::string id, std::string type, sf::Shape *shape, MovementComponent *movementComponent, CollisionComponent *collisionComponent, RenderComponent *renderComponent);
		/*
		Construct a GameObject with no MovementComponent 
		and other fields determined by the given json,
		which should be in the following format:
		{
			{"id", id},
			{"type", string},
			{"drawn", bool},
			{"color", {red, green, blue, alpha}},
			{"solid", bool},
			{"shape", {height, width, x position, y position}},
		}
		 */
		GameObject(json serialization, sf::RenderWindow *window);
		void update(unsigned int delta);
		void updateJson(json update);
		CollisionComponent *collisionComponent;
		MovementComponent *movementComponent;
		RenderComponent *renderComponent;
		sf::Shape* getShape();
		std::string getType();
		std::string getId();
		/*
		converts game object into a json, which stores the type, shape, color, and some simple collision/rendering information.
		format is as follows:

		{
			{"id", id},
			{"type", string},
			{"drawn", bool},
			{"color", {red, green, blue, alpha}},
			{"solid", bool},
			{"shape", {height, width, x position, y position}},
		}

		*/
		json serialize();
		
		/*
		creates json object storing the id and position of the object.
		{
			{"id", string id},
			{"x", float position.x},
			{"y", float position.y},
		}
		*/
		json serializeUpdate();


		v8::Local<v8::Object> exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name = "default");
		v8::Local<v8::Object> exposeToV8(std::string name, v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name = "default");

	protected:
		sf::Shape *shape;
		std::string id;
		std::string type;

		//static getters/setters for scripting
	private:
		//lets javascript access the shape as string
		// static void getShapeCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
		// static void setShapeCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &info);
		static void setShapeCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
		static void getShapeCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
};