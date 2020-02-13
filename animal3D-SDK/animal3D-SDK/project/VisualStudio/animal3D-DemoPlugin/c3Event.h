#pragma once
//#include <a3_GeometryData.h>

class Event
{
public:
	Event();
	~Event();

	virtual void dispatch() = 0;

private:

};

Event::Event()
{
}

Event::~Event()
{
}