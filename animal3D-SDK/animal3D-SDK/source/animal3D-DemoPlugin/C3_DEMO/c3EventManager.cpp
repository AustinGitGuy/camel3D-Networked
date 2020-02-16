#include "c3EventManager.h"

EventManager* EventManager::instance = nullptr;

void EventManager::Init(){
	if(instance == nullptr){
		instance = new EventManager();
	}
}

void EventManager::Cleanup(){
	if(instance != nullptr){
		delete instance;
		instance = nullptr;
	}
}

EventManager* EventManager::GetInstance(){
	return instance;
}

void EventManager::LoadManager(){
	if(!loaded){
		//Load the stuff here
		loaded = true;
	}
}

void EventManager::DeleteManager(){
	if(loaded){
		//Delete the stuff here
		loaded = false;
	}
}

//removes event and returns a pointer to that event
Event* EventManager::PopEvent(){
	Event* event = eventQueue.back();
	eventQueue.pop();
	return event;
}

//Adds event to queue
void EventManager::PushEvent(Event* event){
	eventQueue.push(event);
}

void EventManager::ProcessEvents(){
	while(eventQueue.size() != 0){
		PopEvent()->dispatch();
	}
}