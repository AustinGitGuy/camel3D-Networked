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
	
	void dispatch(c3_DemoState* demoState, EventTypeStruct* event);

private:

	float mScaleWidth;
	float mScaleHeight;
	float mScaleDepth;

};

StretchObjectEvent::StretchObjectEvent()
{
	mScaleHeight = 1.0;
	mScaleWidth = 1.0;
	mScaleDepth = 1.0;
	type = EventType::STRETCH_EVENT;
}

StretchObjectEvent::StretchObjectEvent(float newWidthScale, float newHeightScale, float newDepthScale)
{
	mScaleHeight = newHeightScale;
	mScaleWidth = newWidthScale;
	mScaleDepth = newDepthScale;
	type = EventType::STRETCH_EVENT;
}

StretchObjectEvent::~StretchObjectEvent()
{
}

//Sends event into queue????  or Activate Event's functionality?????
void StretchObjectEvent::dispatch(c3_DemoState* demoState, EventTypeStruct* event)
{
	demoState->xScale += mScaleWidth;
	demoState->yScale += mScaleHeight;


	if(demoState->xScale < 0.25f) demoState->xScale = .25f;
	if(demoState->yScale < 0.25f) demoState->yScale = .25f;

	event->float1 = mScaleWidth;
	event->float2 = mScaleHeight;
	event->float3 = mScaleDepth;
}

#endif