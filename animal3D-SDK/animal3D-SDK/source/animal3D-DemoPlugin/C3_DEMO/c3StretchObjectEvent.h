#pragma once
#include "c3Event.h"

//Include a3_demosceneObject
class StretchObjectEvent: Event
{
public:
	StretchObjectEvent();
	StretchObjectEvent(float newWidthScale, float newHeightScale);
	~StretchObjectEvent();
	
	void dispatch();
	void StretchObject(float newWidth, float newHeight);

private:

	EventType mEventId;
	float mScaleWidth;
	float mScaleHeight;

};

StretchObjectEvent::StretchObjectEvent()
{
	mEventId = SCALE_EVENT_ID;
	mScaleHeight = 1.0;
	mScaleWidth = 1.0;

}

StretchObjectEvent::StretchObjectEvent(float newWidthScale, float newHeightScale)
{
	mEventId = SCALE_EVENT_ID;
	mScaleHeight = newHeightScale;
	mScaleWidth = newWidthScale;

}

StretchObjectEvent::~StretchObjectEvent()
{
}

//Sends event into queue????  or Activate Event's functionality?????
void StretchObjectEvent::dispatch()
{
	StretchObject(mScaleWidth, mScaleHeight);
}

void StretchObjectEvent::StretchObject(float newWidth, float newHeight)
{
	//stretch height and width for object
}
