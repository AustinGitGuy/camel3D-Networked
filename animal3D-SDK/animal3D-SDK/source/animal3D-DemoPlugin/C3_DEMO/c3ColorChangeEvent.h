#pragma once
#include "c3Event.h"

class ColorChangeEvent : Event
{
public:
	ColorChangeEvent();
	ColorChangeEvent(float r, float g, float b);
	~ColorChangeEvent();

	void dispatch();

	void ColorChange(float r, float g, float b);

private:

	EventType mEventID;
	float mRed;
	float mGreen;
	float mBlue;

};

ColorChangeEvent::ColorChangeEvent()
{
	mEventID = COLOR_EVENT_ID;
	mRed = 0.0;
	mGreen = 0.0;
	mBlue = 0.0;
}

//Overload for Default constructor.  Takes in information for objects new color
ColorChangeEvent::ColorChangeEvent(float r, float g, float b)
{
	mEventID = COLOR_EVENT_ID;
	mRed = r;
	mGreen = g;
	mBlue = b;
}

ColorChangeEvent::~ColorChangeEvent()
{
}

//Sends event into queue????  or Activate Event's functionality?????
void ColorChangeEvent::dispatch()
{
	ColorChange(mRed, mGreen, mBlue);
}


void ColorChangeEvent::ColorChange(float r, float g, float b)
{
	//change Object's color to the inputted RGB color


}
