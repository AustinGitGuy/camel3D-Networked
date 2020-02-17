#pragma once
#include "c3Event.h"

#ifndef STRETCHOBJECT_EVENT
#define STRETCHOBJECT_EVENT

//Include a3_demosceneObject
class StretchObjectEvent: public Event
{
public:
	StretchObjectEvent();
	StretchObjectEvent(float newWidthScale, float newHeightScale, float newDepthScale);
	~StretchObjectEvent();
	
	void dispatch();
	void StretchObject(float newWidth, float newHeight, float newDepth);

private:

	float mScaleWidth;
	float mScaleHeight;
	float mDepthScale;

};

StretchObjectEvent::StretchObjectEvent()
{
	mScaleHeight = 1.0;
	mScaleWidth = 1.0;
	mDepthScale = 1.0;
}

StretchObjectEvent::StretchObjectEvent(float newWidthScale, float newHeightScale, float newDepthScale)
{
	mScaleHeight = newHeightScale;
	mScaleWidth = newWidthScale;
	mDepthScale = newDepthScale;
}

StretchObjectEvent::~StretchObjectEvent()
{
}

//Sends event into queue????  or Activate Event's functionality?????
void StretchObjectEvent::dispatch()
{
	StretchObject(mScaleWidth, mScaleHeight, mDepthScale);
}

void StretchObjectEvent::StretchObject(float newWidth, float newHeight, float newDepth)
{
	//stretch height and width for object
}

#endif