#pragma once

#include <queue>

class EventManager {

public:
    static void Init();
    static void Cleanup();

    static EventManager* GetInstance();

    void LoadManager();
    void DeleteManager();

private:
    static EventManager* instance;
    EventManager(){}
    ~EventManager(){}

    bool loaded = false;
};