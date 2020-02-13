#pragma once
#include "c3Event.h"

class ColorChangeEvent : Event
{
public:
	ColorChangeEvent();
	~ColorChangeEvent();

	void dispatch();

	void ColorChange(float r, float g, float b);

private:

};

ColorChangeEvent::ColorChangeEvent()
{
}

ColorChangeEvent::~ColorChangeEvent()
{
}

void ColorChangeEvent::ColorChange(float r, float g, float b)
{
	//change Object's color to the inputted RGB color


}

void ColorChangeEvent::dispatch() 
{

}
