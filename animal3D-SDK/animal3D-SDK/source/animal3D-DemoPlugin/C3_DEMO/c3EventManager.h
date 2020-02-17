#pragma once

#include "c3Event.h"
#include <queue>
#include "C3_DEMO/c3DemoState.h"

class EventManager {

public:
    static void Init();
    static void Cleanup();

    static EventManager* GetInstance();

    void LoadManager(c3_DemoState* newState);
    void DeleteManager();

    Event* PopEvent();
    void PushEvent(Event* event);
    void ProcessEvents();

private:
    static EventManager* instance;
    EventManager(){}
    ~EventManager(){}
    std::queue<Event*> eventQueue;

    c3_DemoState* demoState;

    bool loaded = false;
};