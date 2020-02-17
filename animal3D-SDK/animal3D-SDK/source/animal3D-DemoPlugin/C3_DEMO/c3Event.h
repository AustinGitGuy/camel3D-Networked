#pragma once
//#include <a3_GeometryData.h>

#include "C3_DEMO/c3DemoState.h"

//Base Event Class
class Event {

public:
	virtual void dispatch(c3_DemoState* demoState, EventTypeStruct* event) = 0;
	EventType type;

	bool sent = false;

private:
	
};