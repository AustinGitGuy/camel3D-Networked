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

void EventManager::LoadManager(c3_DemoState* newState){
	if(!loaded){
		//Load the stuff here
		demoState = newState;
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

	EventStruct send;
	send.id = GameMessages::DEFAULT_EVENT_ID;
	int iter = 0;

	while(eventQueue.size() != 0){
		Event* event = PopEvent();
		if(event->type < NUM_EVENT_TYPES && event->type > -1 && !event->sent){
			send.events[iter].type = event->type;
			event->dispatch(demoState, &send.events[iter]);
			iter++;
			delete event;
		}
		else if(event->type < NUM_EVENT_TYPES && event->type > -1){
			delete event;
		}
	}

	if(iter > 0){
		demoState->peer->Send((char*)&send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
	}
}