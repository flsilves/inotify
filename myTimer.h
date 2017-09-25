#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>

using namespace std;

class Timer {

public:
    void start();

    void stop();

    void restart();

    double elapsedMilliseconds();

    double elapsedSeconds();

private:
    chrono::time_point<std::chrono::system_clock> m_StartTime;
    chrono::time_point<std::chrono::system_clock> m_EndTime;
    bool m_bRunning = false;
};

#endif
