#pragma once
#include "c3Event.h"

//Include a3_demosceneObject
class StretchObjectEvent: Event
{
public:
	StretchObjectEvent();
	~StretchObjectEvent();
	
	void dispatch();
	void StretchObject(float newWidth, float newHeight);

private:

};

StretchObjectEvent::StretchObjectEvent()
{
}

StretchObjectEvent::~StretchObjectEvent()
{
}

void StretchObjectEvent::dispatch()
{

}

void StretchObjectEvent::StretchObject(float newWidth, float newHeight)
{
	//stretch height and width for object
}
