#pragma once
#include "c3EventManager.h"
#include "c3Event.h"




class MoveObjectEvent : Event
{
public:
	MoveObjectEvent();
	MoveObjectEvent(float newX, float newY, float newZ);
	~MoveObjectEvent();

	void dispatch();

	void MoveObject(float newX, float newY, float newZ);

private:

	EventType mEventID;
	float mXPos, mYPos, mZPos;

};

MoveObjectEvent::MoveObjectEvent()
{
	mEventID = MOVE_EVENT_ID;
	mXPos = 0.0;
	mYPos = 0.0;
	mZPos = 0.0;

}

//Overload for Default constructor.  Takes in information for objects new position
MoveObjectEvent::MoveObjectEvent(float newX, float newY, float newZ)
{
	mEventID = MOVE_EVENT_ID;
	mXPos = newX;
	mYPos = newY;
	mZPos = newZ;

}

MoveObjectEvent::~MoveObjectEvent()
{
}

//Sends event into queue????  or Activate Event's functionality?????
void MoveObjectEvent::dispatch()
{
	MoveObject(mXPos, mYPos, mZPos);
}

void MoveObjectEvent::MoveObject(float newX, float newY, float newZ)
{
	//Set objects new transformposition equal to inputted values

}
