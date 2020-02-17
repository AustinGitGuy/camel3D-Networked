#pragma once
#include "c3EventManager.h"
#include "c3Event.h"

#ifndef MOVEOBJECT_EVENT
#define MOVEOBJECT_EVENT

class MoveObjectEvent : public Event
{
public:
	MoveObjectEvent();
	MoveObjectEvent(float newX, float newY, float newZ);
	~MoveObjectEvent();

	void dispatch(c3_DemoState* demoState, EventTypeStruct* event);

private:
	float mXPos, mYPos, mZPos;

};

MoveObjectEvent::MoveObjectEvent()
{
	type = EventType::MOVE_EVENT;
	mXPos = 0.0;
	mYPos = 0.0;
	mZPos = 0.0;
}

//Overload for Default constructor.  Takes in information for objects new position
MoveObjectEvent::MoveObjectEvent(float newX, float newY, float newZ)
{
	type = EventType::MOVE_EVENT;
	mXPos = newX;
	mYPos = newY;
	mZPos = newZ;

}

MoveObjectEvent::~MoveObjectEvent()
{
}

//Sends event into queue????  or Activate Event's functionality?????
void MoveObjectEvent::dispatch(c3_DemoState* demoState, EventTypeStruct* event)
{
	demoState->xPos += mXPos;
	demoState->yPos += mYPos;

	event->float1 = mXPos;
	event->float2 = mYPos;
	event->float3 = mZPos;
}

#endif