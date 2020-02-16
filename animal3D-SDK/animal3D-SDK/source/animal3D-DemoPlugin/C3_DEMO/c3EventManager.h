#pragma once

#include "c3Event.h"
#include <queue>

class EventManager {

public:
    static void Init();
    static void Cleanup();

    static EventManager* GetInstance();

    void LoadManager();
    void DeleteManager();

    Event* PopEvent();
    void PushEvent(Event* event);
    void ProcessEvents();

private:
    static EventManager* instance;
    EventManager(){}
    ~EventManager(){}
    std::queue<Event*> eventQueue;

    bool loaded = false;
};