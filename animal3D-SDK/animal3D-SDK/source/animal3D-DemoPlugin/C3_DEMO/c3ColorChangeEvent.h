#pragma once
#include "c3Event.h"

#ifndef COLORCHANGE_EVENT_H
#define COLORCHANGE_EVENT_H

class ColorChangeEvent : public Event
{
public:
	ColorChangeEvent();
	ColorChangeEvent(float r, float g, float b);
	~ColorChangeEvent();

	void dispatch(c3_DemoState* demoState, EventTypeStruct* event);

private:
	float mRed;
	float mGreen;
	float mBlue;

};

ColorChangeEvent::ColorChangeEvent()
{
	mRed = 0.0;
	mGreen = 0.0;
	mBlue = 0.0;
	type = EventType::COLORCHANGE_EVENT;
}

//Overload for Default constructor.  Takes in information for objects new color
ColorChangeEvent::ColorChangeEvent(float r, float g, float b)
{
	mRed = r;
	mGreen = g;
	mBlue = b;
	type = EventType::COLORCHANGE_EVENT;
}

ColorChangeEvent::~ColorChangeEvent()
{
}

//Sends event into queue????  or Activate Event's functionality?????
void ColorChangeEvent::dispatch(c3_DemoState* demoState, EventTypeStruct* event)
{
	demoState->red = mRed;
	demoState->green = mGreen;
	demoState->blue = mBlue;

	event->float1 = mRed;
	event->float2 = mGreen;
	event->float3 = mBlue;
}

#endif