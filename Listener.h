#ifndef INOTIFY_LISTENER_H
#define INOTIFY_LISTENER_H

#define BUF_LEN (1000 * (sizeof(struct inotify_event) + NAME_MAX + 1))

class Listener {

public:
    Listener(string folder)
private:

    char buffer[BUF_LEN];
    Timer clock;


};







class Timer
{

public:
    void start() {
        m_StartTime = std::chrono::system_clock::now();
        m_bRunning = true;
    }

    void stop() {
        m_EndTime = std::chrono::system_clock::now();
        m_bRunning = false;
    }

    void restart() {
        stop();
        start();
    }

    double elapsedMilliseconds() {
        std::chrono::time_point<std::chrono::system_clock> endTime;

        if(m_bRunning) {
            endTime = std::chrono::system_clock::now();
        }
        else {
            endTime = m_EndTime;
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
    }

    double elapsedSeconds() {
        return elapsedMilliseconds() / 1000.0;
    }

private:
    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    bool m_bRunning = false;
};
#endif //INOTIFY_LISTENER_H
