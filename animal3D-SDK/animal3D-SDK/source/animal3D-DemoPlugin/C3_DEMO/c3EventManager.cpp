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

	}
}

void EventManager::DeleteManager(){
	if(loaded){
		//Delete the stuff here

	}
}