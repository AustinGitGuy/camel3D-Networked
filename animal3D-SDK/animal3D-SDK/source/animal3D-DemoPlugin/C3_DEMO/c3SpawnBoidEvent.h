#pragma once
#include "c3EventManager.h"
#include "c3Event.h"

#ifndef SPAWNBOID_EVENT
#define SPAWNBOID_EVENT

class SpawnBoidEvent : public Event
{
public:
	SpawnBoidEvent();
	~SpawnBoidEvent();

	void dispatch(c3_DemoState* demoState, EventTypeStruct* event);

};

SpawnBoidEvent::SpawnBoidEvent()
{
	type = EventType::SPAWN_EVENT;
}

SpawnBoidEvent::~SpawnBoidEvent()
{
}

void SpawnBoidEvent::dispatch(c3_DemoState* demoState, EventTypeStruct* event)
{
	if(demoState->serverType != DATA_PUSH) demoState->flock.addBird(Vector3(demoState->frameWidth, demoState->frameHeight));
}

#endif