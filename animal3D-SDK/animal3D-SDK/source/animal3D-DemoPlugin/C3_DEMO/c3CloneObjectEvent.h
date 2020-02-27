#pragma once
#include "c3EventManager.h"
#include "c3Event.h"

#ifndef CLONEOBJECT_EVENT
#define CLONEOBJECT_EVENT

class CloneObjectEvent : public Event
{
public:
	CloneObjectEvent();
	~CloneObjectEvent();

	void dispatch(c3_DemoState* demoState, EventTypeStruct* event);

};

CloneObjectEvent::CloneObjectEvent()
{
	type = EventType::CLONE_EVENT;
}

CloneObjectEvent::~CloneObjectEvent()
{
}

void CloneObjectEvent::dispatch(c3_DemoState* demoState, EventTypeStruct* event)
{
	demoState->numCubes++;
}

#endif