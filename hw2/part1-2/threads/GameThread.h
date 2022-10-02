#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>
#include <iostream>

class GameThread
{
    bool busy;
    int id;
    GameThread *other;
    std::mutex *_mutex;
    std::condition_variable *_condition_variable;
};