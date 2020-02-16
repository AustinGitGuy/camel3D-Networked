#pragma once
#include "c3Event.h"

class MoveObjectEvent : Event
{
public:
	MoveObjectEvent();
	~MoveObjectEvent();

	void dispatch();

	void MoveObject(float newX, float newY, float newZ);

private:



};

MoveObjectEvent::MoveObjectEvent()
{
}

MoveObjectEvent::~MoveObjectEvent()
{
}

void MoveObjectEvent::dispatch()
{

}

void MoveObjectEvent::MoveObject(float newX, float newY, float newZ)
{
	//Set objects new transformposition equal to inputted values

}
