#pragma once

#include <utility>
#include <v8.h>

#define GO_DEBUG 0


/**
 * Simple class to demonstrate Interceptors, Accessors, Handles, and object
 * creation in v8
 *
 * NOTE: I've used DemoObject to help you think about the functionality as it
 * relates to scripting, but this could be any kind of objet (i.e., Events)..
 */
class DemoObject
{
	public:
		DemoObject();
		~DemoObject();

		std::string guid;
		int x;
		int y;

		/**
		 * This function will make this class instance accessible to scripts in
		 * the given context. 
		 *
		 * IMPORTANT: Please read this definition of this function in
		 * DemoObject.cpp. The helper function I've provided expects certain
		 * parameters which you must use in order to take advance of this
		 * convinience. 
		 */
		v8::Local<v8::Object> exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name="default");

		/**
		 * Static function to keep track of current total number of
		 * DemoObjects, to facilitate creating GUIDs by incrementing a counter.
		 */
		static int getCurrentGUID();

		/**
		 * Shared list of all instances of game objects...auto populated by the
		 * constructor.
		 */
		static std::vector<DemoObject*> game_objects;

		/**
		 * Factor methods for creating new instances of DemoObjects.
		 *
		 * This is primarily a demonstration of how you can create new objects
		 * in javascript. NOTE: this is embedded in this DemoObject class, but
		 * could (and likely should) be used for other functionality as well
		 * (i.e., Events). 
		 */
		static DemoObject* DemoObjectFactory(std::string context_name="default");
		static void ScriptedDemoObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args);

	private:

		/** Static variable to keep count of current number of DemoObject instances */
		static int current_guid;

		v8::Isolate* isolate;
		v8::Global<v8::Context>* context;
		
		/**
		 * NOTE: These "Accessors" have to be **static**
		 *
		 * You will need to implement a setter and getter for every class
		 * member variable you want accessible to javascript.
		 */
		static void setDemoObjectX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
		static void getDemoObjectX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
		static void setDemoObjectY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
		static void getDemoObjectY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
		static void setDemoObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
		static void getDemoObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type

};

