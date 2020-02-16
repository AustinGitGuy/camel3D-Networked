#pragma once
//#include <a3_GeometryData.h>

//Event ID Types
enum EventType
{
	DEFAULT_EVENT_ID, MOVE_EVENT_ID, SCALE_EVENT_ID, COLOR_EVENT_ID
};

//Base Event Class
class Event {

public:
	virtual void dispatch() = 0;

private:
	
};