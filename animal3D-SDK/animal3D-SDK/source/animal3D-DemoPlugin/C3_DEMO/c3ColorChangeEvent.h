#pragma once
#include "c3Event.h"

#ifndef COLORCHANGE_EVENT
#define COLORCHANGE_EVENT

class ColorChangeEvent : public Event
{
public:
	ColorChangeEvent();
	ColorChangeEvent(float r, float g, float b);
	~ColorChangeEvent();

	void dispatch();

	void ColorChange(float r, float g, float b);

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
}

//Overload for Default constructor.  Takes in information for objects new color
ColorChangeEvent::ColorChangeEvent(float r, float g, float b)
{
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

#endif